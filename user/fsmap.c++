#include <iostream>
#include <stdint.h>
#include <fstream>
#include <string>
#include <filesystem>
#include <cmath>
#include <vector>
#include <cstring>

#define FILE_TYPE 0x33
#define DIR_TYPE 0x55
#define NameLength 100

typedef struct{
    uint8_t type;
    uint32_t lba;
    uint32_t parent;
    uint32_t prev;
    uint32_t next;
    char name[NameLength];
} __attribute__((packed)) fs_entity_t;

typedef struct{
    fs_entity_t fs;
    uint32_t data;
    uint32_t n_blocks;
    uint32_t size;
}  __attribute__((packed)) file_entity_t;

typedef struct{
    fs_entity_t fs;
    uint32_t head;
    uint32_t tail;
    uint32_t n_childs;
}  __attribute__((packed)) dir_entity_t;

typedef struct Entry {
    int type;
    const char* name;
    const char* srcFile;
    std::vector<Entry> children;
} Entry;

auto filesystem = Entry{
    .type = DIR_TYPE,
    .name = "",
    .srcFile = "",
    .children = {
        Entry{
            .type = FILE_TYPE,
            .name = "desktop.o",
            .srcFile = "desktop/desktop.o",
            .children = {}
        },
    }
};

void writeFSEntity(fs_entity_t* entity, uint32_t sz, uint32_t rootlba, std::ofstream& fs){
    fs.seekp((entity->lba - rootlba) * 512);
    fs.write((char*) entity, sz);
}

int writeFile(Entry* entry, file_entity_t* file, uint32_t lba, uint32_t rootlba, std::ofstream& fs){
    uint32_t size = (uint32_t) std::filesystem::file_size(entry->srcFile);
    uint32_t nBlocks = (uint32_t) std::ceil(size / 512.0);
    char* buffer = new char[size];
    std::ifstream src(entry->srcFile, std::ios::binary);
    src.read(buffer, size);
    src.close();

    file->data = lba;
    file->n_blocks = nBlocks;
    file->size = size;

    fs.seekp((lba - rootlba) * 512);
    fs.write(buffer, size);
    delete[] buffer;

    return lba + nBlocks;

}

int writeDirectory(Entry* entry, dir_entity_t* dir, uint32_t lba, uint32_t rootlba, std::ofstream& fs){
    uint32_t nChildren = entry->children.size();
    
    if (nChildren == 0) {
        dir->head = dir->tail = 0;
        dir->n_childs = 0;
        return lba;
    }

    fs_entity_t** entities = new fs_entity_t*[nChildren];
    
    dir->head = lba;
    dir->tail = lba + nChildren - 1;
    dir->n_childs = nChildren;

    for (int i = 0; i < nChildren; i++) {
        Entry* child = &entry->children[i];
        if (child->type == FILE_TYPE) {
            auto entity = new file_entity_t{
                .fs = {
                    .type = FILE_TYPE,
                    .lba = lba,
                    .parent = dir->fs.lba,
                    .prev = i == 0 ? 0 : lba - 1,
                    .next = i == nChildren - 1 ? 0 : lba + 1,
                },
            };
            strcpy(entity->fs.name, child->name);
            entities[i] = (fs_entity_t*) entity;
        } else if (child->type == DIR_TYPE) {
            auto entity = new dir_entity_t{
                .fs = {
                    .type = DIR_TYPE,
                    .lba = lba,
                    .parent = dir->fs.lba,
                    .prev = i == 0 ? 0 : lba - 1,
                    .next = i == nChildren - 1 ? 0 : lba + 1,
                },
            };
            strcpy(entity->fs.name, child->name);
            entities[i] = (fs_entity_t*) entity;
        }
        lba ++;
    }

    for (int i = 0; i < nChildren; i++) {
        Entry* child = &entry->children[i];
        if (child->type == FILE_TYPE) {
            lba = writeFile(child, (file_entity_t*) entities[i], lba, rootlba, fs);
        } else if (child->type == DIR_TYPE) {
            lba = writeDirectory(child, (dir_entity_t*) entities[i], lba, rootlba, fs);
        }
    }

    for (int i = 0; i < nChildren; i++) {
        Entry* child = &entry->children[i];
        if (child->type == FILE_TYPE) {
            writeFSEntity(entities[i], sizeof(file_entity_t), rootlba, fs);
            delete (file_entity_t*) entities[i];
        } else if (child->type == DIR_TYPE) {
            writeFSEntity(entities[i], sizeof(dir_entity_t), rootlba, fs);
            delete (dir_entity_t*) entities[i];
        }
    }

    delete[] entities;

    return lba;
}

int main(int argc, char** argv)
{
    uint32_t lba = 0;
    if (argc > 1) {
        lba = std::stoi(argv[1], nullptr, 0);
    }

    auto rootdir = new dir_entity_t{
        .fs = {
            .type = DIR_TYPE,
            .lba = lba,
            .parent = 0,
            .prev = 0,
            .next = 0,
        },
    };
    lba ++;

    std::ofstream fs("filesystem", std::ios::binary);
    writeDirectory(&filesystem, rootdir, lba, rootdir->fs.lba, fs);
    writeFSEntity((fs_entity_t*) rootdir, sizeof(dir_entity_t), rootdir->fs.lba, fs);
    fs.close();

    delete rootdir;
}