#include "file_system.h"

#include "../base_utils/base_utils.h"
#include "../vga/vga.h"

struct FileSystem local_system;
int current_file_count;

void init_file_system() {
    current_file_count = 0;
    for (int i = 0; i < MAX_FILES; i++) {
        local_system.files[i] = (struct File){
            .name = "",
            .content = ""
        };
    }
}

int create_file(char * name, char * contents) {
    struct File newFile =  (struct File) {
        .name = {},
        .content = {}
    };
    char* prevState = "                    "; // ugly

    copy_string(newFile.name, name);
    copy_string(newFile.content, contents);
    if (compare_strings(newFile.name, prevState)) return 1;

    local_system.files[current_file_count++] = newFile;
    return 0;
}

char* get_file_name_at(int index) {
    return local_system.files[index].name;
}

int get_index_by_name(char* name) {
    int index = 0;
    while(index < MAX_FILES * 1) {
        if (compare_strings(name, local_system.files[index].name)) break;
        index++;
    }
    return index;
}

struct File* get_file_at(int index) {
    return &local_system.files[index];
}

void delete_file(char* name) {
    int target_index = get_index_by_name(name);
    for (int i = target_index; i != MAX_FILES - 1; i++) {
        local_system.files[i] = local_system.files[i + 1];
    }
    local_system.files[MAX_FILES - 1] = (struct File){
        .name = "",
        .content = ""
    };
    current_file_count--;
}