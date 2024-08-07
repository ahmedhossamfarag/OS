#include "elf.h"
#include "libc.h"
#include "info.h"
#include "math.h"
#include "strlib.h"

static inline uint8_t elf_check_file(Elf32_Ehdr* ehdr){
    return ehdr->e_ident[0] == EI_MAG0 && ehdr->e_ident[1] == EI_MAG1 && 
        ehdr->e_ident[2] == EI_MAG2 && ehdr->e_ident[3] == EI_MAG3;
}

uint8_t elf_check_supported(Elf32_Ehdr* ehdr){
    return ehdr->e_machine == EM_386 && ehdr->e_ident[EI_CLASS] == ELFCLASS32 &&
        ehdr->e_ident[EI_DATA] == ELFDATA2LSB && ehdr->e_version >= EV_CURRENT;
}

uint8_t elf_check_executable(Elf32_Ehdr* ehdr){
    return ehdr->e_type == ET_EXEC || ehdr->e_type == ET_DYN;
}

static inline void elf_get_ehdr(Elf32_Map *map, char *file)
{
    Elf32_Ehdr* ehdr = (Elf32_Ehdr*) file;
    if(elf_check_file(ehdr)){
        map->ehdr = ehdr;
    }else{
        map->ehdr = 0;
    }
}

static inline void elf_get_shdr(Elf32_Map *map, char *file)
{
    if(map->ehdr->e_shoff){
        map->shdr = (Elf32_Shdr*)(file + map->ehdr->e_shoff);
        if(map->ehdr->e_shnum){
            map->nshdr = map->ehdr->e_shnum;
        }else{
            map->nshdr = map->shdr->sh_size;
        }
    }else{
        map->shdr = 0;
        map->nshdr = 0;
    }
}

static inline void elf_get_phdr(Elf32_Map *map, char *file)
{
    if(map->ehdr->e_phoff){
        map->phdr = (Elf32_Phdr*)(file + map->ehdr->e_phoff);
        if(map->ehdr->e_phnum != PN_XNUM){
            map->nphdr = map->ehdr->e_phnum;
        }else{
            map->nphdr = map->shdr->sh_info;
        }
    }else{
        map->phdr = 0;
        map->nphdr = 0;
    }
}

static inline void elf_get_str(Elf32_Map *map, char *file)
{
    if(map->ehdr->e_shstrndx != SHN_UNDEF){
        if(map->ehdr->e_shstrndx != SHN_XINDEX){
            map->str = file + map->shdr[map->ehdr->e_shstrndx].sh_offset;
        }else{
            map->str = file + map->shdr[map->shdr->sh_link].sh_offset;
        }
    }else{
        map->str = 0;
    }
}

void elf_get_map(Elf32_Map *map, char *file)
{
    elf_get_ehdr(map, file);
    if(map->ehdr){
        elf_get_shdr(map, file);
        if(map->shdr){
            elf_get_phdr(map, file);
            elf_get_str(map, file);
        }
    }
}

char* elf_get_str_section(Elf32_Map* map, uint32_t shindx){
    char* file = (char*) map->ehdr;
    return file + map->shdr[shindx].sh_offset;
}

void* elf_get_table(Elf32_Map* map, Elf32_Shdr* shdr){
    char* file = (char*) map->ehdr;
    return (Elf32_Sym*) (file + shdr->sh_offset);
}

Elf32_Shdr* elf_get_sheader(Elf32_Map* map, uint32_t shindx){
    return &map->shdr[shindx];
}

uint32_t elf_get_num_entries(Elf32_Shdr* shdr){
    return shdr->sh_size / shdr->sh_entsize;
}

/* Check memory region does not intersect with kernel region and drivers region */
static inline uint8_t elf_check_mregion(uint32_t offset, uint32_t size){
    #define between(x, a, b) ((x) >= (a) && (x) <= (b))
    #define intersect(x,y,a,b) (between(x, a, b) || between(y, a, b) || between(a, x, y)) 
    if(intersect(offset, offset+size, KERNEL_OFFSET, KERNEL_END-1)) return 0;
    if(intersect(offset, offset+size, DRIVERS_OFFSET, 0xFFFFFFFF)) return 0;
    return 1;
}

uint8_t elf_load_file(Elf32_Map* map, uint32_t* offset){
    uint32_t org = *offset;
    map->org = org;

    char* file = (char*) map->ehdr;

    uint32_t max_offset = 0; 

    // copy program sections
    Elf32_Phdr* phdr = map->phdr;
    for (uint32_t i = 0; i < map->nphdr; i++)
    {
        if(phdr->p_type == PT_LOAD){
            // check memory region
            if(!elf_check_mregion(org + phdr->p_vaddr , phdr->p_memsz)) return 0;

            // copy data
            mem_copy(file + phdr->p_offset, (char*)org + phdr->p_vaddr, math_min(phdr->p_memsz, phdr->p_filesz));
            if(phdr->p_memsz > phdr->p_filesz){
                mem_set((char*)org + phdr->p_filesz, 0, phdr->p_memsz - phdr->p_filesz);
            }

            // updata max offset in memory
            max_offset = math_max(max_offset, org + phdr->p_vaddr + phdr->p_memsz);
        }
        phdr ++ ;
    }

    // set bss sections to zero
    Elf32_Shdr* shdr = map->shdr;
    for (uint32_t i = 0; i < map->nshdr; i++)
    {
        if(shdr->sh_type == SHT_NOBITS){
            // check memory region
            if(!elf_check_mregion(org + shdr->sh_addr, shdr->sh_size)) return 0;

            // set region to 0
            mem_set((char*)org + shdr->sh_addr, 0, shdr->sh_size);

            // updata max offset int memory
            max_offset = math_max(max_offset, org + shdr->sh_addr + shdr->sh_size);
        }

        shdr ++;
    }

    // make offset multiple of 4k
    *offset = math_cielm(org + max_offset, 0x1000);

    return 1;
}

static void elf_get_dyn_dependecies(Elf32_Map* map, Elf32_Shdr* shdr, array_t* arr){
    Elf32_Dyn* dyn = elf_get_table(map, shdr);
    int n = elf_get_num_entries(shdr);

    char *str = 0;
    for (int i = 0; i < n; i++)
    {
        if(dyn[i].d_tag == DT_STRTAB){
            str = ((char*)map->ehdr) + dyn[i].d_un.d_val;
            break;
        }
    }

    if(!str){
        return;
    }

    for (int i = 0; i < n; i++)
    {
        if(dyn->d_tag == DT_NEEDED){
            array_add(arr, str + dyn->d_un.d_val);
        }
        dyn ++;
    }
}

void elf_get_dependecies(Elf32_Map* map, array_t* arr){
    for (int i = 0; i < map->nshdr; i++)
    {
        Elf32_Shdr* shdr = map->shdr + i;
        if(shdr->sh_type == SHT_DYNAMIC){
            elf_get_dyn_dependecies(map, shdr, arr);
        }
    }
}

static uint32_t elf_st_lookup_sym(Elf32_Map* map, Elf32_Shdr* shdr, char* name){
    char* str = elf_get_str_section(map, shdr->sh_link);
    int n = elf_get_num_entries(shdr);

    Elf32_Sym* sym = (Elf32_Sym*)elf_get_table(map, shdr);
    for (int i = 0; i < n; i++)
    {
        if(str_cmp(name, str + sym->st_name) == 0) return sym->st_value;

        sym ++;
    }
    return 0;
}

uint32_t elf_lookup_sym(Elf32_Map* map, char* name){
    Elf32_Shdr* shdr = map->shdr;

    for (uint32_t i = 0; i < map->nshdr; i++)
    {
        if(shdr->sh_type == SHT_SYMTAB){
            uint32_t value = elf_st_lookup_sym(map, shdr, name);
            if(value) return value;

        }
        shdr ++;
    }
    
    return 0;
}

static uint8_t elf_get_st_value(Elf32_Map* map, Elf32_Shdr* shdr, uint32_t indx, Elf32_Dependecies deps, uint32_t* st_value){

    int n = elf_get_num_entries(shdr);
    if(indx >= n) return 0;

    Elf32_Sym* sym = (Elf32_Sym*)elf_get_table(map, shdr) + indx;
    if(sym->st_shndx == SHN_UNDEF && deps.libs){
        char* str = elf_get_str_section(map, shdr->sh_link);
        *st_value = 0;
        for (uint32_t i = 0; i < deps.nlibs; i++)
        {
            Elf32_Map* lib = deps.libs + i;
            uint32_t value = elf_lookup_sym(lib, str + sym->st_name);
            if(value){
                *st_value = value + lib->org;
                break;
            }
        }
    }else{
        *st_value = sym->st_value;
    }

    return 1;
}

static uint8_t elf_do_section_rel(Elf32_Map* map, Elf32_Shdr* shdr, Elf32_Dependecies deps){
    Elf32_Shdr* st = map->shdr + shdr->sh_link;
    Elf32_Rel* rel = elf_get_table(map, shdr);
    int n = elf_get_num_entries(shdr);

    for (int i = 0; i < n; i++)
    {
        uint32_t sindx = ELF32_R_SYM(rel->r_info);
        uint32_t rtype = ELF32_R_TYPE(rel->r_info);
        uint32_t* target_address = (uint32_t*) (map->org + rel->r_offset);
        uint32_t st_value = 0;
        if(sindx){
            elf_get_st_value(map, st, sindx, deps, &st_value);
        }
        switch (rtype)
        {
        case R_386_GLOB_DAT: {
            *target_address = map->org + st_value;
            break;
        }
        case R_386_JMP_SLOT: {
            *target_address = st_value;
        }
        case R_386_RELATIVE: {
            *target_address = map->org + *target_address;
            break;
        }
        default:
            break;
        }
        rel ++;
    }
}

uint8_t elf_do_rel(Elf32_Map* map, Elf32_Dependecies deps){
    Elf32_Shdr* shdr = map->shdr;
    for (uint32_t i = 0; i < map->nshdr; i++)
    {
        if(shdr->sh_type == SHT_REL){
            if(!elf_do_section_rel(map, shdr, deps)) return 0;
        }
        shdr ++;
    }
    return 1;
}
