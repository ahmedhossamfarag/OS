#include "loader.h"
#include "pagging.h"
#include "scheduler.h"
#include "ata.h"
#include "memory.h"
#include "file_system.h"

#include "vga_print.h"
#include "strlib.h"

static struct 
{
    char* file;
    char* lib;
    fs_entity_t* root_dir, *libfs;
    uint32_t cr3;
    uint32_t entry;
    uint32_t membegin;
} args;


#define SHELL_OFFSET 201
#define SHELL_SIZE 50
#define SHELL_STACK_SZ 0x1000 

extern void linker_load_elf(char* file, uint32_t cr3, uint32_t* entry, uint32_t* membegin, SUCC_ERR);

static void error(){
    println("error")
}

static void loader_succ(){
    println("Loader Succ")

    uint32_t eip = args.entry;
    uint32_t membegin = args.membegin;
    uint32_t ebp = membegin + 0x1000;
    membegin += 0x1000 + 0x100;

    add_new_process(1, 0, args.cr3, eip, ebp, membegin);            
}

static void success(){
    println("succ")
    linker_load_elf(args.file, args.cr3, &args.entry, &args.membegin, loader_succ, error);
}

static void write_lib(){
    file_write((file_entity_t*)args.libfs, args.lib, 50, 50*512, success, error);
}

static void create_lib(){
    file_create((dir_entity_t*)args.root_dir, "libmylib.so", &args.libfs, write_lib, error);
}


void load_program()
{
    args.cr3 = (uint32_t)get_available_pagging_dir();
    if(args.cr3){
        args.file = alloc(SHELL_SIZE * 512);
        ata_read(PRIMARY_BASE, 0, SHELL_OFFSET, SHELL_SIZE, args.file);
        args.lib = alloc(50*512);
        ata_read(PRIMARY_BASE, 0, 251, 50, args.lib);
        file_open(0, 0, &args.root_dir, create_lib, error);
    }
}