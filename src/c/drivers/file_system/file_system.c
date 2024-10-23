#include "file_system.h"

#include "../base_utils/base_utils.h"
#include "../vga/vga.h"
#include "../bash/command_handlers/command_handlers.h"

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

void set_content_for(int index, char* content) {
    trim_to_len(content, MAX_CHAR_COUNT);
    copy_string(local_system.files[index].content, content);
    out_message((struct Message){local_system.files[index].content, RED, BLACK, true});
    out_message((struct Message){local_system.files[index].name, RED, BLACK, true});
}

char* get_file_name_at(int index) {
    return local_system.files[index].name;
}

int get_index_by_name(char* name) {
    int index = 0;
    bool found = false;
    while(index < MAX_FILES * 1) {
        if (compare_strings(name, local_system.files[index].name)) {
            found = true;
            break;
        }
        index++;
    }
    return found ? index : - 1;
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
    execute_dir();
}