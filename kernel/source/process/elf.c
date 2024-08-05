#include "elf.h"
#include "libc.h"
#include "info.h"
#include "math.h"

/* Check the file is elf */
static inline uint8_t elf_check_file(elf32_ehdr_t *hdr) {
    // Check the magic number
    if (hdr->e_ident[0] != 0x7f || hdr->e_ident[1] != 'E' || 
        hdr->e_ident[2] != 'L' || hdr->e_ident[3] != 'F') {
        return 0;
    }

    return 1;
}

/* Check elf is supported */
static inline uint8_t elf_check_supported(elf32_ehdr_t *hdr) {
    if (hdr->e_ident[4] != 1) { // Check if it's a 32-bit ELF file
        return 0;
    }

    if (hdr->e_ident[5] != 1) { // Check if it's using little endian
        return 0;
    }

    return 1;
}

/* Check elf is executable */
static inline uint8_t elf_check_executable(elf32_ehdr_t *hdr) {
    return hdr->e_type == ET_EXEC || hdr->e_type == ET_DYN;
}

static inline uint8_t elf_check_mregion(uint32_t offset, uint32_t size){
    #define between(x, a, b) ((x) >= (a) && (x) <= (b))
    #define intersect(x,y,a,b) (between(x, a, b) || between(y, a, b) || between(a, x, y)) 
    if(intersect(offset, offset+size, KERNEL_OFFSET, KERNEL_END-1)) return 0;
    if(intersect(offset, offset+size, DRIVERS_OFFSET, 0xFFFFFFFF)) return 0;
    return 1;
}

/* Get program header start */
static inline elf32_phdr_t *elf_pheader(elf32_ehdr_t *hdr) {
	return (elf32_phdr_t *)((uint32_t)hdr + hdr->e_phoff);
}

/* Load program segment into memory*/
static inline uint32_t elf_load_psegment(elf32_phdr_t *phdr, char* file) {
    if(!elf_check_mregion(phdr->p_vaddr, phdr->p_memsz)) return 0;

    char* sagment = mem_set((char*)phdr->p_vaddr, 0, phdr->p_memsz);
    uint32_t sz = math_min(phdr->p_memsz, phdr->p_filesz);
    mem_copy(file + phdr->p_offset, (char*)phdr->p_vaddr, sz);

    return phdr->p_vaddr + phdr->p_memsz;
}

/* Read all program headers */
static inline uint32_t elf_read_program_headers(elf32_ehdr_t *ehdr) {
    elf32_phdr_t* phdr = elf_pheader(ehdr);
    uint32_t textend = 0;
    for (int i = 0; i < ehdr->e_phnum; i++) {
        if (phdr->p_type == PT_LOAD) {
            uint32_t end = elf_load_psegment(phdr, (char*)ehdr);
            if(!end) return 0;
            textend = math_max(textend, end);
        }
        phdr ++;
    }
    return textend;
}

static inline elf32_shdr_t *elf_sheader(elf32_ehdr_t *ehdr) {
	return (elf32_shdr_t *)((int)ehdr + ehdr->e_shoff);
}

static inline elf32_shdr_t *elf_section(elf32_ehdr_t *ehdr, int idx) {
	return &elf_sheader(ehdr)[idx];
}

static inline char *elf_str_table(elf32_ehdr_t *ehdr) {
	if(ehdr->e_shstrndx == 0) return 0;
	return (char *)ehdr + elf_section(ehdr, ehdr->e_shstrndx)->sh_offset;
}

static inline char *elf_lookup_string(elf32_ehdr_t *ehdr, int offset) {
	char *strtab = elf_str_table(ehdr);
	if(strtab == 0) return 0;
	return strtab + offset;
}

static inline void elf_read_symbol_table(elf32_shdr_t* shdr, elf32_ehdr_t* ehdr){
    int n = shdr->sh_size / shdr->sh_entsize;
    elf32_sym_t* sym = (elf32_sym_t*)((uint32_t)ehdr + shdr->sh_offset);
    for (int i = 0; i < n; i++)
    {
        sym ++;
    }
}

static inline void elf_read_bss_section(elf32_shdr_t* shdr, elf32_ehdr_t* ehdr){
    if (!shdr->sh_size)
        return;

    if (shdr->sh_flags & SHF_ALLOC)
    {
        if(!elf_check_mregion(shdr->sh_vaddr, shdr->sh_size)) return;
        mem_set((char*)shdr->sh_vaddr, shdr->sh_size, 0);
    }
}

static inline uint32_t elf_lookup_symval(elf32_shdr_t* shdr, elf32_ehdr_t* ehdr, uint8_t indx){
    if(shdr->sh_type != SHT_SYMTAB && shdr->sh_type != SHT_DYNSYM) return 0;

    int n = shdr->sh_size / shdr->sh_entsize;
    elf32_sym_t* sym = (elf32_sym_t*)((uint32_t)ehdr + shdr->sh_offset);
    
    if(indx < n){
        return sym[indx].st_value;
    }
    return 0;
}

static inline void elf_read_relocation_section(elf32_shdr_t* shdr, elf32_ehdr_t* ehdr){
    elf32_shdr_t* sym_table = elf_section(ehdr, shdr->sh_link);
    elf32_rel_t* rel = (elf32_rel_t*)((uint32_t) ehdr + shdr->sh_offset);
    int n = shdr->sh_size / shdr->sh_entsize;
    for (int i = 0; i < n; i++)
    {
        uint32_t sym_index = ELF32_R_SYM(rel->r_info);
        uint32_t type = ELF32_R_TYPE(rel->r_info);
        uint32_t* ref = (uint32_t*)((uint32_t)ehdr + shdr->sh_offset+ rel->r_offset);
        uint32_t symval = elf_lookup_symval(sym_table, ehdr, sym_index);
        switch (type)
        {
        case R_386_32:
            *ref = symval;
            break;
        default:
            break;
        }
        rel ++;
    }
    
}

static inline void elf_read_relocationa_section(elf32_shdr_t* shdr, elf32_ehdr_t* ehdr){
    elf32_shdr_t* sym_table = elf_section(ehdr, shdr->sh_link);
    elf32_rela_t* rela = (elf32_rela_t*)((uint32_t) ehdr + shdr->sh_offset);
    int n = shdr->sh_size / shdr->sh_entsize;
    for (int i = 0; i < n; i++)
    {
        uint32_t sym_index = ELF32_R_SYM(rela->r_info);
        uint32_t type = ELF32_R_TYPE(rela->r_info);
        uint32_t* ref = (uint32_t*)((uint32_t)ehdr + shdr->sh_offset+ rela->r_offset);
        uint32_t symval = elf_lookup_symval(sym_table, ehdr, sym_index);
        switch (type)
        {
        case R_386_32:
            *ref = symval + rela->r_addend;
            break;
        default:
            break;
        }
        rela ++;
    }
    
}

static inline void elf_read_section_headers(elf32_ehdr_t* ehdr){
    elf32_shdr_t* shdr = elf_sheader(ehdr);
    for (int i = 0; i < ehdr->e_shnum; i++)
    {
        switch (shdr->sh_type)
        {
        case SHT_NOBITS:
            elf_read_bss_section(shdr, ehdr);
            break;
        case SHT_REL:
            elf_read_relocation_section(shdr, ehdr);
            break;
        case SHT_RELA:
            elf_read_relocationa_section(shdr, ehdr);
            break;
        default:
            break;
        }       
        shdr ++;
    }
    
}


uint32_t elf_load_file(void* file, uint32_t* eip){
    elf32_ehdr_t* ehdr = (elf32_ehdr_t*) file;
    if(!elf_check_file(ehdr) || !elf_check_supported(ehdr) || !elf_check_executable(ehdr)){
        return 0;
    }

    elf_read_section_headers(ehdr);

    *eip = ehdr->e_entry;
    return elf_read_program_headers(ehdr);
}