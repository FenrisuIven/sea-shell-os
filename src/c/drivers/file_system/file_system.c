#include "file_system.h"

#include "../base_utils/base_utils.h"
#include "../vga/vga.h"
#include "../shell/command_handlers/command_handlers.h"

struct File local_system[MAX_FILES];
int current_file_count;

void init_file_system() {
    current_file_count = 0;
    for (int i = 0; i < MAX_FILES; i++) {
        local_system[i] = (struct File) {};
        copy_string(local_system[i].name, "");
        copy_string(local_system[i].content, "");
    }
}

void set_content_for(int index, char* content) {
    trim_to_len(content, MAX_CHAR_COUNT - 1);
    copy_string(local_system[index].content, content);
}

int create_file(char* name) {
    local_system[current_file_count] = (struct File) {};

    trim_to_len(name, MAX_FILE_NAME - 1);
    copy_string(local_system[current_file_count++].name, name);

    return 0;
}

char* get_file_name_at(int index) {
    return local_system[index].name;
}

int get_index_by_name(char* name) {
    trim_to_len(name, MAX_FILE_NAME - 1);
    int index = 0;
    bool found = false;
    while(index < MAX_FILES * 1) {
        if (compare_strings(name, local_system[index].name)) {
            found = true;
            break;
        }
        index++;
    }
    return found ? index : - 1;
}

struct File* get_file_at(int index) {
    return &local_system[index];
}

void delete_file(char* name) {
    int target_index = get_index_by_name(name);
    for (int i = target_index; i != MAX_FILES - 1; i++) {
        local_system[i] = local_system[i + 1];
    }
    local_system[MAX_FILES - 1] = (struct File){
        .name = "",
        .content = ""
    };
    current_file_count--;
    execute_dir();
}