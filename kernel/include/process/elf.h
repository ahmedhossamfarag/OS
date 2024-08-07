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

// ELF32 Data Types
typedef uint32_t Elf32_Addr;
typedef uint16_t Elf32_Half;
typedef uint32_t Elf32_Off;
typedef int32_t  Elf32_Sword;
typedef uint32_t Elf32_Word;

// ELF Header
typedef struct {
    unsigned char e_ident[EI_NIDENT]; // Magic number and other info
    Elf32_Half    e_type;             // Object file type
    Elf32_Half    e_machine;          // Architecture
    Elf32_Word    e_version;          // Object file version
    Elf32_Addr    e_entry;            // Entry point virtual address
    Elf32_Off     e_phoff;            // Program header table file offset
    Elf32_Off     e_shoff;            // Section header table file offset
    Elf32_Word    e_flags;            // Processor-specific flags
    Elf32_Half    e_ehsize;           // ELF header size in bytes
    Elf32_Half    e_phentsize;        // Program header table entry size
    Elf32_Half    e_phnum;            // Program header table entry count
    Elf32_Half    e_shentsize;        // Section header table entry size
    Elf32_Half    e_shnum;            // Section header table entry count
    Elf32_Half    e_shstrndx;         // Section header string table index
} Elf32_Ehdr;

// Section Header
typedef struct {
    Elf32_Word sh_name;      // Section name (string table index)
    Elf32_Word sh_type;      // Section type
    Elf32_Word sh_flags;     // Section flags
    Elf32_Addr sh_addr;      // Section virtual addr at execution
    Elf32_Off  sh_offset;    // Section file offset
    Elf32_Word sh_size;      // Section size in bytes
    Elf32_Word sh_link;      // Link to another section
    Elf32_Word sh_info;      // Additional section information
    Elf32_Word sh_addralign; // Section alignment
    Elf32_Word sh_entsize;   // Entry size if section holds table
} Elf32_Shdr;

// Program Header
typedef struct {
    Elf32_Word p_type;    // Segment type
    Elf32_Off  p_offset;  // Segment file offset
    Elf32_Addr p_vaddr;   // Segment virtual address
    Elf32_Addr p_paddr;   // Segment physical address
    Elf32_Word p_filesz;  // Segment size in file
    Elf32_Word p_memsz;   // Segment size in memory
    Elf32_Word p_flags;   // Segment flags
    Elf32_Word p_align;   // Segment alignment
} Elf32_Phdr;

typedef enum{
    EM_NONE = 0,       // No machine
    EM_386  = 3       // Intel 80386
} Elf32_e_machine;

typedef enum{
    EV_NONE =   0,     // Invalid version
    EV_CURRENT = 1     // Current version
} Elf32_e_version;

typedef enum{
    ELFCLASSNONE = 0,   // Invalid class
    ELFCLASS32  = 1,   // 32-bit objects
    ELFCLASS64  = 2   // 64-bit objects
} Elf32_file_class;

typedef enum{
    ELFDATANONE = 0,    // Invalid data encoding
    ELFDATA2LSB = 1,    // 2's complement, little endian
    ELFDATA2MSB = 2    // 2's complement, big endian
} Elf32_data_ecoding;

typedef enum
{
    ET_NONE = 0, // Unkown Type
    ET_REL = 1,  // Relocatable File
    ET_EXEC = 2,  // Executable File
    ET_DYN = 3, // Dynamic Shared Lib
    ET_CORE = 4 // Core File
} Elf32_e_type;

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
} Elf32_p_type;

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
    SHT_INIT_ARRAY = 0x0E,         // Contains pointers to initialization functions
    SHT_FINI_ARRAY = 0x0F,         // Contains pointers to termination functions
    SHT_PREINIT_ARRAY = 0x10,      // Contains pointers to pre-initialization functions
    SHT_GROUP = 0x11,              // Identifies a section group
    SHT_SYMTAB_SHNDX = 0x12,       // Contains extended section indices
    SHT_NUM = 0x13,                // Number of defined types
    SHT_LOOS = 0x60000000,         // Start of OS-specific
    SHT_GNU_ATTRIBUTES = 0x6ffffff5, // Object attributes
    SHT_GNU_HASH = 0x6ffffff6,     // GNU-style hash table
    SHT_GNU_LIBLIST = 0x6ffffff7,  // Prelink library list
    SHT_CHECKSUM = 0x6ffffff8,     // Checksum for ELF file content
    SHT_LOSUNW = 0x6ffffffa,       // Sun-specific low boundary
    SHT_SUNW_COMDAT = 0x6ffffffb,  // Sun-specific COMDAT section
    SHT_SUNW_syminfo = 0x6ffffffc, // Sun-specific symbol information
    SHT_GNU_verdef = 0x6ffffffd,   // Version definition section
    SHT_GNU_verneed = 0x6ffffffe,  // Version needs section
    SHT_GNU_versym = 0x6fffffff,   // Version symbol table
    SHT_HIOS = 0x6fffffff,         // End of OS-specific
    SHT_LOPROC = 0x70000000,       // Start of processor-specific
    SHT_HIPROC = 0x7fffffff,       // End of processor-specific
    SHT_LOUSER = 0x80000000,       // Start of application-specific
    SHT_HIUSER = 0x8fffffff        // End of application-specific
} Elf32_sh_type;

typedef enum
{
    SHF_WRITE = 0x01, // Writable section
    SHF_ALLOC = 0x02  // Exists in memory
} ELf32_sh_flag;

typedef struct
{
    Elf32_Word st_name;       // Symbol name (string table index)
    Elf32_Word st_value;      // Symbol value
    Elf32_Word st_size;       // Symbol size
    unsigned char st_info;  // Symbol type and binding
    unsigned char st_other; // No meaning, 0
    Elf32_Half st_shndx;      // Section index
} Elf32_Sym;

#define ELF32_ST_BIND(INFO) ((INFO) >> 4) // Symbol binding
#define ELF32_ST_TYPE(INFO) ((INFO) & 0x0F) // Symbol type

typedef enum
{
    STB_LOCAL = 0,  // Local scope
    STB_GLOBAL = 1, // Global scope
    STB_WEAK = 2    // Weak, (ie. __attribute__((weak)))
} Elf32_st_binding;

typedef enum
{
    STT_NOTYPE = 0, // No type
    STT_OBJECT = 1, // Variables, arrays, etc.
    STT_FUNC = 2    // Methods or functions
} Elf32_st_type;

typedef struct
{
    uint32_t r_offset;
    uint32_t r_info;
} Elf32_Rel;

typedef struct
{
    uint32_t r_offset;
    uint32_t r_info;
    int32_t r_addend;
} Elf32_Rela;

#define ELF32_R_SYM(INFO) ((INFO) >> 8)
#define ELF32_R_TYPE(INFO) ((uint8_t)(INFO))

typedef enum {
    R_386_NONE = 0,        // No relocation
    R_386_32 = 1,          // Direct 32-bit
    R_386_PC32 = 2,        // PC relative 32-bit
    R_386_GOT32 = 3,       // 32-bit GOT entry
    R_386_PLT32 = 4,       // 32-bit PLT address
    R_386_COPY = 5,        // Copy symbol at runtime
    R_386_GLOB_DAT = 6,    // Create GOT entry
    R_386_JMP_SLOT = 7,    // Create PLT entry
    R_386_RELATIVE = 8,    // Adjust by program base
    R_386_GOTOFF = 9,      // 32-bit offset to GOT
    R_386_GOTPC = 10       // 32-bit PC relative offset to GOT
} Elf32_rel_type;

typedef struct {
    Elf32_Sword d_tag;  // Type of dynamic entry
    union {
        Elf32_Word d_val;  // Integer value
        Elf32_Addr d_ptr;  // Address value
        Elf32_Off d_off;
    } d_un;
} Elf32_Dyn;

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
    DT_RUNPATH = 29,     // Library search path
    DT_FLAGS = 30,       // Flags for the object being loaded
    DT_ENCODING = 32,    // Start of encoded range
    DT_PREINIT_ARRAY = 32, // Array with addresses of preinit functions
    DT_PREINIT_ARRAYSZ = 33,// Size in bytes of DT_PREINIT_ARRAY
    DT_MAXPOSTAGS = 34,  // Number of positive tags
    DT_LOOS = 0x60000000,// Start of OS-specific
    DT_HIOS = 0x6fffffff,// End of OS-specific
    DT_LOPROC = 0x70000000, // Start of processor-specific
    DT_HIPROC = 0x7fffffff  // End of processor-specific
} Elf32_d_tag;

typedef struct
{
    Elf32_Ehdr* ehdr;
    Elf32_Shdr* shdr;
    uint32_t nshdr;
    Elf32_Phdr* phdr;
    uint32_t nphdr;
    char* str;
    uint32_t org;
} Elf32_Map;

typedef struct
{
    Elf32_Map* libs;
    uint32_t nlibs;
} Elf32_Dependecies;

uint8_t elf_check_supported(Elf32_Ehdr* ehdr);

uint8_t elf_check_executable(Elf32_Ehdr* ehdr);

void elf_get_map(Elf32_Map* map, char* file);

char* elf_get_str_section(Elf32_Map* map, uint32_t shindx);

void* elf_get_table(Elf32_Map* map, Elf32_Shdr* shdr);

Elf32_Shdr* elf_get_sheader(Elf32_Map* map, uint32_t shindx);

uint8_t elf_load_file(Elf32_Map* map, uint32_t* offset);

void elf_get_dependecies(Elf32_Map* map, array_t* arr);

uint32_t elf_lookup_sym(Elf32_Map* map, char* name);

uint8_t elf_do_rel(Elf32_Map* map, Elf32_Dependecies deps);