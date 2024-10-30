#include "file_viewer.h"
#include "../file_system.h"

#include "../../bash/bash.h"
#include "../../keyboard/keyboard.h"
#include "../../keyboard/key_actions_handlers.h"
#include "../../vga/vga.h"
#include "../../base_utils/base_utils.h"
#include "../../../kernel/kernel.h"

struct File* current_file;
int file_index;
bool file_is_set = false;

int input_buffer_index = 0;
char local_input_buffer[FB_WIDTH * FB_HEIGHT];

void init_content_input_handler() {
    file_is_set = false;
    current_file = get_file_at(0);
    file_index = 0;
    input_buffer_index = 0;
    local_input_buffer[0] = '\0';
}

void char_to_current_line(char character) {
    local_input_buffer[input_buffer_index++] = character;
}
void new_line_in_buffer() {
    char_to_current_line('\n');
}

int add_file_content(char * args) {
    set_content_for(file_index, args);
    return 0;
}

int execute_accept_file_input(char* args) {
    if(!file_is_set) {
        trim_to_len(args, MAX_FILE_NAME);
        int target_index = get_index_by_name(args);
        if (target_index == -1) return 1;

        file_index = target_index;
        struct File* target = get_file_at(target_index);
        current_file = target;
        file_is_set = true;
        return 0;
    }
    if(add_file_content(args) == 0) return 0;
}

void print_editor_status() {
    clear_frame(false);
    char message[20 + MAX_FILE_NAME] = "Viewer v0.0.1 - ";
    concat_strings(message, current_file->name);
    print_top_bar(message);
}

void viewer_key_handler(struct keyboard_event event) {
    if (event.key && event.type == EVENT_KEY_PRESSED) {
        switch(event.key) {
            case KEY_ESC:
                key_Escape_Action(init_bash);
                execute_accept_file_input(local_input_buffer);
            return;
            case KEY_ENTER:
                new_line_in_buffer();
                start_next_line();
            return;
        }
        out_char((struct FramebufferChar){event.key_character, DEF_BG, DEF_FG, '\0'});
        char_to_current_line(event.key_character);
    }
}

int init_viewer(char* name) {
    init_content_input_handler();
    if(execute_accept_file_input(name) == 1) return 1;

    keyboard_set_handler(viewer_key_handler);
    print_editor_status();
    clear_input_buffer();
    return 0;
}