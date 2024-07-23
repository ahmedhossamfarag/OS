#include "io_syscall.h"
#include "interrupt_handler.h"
#include "screen_print.h"
#include "pic.h"
#include "interrupt.h"
#include "apic.h"
#include "strlib.h"
#include "scheduler.h"
#include "keyboard.h"
#include "memory.h"
#include "file_system.h"
#include "disk.h"
#include "math.h"
#include "resources.h"



/*


#define MAX_N_FILES 10
file_t* open_files;
uint32_t n_open_files;

uint32_t fopen_handler(cpu_state_t*)
{

    char* name;
    asm("mov %%eax, %0":"=a"(name));

    if(n_open_files >= MAX_N_FILES) return 0;

    uint32_t len = str_len(name);
    char* name_splited = alloc(len+2);
    char** splits = (char**) alloc((len+2) * sizeof(char*));
    if(!name_splited || !splits) return 0;
    str_split(name, '/', splits, name_splited);

    file_t file = {0, 0};
    file_t* parent = 0;

    int i = 0;
    while(splits[i]){
        file = file_open(parent, splits[i]);
        if(!file.lba){
            break;
        }
        parent = &file;
        i++;
    }
    
    free(name_splited, len+2);
    free((char*)splits, (len+2) * sizeof(char*));


    if(file.lba){
        open_files[n_open_files] = file;
        n_open_files  ++;
        return file.lba;
    }else{
        return 0;
    }

}

static file_t* get_open_file(uint32_t lba){
    for (file_t* f = open_files; f < open_files + n_open_files; f++)
    {
        if(f->lba == lba){
            return f;
        }
    }
    return 0;
}

uint32_t fsize_handler(cpu_state_t*)
{
    uint32_t lba;
    uint32_t *to;
    asm("mov %%eax, %0": "=a"(lba),"=d"(to));

    file_t* file = get_open_file(lba);

    if(!file) return 0;

    file_entity_t* entity = (file_entity_t*) alloc(SectorSize);
    if(!entity) return 0;
    file_get_info(file, entity);

    *to = entity->size;
    return 1;
}

uint32_t fclose_handler(cpu_state_t*)
{
    uint32_t lba;
    asm("mov %%eax, %0":"=a"(lba));
    file_t* file = get_open_file(lba);
    if(file){
        for (file_t* f = file; f < open_files + n_open_files - 1; f++)
        {
            *f = *(f+1);
        }
        n_open_files--;
        return 1;
    }else{
        return 0;
    }
}

uint32_t fread_handler(cpu_state_t*)
{
    uint32_t lba;
    char *to;
    uint32_t seek;
    uint32_t count;
    asm("mov %%eax, %0": "=a"(lba),"=d"(to),"=b"(seek),"=c"(count));

    file_t* file = get_open_file(lba);

    if(!file) return 0;

    uint32_t disk_seek = seek / SectorSize;
    uint32_t disk_count = math_cielm(seek + count, SectorSize) - disk_seek;

    char* data = alloc(disk_count * SectorSize);
    if(!data) return 0;
    if(file_read(file, disk_seek, disk_count, data)){
        uint32_t start = seek - disk_seek * SectorSize;
        uint32_t end = start + count;
        
        for(char* c = data + start; c < c + end; c++){
            *to = *c;
            to++;
        }

        free(data, disk_count * SectorSize);

        return 1;
    }

    free(data, disk_count * SectorSize);

    return 0;
}

uint32_t fwrite_handler(cpu_state_t*)
{
    uint32_t lba;
    char *from;
    uint32_t count;
    asm("mov %%eax, %0": "=a"(lba),"=d"(from),"=c"(count));

    file_t* file = get_open_file(lba);

    if(!file) return 0;

    if(file_write(file, from, count)){
        return 1;
    }

    return 0;
}

uint32_t prints_handler(cpu_state_t*)
{
    char* str;
    asm("mov %%eax, %0":"=a"(str));

    screen_print_str(str);

    return 1;
}

static void scans_thread_awake(){
    set_keyboard_handler_proc(0);
}

char* scans_to;
uint32_t scans_n;
static void scans_keyboard_proc(key_info_t k){
    *scans_to = k.character;
    scans_n --;
    if(scans_n){
        scans_to ++;
    }else{
        scans_thread_awake();
    }
}

static void scans_line_keyboard_proc(key_info_t k){
    if(k.key_code != ENTER){
        *scans_to = k.character;
        scans_to ++;
    }else{
        *scans_to = '\0';
        scans_thread_awake();
    }
}

uint32_t scans_handler(cpu_state_t*)
{
    char* to;
    uint32_t n;
    asm("mov %%edx, %0":"=d"(to),"=c"(n));

    if(n == 0)
        return 1;
    
    scans_to = to;
    scans_n = n;
    
    set_keyboard_handler_proc(scans_keyboard_proc);

}

uint32_t scans_line_handler(cpu_state_t*)
{
    char* to;
    asm("mov %%edx, %0":"=d"(to));

    scans_to = to;

    set_keyboard_handler_proc(scans_line_keyboard_proc);

}



void io_syscall_init(){
    open_files = (file_t*) alloc(MAX_N_FILES * sizeof(file_t));
    n_open_files = 0;
}





*/