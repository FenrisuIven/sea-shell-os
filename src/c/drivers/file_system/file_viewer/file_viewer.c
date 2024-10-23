#include "file_viewer.h"
#include "../file_system.h"

#include "../../bash/bash.h"
#include "../../keyboard/keyboard.h"
#include "../../keyboard/key_actions_handlers.h"
#include "../../vga/vga.h"
#include "../../base_utils/base_utils.h"
#include "../../../kernel/kernel.h"

struct File* current_file;
bool file_is_set = false;

int local_input_line = 0;
int local_input_char = 0;
char local_input_buffer[FB_WIDTH][FB_HEIGHT];

void char_to_current_line(char character) {
    local_input_buffer[local_input_line][local_input_char++] = character;
}
void new_line_in_buffer() {
    local_input_line++;
}

void init_content_input_handler() {
    current_file = get_file_at(0);
}

int add_file_content(char * args) {
    if(local_input_line != 0) concat_strings(args, "\n");
    concat_strings(current_file->content, args);
    return 0;
}

int execute_accept_file_input(char* args) {
    if(!file_is_set) {
        trim_to_len(args, MAX_FILE_NAME);
        int target_index = get_index_by_name(args);
        if (target_index == -1) return 1;

        struct File* target = get_file_at(target_index);
        // out_message((struct Message){target->name, ERR_BG, ERR_FG, true});
        current_file = target;
        // out_message((struct Message){current_file->name, ERR_BG, ERR_FG, true});
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
            return;
            case KEY_ENTER:
                execute_accept_file_input(local_input_buffer[0]);
            return;
        }
        out_char((struct FramebufferChar){event.key_character, DEF_BG, DEF_FG, '\0'});
        char_to_current_line(event.key_character);

        if (local_input_char == 80) {
            start_next_line(true);
            new_line_in_buffer();
        }
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