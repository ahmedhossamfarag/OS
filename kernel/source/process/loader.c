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
    uint32_t file_nsectors;
    fs_entity_t* root_dir, *filefs;
    uint32_t cr3;
    uint32_t entry;
    uint32_t membegin;
} args;


#define SHELL_STACK_SZ 0x1000 
#define MEMORY_PADDING 0x100

extern void linker_load_elf(char* file, uint32_t cr3, uint32_t* entry, uint32_t* membegin, SUCC_ERR);


static void loader_free(){
    if(args.file){
        free(args.file, args.file_nsectors*SectorSize);
    }
    if(args.root_dir){
        file_close(args.root_dir, 0, 0);
    }
    if(args.filefs){
        file_close(args.filefs, 0, 0);
    }
}

static void loader_error(){
    loader_free();
    if(args.cr3){
        free_pagging_dir((uint32_t*)args.cr3);
    }
    println("Loader error")
}

static void loader_succ(){
    loader_free();
    println("Loader Success")

    uint32_t eip = args.entry;
    uint32_t membegin = args.membegin;
    uint32_t ebp = membegin + SHELL_STACK_SZ;
    membegin += SHELL_STACK_SZ + MEMORY_PADDING;

    add_new_process(1, 0, args.cr3, eip, ebp, membegin);            
}

static void link_file(){
    linker_load_elf(args.file, args.cr3, &args.entry, &args.membegin, loader_succ, loader_error);
}

static void read_file(){
    args.file_nsectors = ((file_entity_t*)args.filefs)->n_blocks;
    args.file = alloc(args.file_nsectors * SectorSize);
    if(!args.file){
        loader_error();
        return;
    }
    file_read((file_entity_t*)args.filefs, args.file, 0, args.file_nsectors, link_file, loader_error);
}

static void open_file(){
    file_open((dir_entity_t*)args.root_dir, "shell", &args.filefs, read_file, loader_error);
}

void load_program()
{
    args.root_dir = args.filefs = 0;
    args.file = 0;
    args.cr3 = (uint32_t)get_available_pagging_dir();
    if(args.cr3){
        file_open(0, 0, &args.root_dir, open_file, loader_error);
    }
}