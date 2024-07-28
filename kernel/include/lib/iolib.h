
#include <stdint.h>

#define FILE uint32_t

FILE* fopen(const char* name);

uint8_t fsize(FILE* file, uint32_t* to);

uint8_t fclose(FILE* file);

uint8_t fread(FILE* file, char* to, uint32_t seek, uint32_t count);

uint8_t fwrite(FILE* file, char* from, uint32_t count);

FILE* fcreate(FILE* parent, char* name, uint8_t is_dir);

uint8_t fdelete(FILE* file);

uint8_t flist(FILE* dir, char* to);

uint8_t prints(const char* str);

uint8_t scans(char* to, uint32_t count);

uint8_t scans_line(char* to);