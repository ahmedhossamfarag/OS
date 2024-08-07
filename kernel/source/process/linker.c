#include "elf.h"
#include "memory.h"
#include "file_system.h"
#include "dslib.h"


static struct
{
    uint32_t cr3;
    char* file;
    uint32_t* entry;
    uint32_t* membegin;
    Elf32_Map fmap;
    Elf32_Dependecies deps;
    array_t* deps_names;
    fs_entity_t** deps_fs;
    fs_entity_t* rootdir;
    char** deps_data;
    uint32_t loaded_deps;
    uint32_t open_deps;
    uint32_t max_offset;
    SUCC_ERR_V
} args;

static void linker_free(){
    uint32_t ndeps = args.deps.nlibs;

    if(args.deps_names){
        array_delete(args.deps_names, free);
    }

    if(args.deps.libs){
        free((char*)args.deps.libs, ndeps*sizeof(Elf32_Map));
    }

    if(args.deps_data){
        if(args.loaded_deps){
            for (uint32_t i = 0; i < args.loaded_deps - 1; i++){
                if(args.deps_data[i]){
                    free(args.deps_data[i], ((file_entity_t*)args.deps_fs[i])->n_blocks*SectorSize);
                }
            } 
        }       
        free((char*)args.deps_data, ndeps*sizeof(char*));
    }

    if(args.deps_fs){
        if(args.open_deps){
            for (uint32_t i = 0; i < args.deps.nlibs; i++){
                if(args.deps_fs[i]){
                    file_close(args.deps_fs[i], 0, 0);
                }
            }
        }
        free((char*)args.deps_fs, ndeps*sizeof(fs_entity_t*));
    }

    if(args.rootdir){
        file_close(args.rootdir, 0, 0);
    }
}

static void linker_error(){
    linker_free();
    if(args.error_proc){
        args.error_proc();
    }
}

static void linker_success(){
    linker_free();
    if(args.entry){
        *args.entry = args.fmap.ehdr->e_entry;
    }
    if(args.membegin){
        *args.membegin = args.max_offset;
    }
    if(args.success_proc){
        args.success_proc();
    }
}

static void linker_process(){
    uint32_t current_cr3;
    asm("mov %%cr3, %0":"=r"(current_cr3));
    uint32_t th_cr3 = args.cr3;
    asm volatile("mov %0, %%cr3" :: "r"(th_cr3));

    // create maps
    for (uint32_t i = 0; i < args.deps.nlibs; i++)
    {
        elf_get_map(args.deps.libs + i, args.deps_data[i]);
        Elf32_Map* map = args.deps.libs + i;
        if(!map->ehdr || !map->shdr || !map->phdr){
            linker_error();
            return;
        }
    }

    // load program sections
    args.max_offset = 0;
    if(!elf_load_file(&args.fmap, &args.max_offset)){
        linker_error();
        return;
    }
    for (uint32_t i = 0; i < args.deps.nlibs; i++)
    {
        if(!elf_load_file(args.deps.libs + i, &args.max_offset)){
            linker_error();
            return;
        }
    }

    // relocation
    elf_do_rel(&args.fmap, args.deps);



    Elf32_Dependecies def_deps = {0,0};
    for (uint32_t i = 0; i < args.deps.nlibs; i++)
    {
        elf_do_rel(args.deps.libs + i, def_deps);
    }

    asm volatile("mov %0, %%cr3" :: "r"(current_cr3));

    linker_success();
}

static void linker_load_next_dependency(){
    args.loaded_deps ++;
    if(args.loaded_deps > args.deps.nlibs){
        linker_process();
        return;
    }
    uint32_t i = args.loaded_deps - 1;
    fs_entity_t* fs = args.deps_fs[i];
    if(fs->type != FILE_TYPE){
        linker_error();
        return;
    }
    char* data = alloc(((file_entity_t*)fs)->n_blocks*SectorSize);
    if(!data){
        linker_error();
        return;
    }
    args.deps_data[i] = data;
    
    file_read((file_entity_t*)fs, data, 0, ((file_entity_t*)fs)->n_blocks, linker_load_next_dependency, linker_error);
}

static void linker_open_next_dependency(){
    args.open_deps ++;
    if(args.open_deps > args.deps.nlibs){
        linker_load_next_dependency();
        return;
    }
    uint32_t i = args.open_deps - 1;
    file_open((dir_entity_t*)args.rootdir, args.deps_names->data[i], args.deps_fs + i, linker_open_next_dependency, linker_error);
}

static void linker_load_dependencies(){
    args.deps_fs = 0;
    args.rootdir = 0;
    args.deps_data = 0;
    args.loaded_deps = args.open_deps = 0;
    args.deps.libs = 0;
    args.deps_names = array_new(10, alloc);
    if(!args.deps_names){
        linker_error();
        return;
    }
    elf_get_dependecies(&args.fmap, args.deps_names);
    if(args.deps_names->size){
        uint32_t ndeps = args.deps_names->size;
        args.deps.nlibs = ndeps;
        args.deps.libs = (Elf32_Map*) alloc(ndeps*sizeof(Elf32_Map));
        args.deps_fs = (fs_entity_t**) alloc(ndeps*sizeof(fs_entity_t*));
        args.deps_data = (char**) alloc(ndeps*sizeof(char*));
        if(!args.deps.libs || !args.deps_fs || !args.deps_data){
            linker_error();
            return;
        }
        for (uint32_t i = 0; i < ndeps; i++)
        {
            args.deps_fs[i] = 0;
            args.deps_data[i] = 0;
        }
        
        file_open(0, 0, &args.rootdir, linker_open_next_dependency, linker_error);
    }else{
        linker_process();
    }
    
}

void linker_load_elf(char* file, uint32_t cr3, uint32_t* entry, uint32_t* membegin, SUCC_ERR){
    args.file = file;
    args.cr3 = cr3;
    args.entry = entry;
    args.membegin = membegin;
    args.success_proc = success_proc;
    args.error_proc = error_proc;

    elf_get_map(&args.fmap, file);

    Elf32_Map* map = &args.fmap;
    if(!map->ehdr || !map->shdr || !map->phdr || 
        !elf_check_supported(map->ehdr) || !elf_check_executable(map->ehdr))
    {
        if(error_proc)
            error_proc();
        return;
    }

    linker_load_dependencies();
}
 
 
 /*
 
    
#define view(name, value) print(name) println(value)
#define viewline(name1, value1, name2, value2) print(name1) print(value1) print(" & ") print(name2) println(value2)

static void linker_read_sym_table(Elf32_Map* map, Elf32_Shdr* shdr){
    if(shdr->sh_type != SHT_SYMTAB && shdr->sh_type != SHT_DYNSYM) return;
    char* str = elf_get_str_section(map, shdr->sh_link);
    int n = shdr->sh_size / shdr->sh_entsize;
    Elf32_Sym* sym = elf_get_table(map, shdr);
    for (int i = 0; i < n; i++)
    {
        viewline("Sym ", str + sym->st_name, "Value ", sxint(sym->st_value))
        // view("Value ", sxint(sym->st_value))
        // view("Section ", map->str + elf_get_sheader(map, sym->st_shndx)->sh_name)
        sym ++;
    }
}



static void linker_read_rel_table(Elf32_Map* map, Elf32_Shdr* shdr){
    linker_read_sym_table(map, map->shdr + shdr->sh_link);
    Elf32_Rel* rel = elf_get_table(map, shdr);
    int n = shdr->sh_size / shdr->sh_entsize;

    for (int i = 0; i < n; i++){
        uint32_t* target_address = (uint32_t*) rel->r_offset;
        viewline("Rel ", sxint(rel->r_info), sxint(target_address), sxint(*target_address))
        rel ++;
    }
}

static void linker_read_d_table(Elf32_Map* map, Elf32_Shdr* shdr){
    Elf32_Dyn* dyn = elf_get_table(map, shdr);
    int n = shdr->sh_size / shdr->sh_entsize;
    char *str;
    for (int i = 0; i < n; i++)
    {
        if(dyn->d_tag == DT_STRTAB){
            str = ((char*)map->ehdr) + dyn->d_un.d_val;
        }
        dyn ++;
    }
    dyn = elf_get_table(map, shdr);
    for (int i = 0; i < n; i++)
    {
        if(dyn->d_tag == DT_NEEDED){
        view(sxint(dyn->d_tag), str + dyn->d_un.d_val)

        }
        dyn ++;
    }
    
}
static void linker_read_sections(Elf32_Map* map){
    for (int i = 0; i < map->nshdr; i++)
    {
        Elf32_Shdr* shdr = map->shdr + i;
        // view("Shdr ", map->str + shdr->sh_name);
        switch (shdr->sh_type)
        {
        case SHT_SYMTAB:
            // linker_read_sym_table(map, shdr);
            break;
        case SHT_DYNSYM:
            // linker_read_sym_table(map, shdr);
        case SHT_REL:
            linker_read_rel_table(map, shdr);
        case SHT_DYNAMIC:
            // linker_read_d_table(map, shdr);
        default:
            break;
        }
    }
    
}

        
static void linker_print_ehdr(Elf32_Map* map){
    view("Ehdr ", sxint(map->ehdr))
    view("Type ", sint(map->ehdr->e_type))
    view("Shdr ", sxint(map->shdr))
    view("ShNum ", sint(map->nshdr))
    view("Phdr ", sxint(map->phdr))
    view("PhNum ", sint(map->nphdr))
    view("Str ", sxint(map->str));
}


void linker_load_file(char* file, char* lib){
    Elf32_Map fmap = {0};
    Elf32_Map lmap = {0};
    elf_get_map(&fmap, file);
    elf_get_map(&lmap, lib);
    // linker_print_ehdr(&fmap);
    // println("")
    // linker_print_ehdr(&lmap.map);
    if(fmap.ehdr && lmap.ehdr){
        // if(fmap.shdr){
        //     // linker_read_sections(&fmap);
        //     uint32_t org = 0;
        //     uint32_t offset = org;
        //     Elf32_Dependecies fdeps = {&lmap, 1};
        //     Elf32_Dependecies ldeps = {0, 0};
        //     println(sint(elf_load_file(&fmap, &offset)));
        //     println(sint(elf_load_file(&lmap, &offset)));
        //     println(sint(elf_do_rel(&fmap, fdeps)));
        //     println(sint(elf_do_rel(&lmap, ldeps)));
        //     uint32_t main = org + elf_lookup_sym(&fmap, "main");
            
            
        //     int (*entry)() = main;
        //     int x = entry();
        //     println(sxint(x));
        // }
        array_t* deps_names = array_new(10, alloc);
        elf_get_dependecies(&fmap, deps_names);
        println(sint(deps_names->size));
        for (int i = 0; i < deps_names->size; i++)
        {
            println(deps_names->data[i])
        }
        
    }
}
 */