#include "file_viewer.h"
#include "../file_system.h"

#include "../../bash/bash.h"
#include "../../keyboard/keyboard.h"
#include "../../vga/vga.h"
#include "../../base_utils/base_utils.h"
#include "../../../kernel/kernel.h"

struct File* current_file;
bool file_is_set = false;

void init_content_input_handler() {
    current_file = get_file_at(0);
}

int add_file_content(char * args) {
    concat_strings(args, "\n");
    concat_strings(current_file->content, args);
    return 0;
}

int execute_accept_file_input(char* args) {
    if(!file_is_set) {
        trim_to_len(args, MAX_FILE_NAME);
        struct File* target = get_file_at(get_index_by_name(args));
        if (compare_strings(target->name, "")) return 1;

        current_file = get_file_at(get_index_by_name(args));
        file_is_set = true;
        return 0;
    }
    if(add_file_content(args) == 0) return 0;
}

void clear_screen() {
    clear_frame(false);
    char message[20 + MAX_FILE_NAME] = "Viewer v0.0.1 - ";
    concat_strings(message, current_file->name);
    print_top_bar(message);
}

void viewer_key_handler(struct keyboard_event event) {
    if (event.key && event.type == EVENT_KEY_PRESSED) {
        out_char((struct FramebufferChar){event.key_character, DEF_BG, DEF_FG, '\0'});
        set_buffer_elem(event.key_character);

        if (char_counter == 80) {
            start_next_line(true);
        }
    }
}

int init_viewer(char* name) {
    if(execute_accept_file_input(name)) return 1;

    init_content_input_handler();
    keyboard_set_handler(viewer_key_handler);
    clear_screen();
    clear_input_buffer();
    return 0;
}