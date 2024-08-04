#include <stdint.h>

typedef struct
{
    unsigned char e_ident[16]; // Magic number and other info
    uint16_t e_type;           // Object file type
    uint16_t e_machine;        // Architecture
    uint32_t e_version;        // Object file version
    uint32_t e_entry;          // Entry point virtual address
    uint32_t e_phoff;          // Program header table file offset
    uint32_t e_shoff;          // Section header table file offset
    uint32_t e_flags;          // Processor-specific flags
    uint16_t e_ehsize;         // ELF header size in bytes
    uint16_t e_phentsize;      // Program header table entry size
    uint16_t e_phnum;          // Program header table entry count
    uint16_t e_shentsize;      // Section header table entry size
    uint16_t e_shnum;          // Section header table entry count
    uint16_t e_shstrndx;       // Section header string table index
} elf32_ehdr_t;

typedef enum
{
    ET_NONE = 0, // Unkown Type
    ET_REL = 1,  // Relocatable File
    ET_EXEC = 2  // Executable File
} elf_type_t;

typedef struct
{
    uint32_t p_type;   // Segment type
    uint32_t p_offset; // Segment file offset
    uint32_t p_vaddr;  // Segment virtual address
    uint32_t p_paddr;  // Segment physical address
    uint32_t p_filesz; // Segment size in file
    uint32_t p_memsz;  // Segment size in memory
    uint32_t p_flags;  // Segment flags
    uint32_t p_align;  // Segment alignment
} elf32_phdr_t;

typedef enum
{
    PT_NULL = 0,                  // Program header table entry unused
    PT_LOAD = 1,                  // Loadable segment
    PT_DYNAMIC = 2,               // Dynamic linking information
    PT_INTERP = 3,                // Interpreter information
    PT_NOTE = 4,                  // Auxiliary information
    PT_SHLIB = 5,                 // Reserved
    PT_PHDR = 6,                  // Segment containing program header table itself
    PT_TLS = 7,                   // Thread-Local Storage segment
    PT_NUM = 8,                   // Number of defined types
    PT_LOOS = 0x60000000,         // Start of OS-specific
    PT_GNU_EH_FRAME = 0x6474e550, // GCC .eh_frame_hdr segment
    PT_GNU_STACK = 0x6474e551,    // Indicates stack executability
    PT_GNU_RELRO = 0x6474e552,    // Read-only after relocation
    PT_LOSUNW = 0x6ffffffa,       // Sun-specific low
    PT_SUNWBSS = 0x6ffffffa,      // Sun specific bss
    PT_SUNWSTACK = 0x6ffffffb,    // Stack segment
    PT_HISUNW = 0x6fffffff,       // Sun-specific high
    PT_HIOS = 0x6fffffff,         // End of OS-specific
    PT_LOPROC = 0x70000000,       // Start of processor-specific
    PT_HIPROC = 0x7fffffff        // End of processor-specific
} elf32_ptype_t;

typedef struct
{
    uint32_t sh_name;      // Section name (string tbl index)
    uint32_t sh_type;      // Section type
    uint32_t sh_flags;     // Section flags
    uint32_t sh_addr;      // Section virtual addr at execution
    uint32_t sh_offset;    // Section file offset
    uint32_t sh_size;      // Section size in bytes
    uint32_t sh_link;      // Link to another section
    uint32_t sh_info;      // Additional section information
    uint32_t sh_addralign; // Section alignment
    uint32_t sh_entsize;   // Entry size if section holds table
} elf32_shdr_t;

typedef enum
{
    SHT_NULL = 0,     // Null section
    SHT_PROGBITS = 1, // Program information
    SHT_SYMTAB = 2,   // Symbol table
    SHT_STRTAB = 3,   // String table
    SHT_RELA = 4,     // Relocation (w/ addend)
    SHT_NOBITS = 8,   // Not present in file
    SHT_REL = 9,      // Relocation (no addend)
} sht_types_t;

typedef enum
{
    SHF_WRITE = 0x01, // Writable section
    SHF_ALLOC = 0x02  // Exists in memory
} sht_attributes_t;

typedef struct
{
    uint32_t st_name;       // Symbol name (string table index)
    uint32_t st_value;      // Symbol value
    uint32_t st_size;       // Symbol size
    unsigned char st_info;  // Symbol type and binding
    unsigned char st_other; // No meaning, 0
    uint16_t st_shndx;      // Section index
} elf32_sym_t;

#define ELF32_ST_BIND(INFO) ((INFO) >> 4)
#define ELF32_ST_TYPE(INFO) ((INFO) & 0x0F)

typedef enum
{
    STB_LOCAL = 0,  // Local scope
    STB_GLOBAL = 1, // Global scope
    STB_WEAK = 2    // Weak, (ie. __attribute__((weak)))
} stT_bindings_t;

typedef enum
{
    STT_NOTYPE = 0, // No type
    STT_OBJECT = 1, // Variables, arrays, etc.
    STT_FUNC = 2    // Methods or functions
} stT_types_t;

typedef struct
{
    uint32_t r_offset;
    uint32_t r_info;
} elf32_rel_t;

typedef struct
{
    uint32_t r_offset;
    uint32_t r_info;
    int32_t r_addend;
} elf32_rela_t;

#define ELF32_R_SYM(INFO) ((INFO) >> 8)
#define ELF32_R_TYPE(INFO) ((uint8_t)(INFO))

typedef enum
{
    R_386_NONE = 0, // No relocation
    R_386_32 = 1,   // Symbol + Offset
    R_386_PC32 = 2  // Symbol + Offset - Section Offset
} rtT_types_t;

uint8_t elf_load_file(void *file, uint32_t *eip);