#include "elf.h"
#include "libc.h"
#include "info.h"
#include "math.h"
#include "strlib.h"

static inline uint8_t elf_check_file(Elf64_Ehdr* ehdr){
    return ehdr->e_ident[0] == EI_MAG0 && ehdr->e_ident[1] == EI_MAG1 && 
        ehdr->e_ident[2] == EI_MAG2 && ehdr->e_ident[3] == EI_MAG3;
}

uint8_t elf_check_supported(Elf64_Ehdr* ehdr){
    return ehdr->e_ident[EI_CLASS] == ELFCLASS32 &&
        ehdr->e_ident[EI_DATA] == ELFDATA2LSB && ehdr->e_version >= EV_CURRENT;
}

uint8_t elf_check_executable(Elf64_Ehdr* ehdr){
    return ehdr->e_type == ET_EXEC || ehdr->e_type == ET_DYN;
}

static inline void elf_get_ehdr(Elf64_Map *map, char *file)
{
    Elf64_Ehdr* ehdr = (Elf64_Ehdr*) file;
    if(elf_check_file(ehdr)){
        map->ehdr = ehdr;
    }else{
        map->ehdr = 0;
    }
}

static inline void elf_get_shdr(Elf64_Map *map, char *file)
{
    if(map->ehdr->e_shoff){
        map->shdr = (Elf64_Shdr*)(file + map->ehdr->e_shoff);
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

static inline void elf_get_phdr(Elf64_Map *map, char *file)
{
    if(map->ehdr->e_phoff){
        map->phdr = (Elf64_Phdr*)(file + map->ehdr->e_phoff);
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

static inline void elf_get_str(Elf64_Map *map, char *file)
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

void elf_get_map(Elf64_Map *map, char *file)
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

char* elf_get_str_section(Elf64_Map* map, uint32_t shindx){
    char* file = (char*) map->ehdr;
    return file + map->shdr[shindx].sh_offset;
}

void* elf_get_table(Elf64_Map* map, Elf64_Shdr* shdr){
    char* file = (char*) map->ehdr;
    return (Elf64_Sym*) (file + shdr->sh_offset);
}

Elf64_Shdr* elf_get_sheader(Elf64_Map* map, uint32_t shindx){
    return &map->shdr[shindx];
}

uint32_t GLOBelf_get_num_entries(Elf64_Shdr* shdr){
    return shdr->sh_size / shdr->sh_entsize;
}

/* Check memory region does not intersect with kernel region and drivers region */
static inline uint8_t elf_check_mregion(uint64_t offset, uint64_t size){
    #define between(x, a, b) ((x) >= (a) && (x) <= (b))
    #define intersect(x,y,a,b) (between(x, a, b) || between(y, a, b) || between(a, x, y)) 
    if(intersect(offset, offset+size, KERNEL_OFFSET, KERNEL_END-1)) return 0;
    if(intersect(offset, offset+size, DRIVERS_OFFSET, 0xFFFFFFFF)) return 0;
    return 1;
}

uint8_t elf_load_file(Elf64_Map* map, uint64_t* offset){
    uint64_t org = *offset;
    map->org = org;

    char* file = (char*) map->ehdr;

    uint64_t max_offset = 0; 

    // copy program sections
    Elf64_Phdr* phdr = map->phdr;
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
    Elf64_Shdr* shdr = map->shdr;
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
    *offset = math_cielm64(org + max_offset, 0x1000);

    return 1;
}

static void elf_get_dyn_dependecies(Elf64_Map* map, Elf64_Shdr* shdr, array_t* arr){
    Elf64_Dyn* dyn = elf_get_table(map, shdr);
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

void elf_get_dependecies(Elf64_Map* map, array_t* arr){
    for (uint32_t i = 0; i < map->nshdr; i++)
    {
        Elf64_Shdr* shdr = map->shdr + i;
        if(shdr->sh_type == SHT_DYNAMIC){
            elf_get_dyn_dependecies(map, shdr, arr);
        }
    }
}

static Elf64_Addr elf_st_lookup_sym(Elf64_Map* map, Elf64_Shdr* shdr, char* name){
    char* str = elf_get_str_section(map, shdr->sh_link);
    int n = elf_get_num_entries(shdr);

    Elf64_Sym* sym = (Elf64_Sym*)elf_get_table(map, shdr);
    for (int i = 0; i < n; i++)
    {
        if(str_cmp(name, str + sym->st_name) == 0) return sym->st_value;

        sym ++;
    }
    return 0;
}

Elf64_Addr elf_lookup_sym(Elf64_Map* map, char* name){
    Elf64_Shdr* shdr = map->shdr;

    for (uint32_t i = 0; i < map->nshdr; i++)
    {
        if(shdr->sh_type == SHT_SYMTAB){
            Elf64_Addr value = elf_st_lookup_sym(map, shdr, name);
            if(value) return value;

        }
        shdr ++;
    }
    
    return 0;
}

static uint8_t elf_get_st_value(Elf64_Map* map, Elf64_Shdr* shdr, uint32_t indx, Elf64_Dependecies deps, Elf64_Addr* st_value){

    uint32_t n = elf_get_num_entries(shdr);
    if(indx >= n) return 0;

    Elf64_Sym* sym = (Elf64_Sym*)elf_get_table(map, shdr) + indx;
    if(sym->st_shndx == SHN_UNDEF && deps.libs){
        char* str = elf_get_str_section(map, shdr->sh_link);
        *st_value = 0;
        for (uint32_t i = 0; i < deps.nlibs; i++)
        {
            Elf64_Map* lib = deps.libs + i;
            Elf64_Addr value = elf_lookup_sym(lib, str + sym->st_name);
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

static uint8_t elf_do_section_rel(Elf64_Map* map, Elf64_Shdr* shdr, Elf64_Dependecies deps){
    Elf64_Shdr *symtab = map->shdr + shdr->sh_link;
    Elf64_Rela *rela = elf_get_table(map, shdr);
    int n = elf_get_num_entries(shdr);

    for (int i = 0; i < n; i++, rela++)
    {
        uint32_t sym_index = ELF64_R_SYM(rela->r_info);
        uint32_t type      = ELF64_R_TYPE(rela->r_info);

        uint8_t *target = map->org + rela->r_offset;

        Elf64_Addr S = 0;                  // Symbol value
        Elf64_Addr P = (Elf64_Addr)target; // Relocation place
        Elf64_Addr B = (Elf64_Addr)map->org; // Load base
        Elf64_Addr A = rela->r_addend;     // Addend

        if (sym_index != 0) {
            elf_get_st_value(map, symtab, sym_index, deps, &S);
        }

        switch (type)
        {
            case R_X86_64_NONE:
                break;
            case R_X86_64_64:
                *(uint64_t *)target = S + A;
                break;
            case R_X86_64_PC32:
                *(uint32_t *)target = (uint32_t)(S + A - P);
                break;
            case R_X86_64_GLOB_DAT:
                *(uint64_t *)target = S;
                break;
            case R_X86_64_JUMP_SLOT:
                *(uint64_t *)target = S;
                break;
            case R_X86_64_RELATIVE:
                *(uint64_t *)target = B + A;
                break;
            case R_X86_64_32:
                *(uint32_t *)target = (uint32_t)(S + A);
                break;
            case R_X86_64_32S:
                *(int32_t *)target = (int32_t)(S + A);
                break;
            case R_X86_64_16:
                *(uint16_t *)target = (uint16_t)(S + A);
                break;
            case R_X86_64_PC16:
                *(uint16_t *)target = (uint16_t)(S + A - P);
                break;
            case R_X86_64_8:
                *(uint8_t *)target = (uint8_t)(S + A);
                break;
            case R_X86_64_PC8:
                *(uint8_t *)target = (uint8_t)(S + A - P);
                break;
            default:
                break;
        }
    }

    return 1;
}

uint8_t elf_do_rel(Elf64_Map* map, Elf64_Dependecies deps){
    Elf64_Shdr* shdr = map->shdr;
    for (uint32_t i = 0; i < map->nshdr; i++)
    {
        if(shdr->sh_type == SHT_RELA){
            if(!elf_do_section_rel(map, shdr, deps)) return 0;
        }
        shdr ++;
    }
    return 1;
}
