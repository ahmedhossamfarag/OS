#include "elf.h"
#include "libc.h"
#include "info.h"

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
    return hdr->e_type == ET_EXEC;
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
static inline uint8_t elf_load_psegment(elf32_phdr_t *phdr, char* file) {
    if(!elf_check_mregion(phdr->p_vaddr, phdr->p_memsz)) return 0;

    char* sagment = mem_set((char*)phdr->p_vaddr, 0, phdr->p_memsz);
    mem_copy((char*)phdr->p_vaddr, file + phdr->p_offset, phdr->p_filesz);

    return 1;
}

/* Read all program headers */
static inline uint8_t elf_read_program_headers(elf32_ehdr_t *ehdr) {
    elf32_phdr_t* phdr = elf_pheader(ehdr);
    for (int i = 0; i < ehdr->e_phnum; i++) {
        if (phdr->p_type == PT_LOAD) {
            if(!elf_load_psegment(phdr, (char*)ehdr)) return 0;
        }
        phdr ++;
    }
    return 1;
}

uint8_t elf_load_file(void* file, uint32_t* eip){
    elf32_ehdr_t* ehdr = (elf32_ehdr_t*) file;
    if(!elf_check_file(ehdr) || !elf_check_supported(ehdr) || !elf_check_executable(ehdr)){
        return 0;
    }
    if(!elf_read_program_headers(ehdr)) return 0;
    *eip = ehdr->e_entry;
    return 1;
}