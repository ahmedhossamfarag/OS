#include <stdint.h>
#include "dslib.h"

// ELF Header Magic Number
#define EI_MAG0 0x7f
#define EI_MAG1 'E'
#define EI_MAG2 'L'
#define EI_MAG3 'F'

// ELF Header Size
#define EI_NIDENT 16


// ELF Identification Indexes
#define EI_CLASS     4   // File class
#define EI_DATA      5   // Data encoding
#define EI_VERSION   6   // File version
#define EI_OSABI     7   // OS/ABI identification
#define EI_ABIVERSION 8  // ABI version

// Phdr X num
#define PN_XNUM 0xffff
// Shdr X index
#define SHN_XINDEX 0xffff 
// Special Section Indexes
#define SHN_UNDEF 0      // Undefined section

// ELF64 Data Types
typedef uint64_t Elf64_Addr;
typedef uint16_t Elf64_Half;
typedef uint64_t Elf64_Off;
typedef int32_t  Elf64_Sword;
typedef uint32_t Elf64_Word;
typedef uint64_t Elf64_Xword;
typedef int64_t  Elf64_Sxword;

// ELF Header
typedef struct {
    unsigned char e_ident[EI_NIDENT]; // Magic number and other info
    Elf64_Half    e_type;             // Object file type
    Elf64_Half    e_machine;          // Architecture
    Elf64_Word    e_version;          // Object file version
    Elf64_Addr    e_entry;            // Entry point virtual address
    Elf64_Off     e_phoff;            // Program header table file offset
    Elf64_Off     e_shoff;            // Section header table file offset
    Elf64_Word    e_flags;            // Processor-specific flags
    Elf64_Half    e_ehsize;           // ELF header size in bytes
    Elf64_Half    e_phentsize;        // Program header table entry size
    Elf64_Half    e_phnum;            // Program header table entry count
    Elf64_Half    e_shentsize;        // Section header table entry size
    Elf64_Half    e_shnum;            // Section header table entry count
    Elf64_Half    e_shstrndx;         // Section header string table index
} Elf64_Ehdr;

// Section Header
typedef struct {
    Elf64_Word sh_name;      // Section name (string table index)
    Elf64_Word sh_type;      // Section type
    Elf64_Xword sh_flags;     // Section flags
    Elf64_Addr sh_addr;      // Section virtual addr at execution
    Elf64_Off  sh_offset;    // Section file offset
    Elf64_Xword sh_size;      // Section size in bytes
    Elf64_Word sh_link;      // Link to another section
    Elf64_Word sh_info;      // Additional section information
    Elf64_Xword sh_addralign; // Section alignment
    Elf64_Xword sh_entsize;   // Entry size if section holds table
} Elf64_Shdr;

// Program Header
typedef struct {
    Elf64_Word p_type;    // Segment type
    Elf64_Word p_flags;   // Segment flags
    Elf64_Off  p_offset;  // Segment file offset
    Elf64_Addr p_vaddr;   // Segment virtual address
    Elf64_Addr p_paddr;   // Segment physical address
    Elf64_Xword p_filesz;  // Segment size in file
    Elf64_Xword p_memsz;   // Segment size in memory
    Elf64_Xword p_align;   // Segment alignment
} Elf64_Phdr;

typedef enum{
    EM_NONE = 0,       // No machine
    EM_386  = 3       // Intel 80386
} Elf64_e_machine;

typedef enum{
    EV_NONE =   0,     // Invalid version
    EV_CURRENT = 1     // Current version
} Elf64_e_version;

typedef enum{
    ELFCLASSNONE = 0,   // Invalid class
    ELFCLASS32  = 1,   // 32-bit objects
    ELFCLASS64  = 2   // 64-bit objects
} Elf64_file_class;

typedef enum{
    ELFDATANONE = 0,    // Invalid data encoding
    ELFDATA2LSB = 1,    // 2's complement, little endian
    ELFDATA2MSB = 2    // 2's complement, big endian
} Elf64_data_ecoding;

typedef enum
{
    ET_NONE = 0, // Unkown Type
    ET_REL = 1,  // Relocatable File
    ET_EXEC = 2,  // Executable File
    ET_DYN = 3, // Dynamic Shared Lib
    ET_CORE = 4, // Core File
    ET_LOOS = 0xFE00,   // Environment-specific use
    ET_HIOS = 0xFEFF,
    ET_LOPROC = 0xFF00,   // Processor-specific use
    ET_HIPROC = 0xFFFF
} Elf64_e_type;

typedef enum
{
    PT_NULL = 0,                  // Program header table entry unused
    PT_LOAD = 1,                  // Loadable segment
    PT_DYNAMIC = 2,               // Dynamic linking information
    PT_INTERP = 3,                // Interpreter information
    PT_NOTE = 4,                  // Auxiliary information
    PT_SHLIB = 5,                 // Reserved
    PT_PHDR = 6,                  // Segment containing program header table itself
    PT_LOOS = 0x60000000,         // Start of OS-specific
    PT_HIOS = 0x6fffffff,         // End of OS-specific
    PT_LOPROC = 0x70000000,       // Start of processor-specific
    PT_HIPROC = 0x7fffffff        // End of processor-specific
} Elf64_p_type;

typedef enum {
    SHT_NULL = 0x0,                // Marks an unused section header
    SHT_PROGBITS = 0x1,            // Contains information defined by the program
    SHT_SYMTAB = 0x2,              // Contains a linker symbol table
    SHT_STRTAB = 0x3,              // Contains a string table
    SHT_RELA = 0x4,                // Contains "Rela" type relocation entries
    SHT_HASH = 0x5,                // Contains a symbol hash table
    SHT_DYNAMIC = 0x6,             // Contains information for dynamic linking
    SHT_NOTE = 0x7,                // Contains information that marks files
    SHT_NOBITS = 0x8,              // Contains uninitialized data
    SHT_REL = 0x9,                 // Contains "Rel" type relocation entries
    SHT_SHLIB = 0x0A,              // Reserved for future use
    SHT_DYNSYM = 0x0B,             // Contains a dynamic linker symbol table
    SHT_LOOS = 0x60000000,         // Start of OS-specific
    SHT_HIOS = 0x6fffffff,         // End of OS-specific
    SHT_LOPROC = 0x70000000,       // Start of processor-specific
    SHT_HIPROC = 0x7fffffff       // End of processor-specific
} Elf64_sh_type;

typedef enum
{
    SHF_WRITE = 0x01, // Writable section
    SHF_ALLOC = 0x02,  // Exists in memory
    SHF_EXECINSTR = 0x4,    // Section contains executable instructions
    SHF_MASKOS = 0x0F000000,    // Environment-specific use
    SHF_MASKPROC = 0xF0000000   // Processor-specific use
} Elf64_sh_flag;

typedef struct
{
    Elf64_Word st_name;       // Symbol name (string table index)
    unsigned char st_info;  // Symbol type and binding
    unsigned char st_other; // No meaning, 0
    Elf64_Half st_shndx;      // Section index
    Elf64_Addr st_value;      // Symbol value
    Elf64_Xword st_size;       // Symbol size
} Elf64_Sym;

#define Elf64_ST_BIND(INFO) ((INFO) >> 4) // Symbol binding
#define Elf64_ST_TYPE(INFO) ((INFO) & 0x0F) // Symbol type

typedef enum
{
    STB_LOCAL = 0,  // Local scope
    STB_GLOBAL = 1, // Global scope
    STB_WEAK = 2,    // Weak, (ie. __attribute__((weak)))
    STT_LOOS = 10,  // Environment-specific use
    STT_HIOS = 12,
    STT_LOPROC = 13,    // Processor-specific use
    STT_HIPROC = 15
} Elf64_st_binding;

typedef enum
{
    STT_NOTYPE = 0, // No type
    STT_OBJECT = 1, // Variables, arrays, etc.
    STT_FUNC = 2,    // Methods or functions 
    STT_SECTION = 3, // Section
    STT_FILE = 4    // Source file
} Elf64_st_type;

typedef struct
{
    Elf64_Addr r_offset;
    Elf64_Xword r_info;
} Elf64_Rel;

typedef struct
{
    Elf64_Addr r_offset;
    Elf64_Xword r_info;
    Elf64_Sxword r_addend;
} Elf64_Rela;

#define ELF64_R_SYM(INFO) ((INFO) >> 32)
#define ELF64_R_TYPE(INFO) ((INFO) & 0xFFFFFFFF)

typedef enum {
    R_X86_64_NONE = 0,              // No relocation
    R_X86_64_64 = 1,                // S + A // Direct 64-bit absolute address
    R_X86_64_PC32 = 2,               // S + A - P // PC-relative 32-bit displacement
    R_X86_64_GOT32 = 3,              // G + A // 32-bit GOT entry offset
    R_X86_64_PLT32 = 4,              // L + A - P // 32-bit PC-relative PLT address
    R_X86_64_COPY = 5,               // Dynamic linker copy relocation
    R_X86_64_GLOB_DAT = 6,           // S // GOT entry for a global symbol
    R_X86_64_JUMP_SLOT = 7,          // S // PLT/GOT function address
    R_X86_64_RELATIVE = 8,           // B + A // Relative to object load base
    R_X86_64_GOTPCREL = 9,           // G + GOT + A - P // PC-relative GOT entry
    R_X86_64_32 = 10,                // S + A // Direct 32-bit absolute address
    R_X86_64_32S = 11,               // S + A // Direct signed 32-bit absolute address
    R_X86_64_16 = 12,                // S + A // Direct 16-bit absolute address
    R_X86_64_PC16 = 13,              // S + A - P // PC-relative 16-bit displacement
    R_X86_64_8 = 14,                 // S + A // Direct 8-bit absolute address
    R_X86_64_PC8 = 15,               // S + A - P // PC-relative 8-bit displacement
    R_X86_64_DTPMOD64 = 16,          // Module ID for TLS
    R_X86_64_DTPOFF64 = 17,          // TLS offset relative to DTV
    R_X86_64_TPOFF64 = 18,           // TLS offset relative to thread pointer
    R_X86_64_TLSGD = 19,             // TLS General Dynamic model
    R_X86_64_TLSLD = 20,             // TLS Local Dynamic model
    R_X86_64_DTPOFF32 = 21,          // 32-bit TLS offset
    R_X86_64_GOTTPOFF = 22,          // GOT entry for TLS offset
    R_X86_64_TPOFF32 = 23,           // 32-bit TLS offset from thread pointer
    R_X86_64_PC64 = 24,              // S + A - P // PC-relative 64-bit displacement 
    R_X86_64_GOTOFF64 = 25,          // S + A - GOT // 64-bit GOT-relative offset
    R_X86_64_GOTPC32 = 26,           // GOT + A - P // 32-bit PC-relative GOT address
    R_X86_64_SIZE32 = 32,             // Z + A // 32-bit symbol size
    R_X86_64_SIZE64 = 33,             // Z + A // 64-bit symbol size
    R_X86_64_GOTPC32_TLSDESC = 34,   // TLS descriptor GOT PC-relative
    R_X86_64_TLSDESC_CALL = 35,      // TLS descriptor function call
    R_X86_64_TLSDESC = 36,           // TLS descriptor
    R_X86_64_IRELATIVE = 37,         // B + A // Indirect relative relocation
    R_X86_64_RELATIVE64 = 38,        // B + A // 64-bit relative relocation
    R_X86_64_GOTPCRELX = 41,         // Relaxable GOT PC-relative relocation
    R_X86_64_REX_GOTPCRELX = 42      // Relaxable GOT PC-relative with REX prefix
} Elf64_X86_64_RelocationType;

typedef struct {
    Elf64_Sxword d_tag;  // Type of dynamic entry
    union {
        Elf64_Xword d_val;  // Integer value
        Elf64_Addr d_ptr;  // Address value
    } d_un;
} Elf64_Dyn;

typedef enum {
    DT_NULL = 0,         // Marks end of dynamic section
    DT_NEEDED = 1,       // Name of needed library
    DT_PLTRELSZ = 2,     // Size in bytes of PLT relocs
    DT_PLTGOT = 3,       // Address of PLT and/or GOT
    DT_HASH = 4,         // Address of symbol hash table
    DT_STRTAB = 5,       // Address of string table
    DT_SYMTAB = 6,       // Address of symbol table
    DT_RELA = 7,         // Address of Rela relocs
    DT_RELASZ = 8,       // Total size of Rela relocs
    DT_RELAENT = 9,      // Size of one Rela reloc
    DT_STRSZ = 10,       // Size of string table
    DT_SYMENT = 11,      // Size of one symbol table entry
    DT_INIT = 12,        // Address of init function
    DT_FINI = 13,        // Address of termination function
    DT_SONAME = 14,      // Name of shared object
    DT_RPATH = 15,       // Library search path (deprecated)
    DT_SYMBOLIC = 16,    // Start symbol search within local object
    DT_REL = 17,         // Address of Rel relocs
    DT_RELSZ = 18,       // Total size of Rel relocs
    DT_RELENT = 19,      // Size of one Rel reloc
    DT_PLTREL = 20,      // Type of reloc in PLT
    DT_DEBUG = 21,       // For debugging; unspecified
    DT_TEXTREL = 22,     // Reloc might modify .text
    DT_JMPREL = 23,      // Address of PLT relocs
    DT_BIND_NOW = 24,    // Process relocations of object now
    DT_INIT_ARRAY = 25,  // Array with addresses of init functions
    DT_FINI_ARRAY = 26,  // Array with addresses of fini functions
    DT_INIT_ARRAYSZ = 27,// Size in bytes of DT_INIT_ARRAY
    DT_FINI_ARRAYSZ = 28,// Size in bytes of DT_FINI_ARRAY
    DT_LOOS = 0x60000000,// Start of OS-specific
    DT_HIOS = 0x6fffffff,// End of OS-specific
    DT_LOPROC = 0x70000000, // Start of processor-specific
    DT_HIPROC = 0x7fffffff  // End of processor-specific
} Elf64_d_tag;

typedef struct
{
    Elf64_Ehdr* ehdr;
    Elf64_Shdr* shdr;
    uint64_t nshdr;
    Elf64_Phdr* phdr;
    uint64_t nphdr;
    char* str;
    char* org;
} Elf64_Map;

typedef struct
{
    Elf64_Map* libs;
    uint64_t nlibs;
} Elf64_Dependecies;


uint8_t elf_check_supported(Elf64_Ehdr* ehdr);

uint8_t elf_check_executable(Elf64_Ehdr* ehdr);

void elf_get_map(Elf64_Map* map, char* file);

char* elf_get_str_section(Elf64_Map* map, uint32_t shindx);

void* elf_get_table(Elf64_Map* map, Elf64_Shdr* shdr);

Elf64_Shdr* elf_get_sheader(Elf64_Map* map, uint32_t shindx);

uint8_t elf_load_file(Elf64_Map* map, uint32_t* offset);

void elf_get_dependecies(Elf64_Map* map, array_t* arr);

uint32_t elf_lookup_sym(Elf64_Map* map, char* name);

uint8_t elf_do_rel(Elf64_Map* map, Elf64_Dependecies deps);
