#include "file_viewer.h"

#include "../../serial_port/serial_port.h"
#include "../../timer/timer.h"
#include "../file_system.h"

#include "../../shell/shell.h"
#include "../../keyboard/keyboard.h"
#include "../../keyboard/key_actions_handlers.h"
#include "../../vga/vga.h"
#include "../../base_utils/base_utils.h"
#include "../../base_utils/int_to_char.h"
#include "../../../kernel/kernel.h"

struct File* current_file;
int file_index;
bool file_is_set = false;

int input_buffer_index = 0;
char local_input_buffer[FB_WIDTH * FB_HEIGHT];

int cursor = 0;
int global_pos[2];

void init_content_input_handler() {
    file_is_set = false;
    current_file = get_file_at(0);
    file_index = 0;
    input_buffer_index = 0;
    cursor = 0;
    global_pos[0] = 0;
    global_pos[1] = 0;
    local_input_buffer[0] = '\0';
}

void insert_char(char character) {
    for (int i = input_buffer_index + 1; i > cursor; i--) {
        local_input_buffer[i] = local_input_buffer[i - 1];
    }
    local_input_buffer[cursor] = character;
    input_buffer_index++;
}

void set_char(char character) {
    int index = input_buffer_index;
    if (cursor != input_buffer_index && cursor < input_buffer_index) index = cursor;
    if (local_input_buffer[index] == '\n') {
        insert_char(character);
    }
    else {
        local_input_buffer[index] = character;
        input_buffer_index++;
    }
}
void new_line_in_buffer() {
    set_char('\n');
    cursor++;
}

void rem_last_char() {
    local_input_buffer[cursor] = '\0';
    input_buffer_index--;
}

void get_fb_pos() {
    int index = 0;
    int row = 0;
    int col = 0;
    while (local_input_buffer[index] != '\0') {
        if (index == cursor) break;
        if (local_input_buffer[index] == '\n') {
            row++;
            col = 0;
            index++;
            continue;
        }
        col++;
        index++;
    }
    global_pos[0] = row;
    global_pos[1] = col;
}
void update_viewer_cursor() {
    get_fb_pos();
    set_cursor(global_pos[0] + 1, global_pos[1]);
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

void move_internal_cursor(enum key action) {
    switch (action) {
        case KEY_KEYPAD_4:  //to the left
            if (cursor != 0) cursor--;
            serial_print(to_char(cursor));
            break;
        case KEY_KEYPAD_6:  // to the right
            if (cursor != input_buffer_index) cursor++;
            serial_print(to_char(cursor));
            break;
    }
    update_viewer_cursor();
}

void viewer_key_handler(struct keyboard_event event) {
    if (event.key && event.type == EVENT_KEY_PRESSED) {
        switch(event.key) {
            case KEY_KEYPAD_6:
            case KEY_KEYPAD_4:
                move_internal_cursor(event.key);
                return;
            case KEY_ESC:
                key_Escape_Action(init_shell);
                execute_accept_file_input(local_input_buffer);
            return;
            case KEY_ENTER:
                new_line_in_buffer();
                update_viewer_cursor();
                serial_print(to_char(cursor));
            return;
            case KEY_BACKSPACE:
                cursor = input_buffer_index - 1;
                rem_last_char();
                // serial_print(local_input_buffer);
                // serial_print("\n");
                get_fb_pos();
                framebuffer_set_char_at((struct FramebufferChar){
                        ' ', DEF_BG, DEF_FG, '\0'},
                        (FB_WIDTH * global_pos[0] + global_pos[1]) * 2
                    );
                update_viewer_cursor();
                return;
        }
        framebuffer_set_char_at((struct FramebufferChar){
            event.key_character, DEF_BG, DEF_FG, '\0'},
            (FB_WIDTH * global_pos[0] + global_pos[1]) * 2
        );
        set_char(event.key_character);
        cursor++;
        update_viewer_cursor();
        serial_print(to_char(cursor));
    }
}

int init_viewer(char* name) {
    init_content_input_handler();
    timer_set_handler(void_func);
    if(execute_accept_file_input(name) == 1) return 1;

    keyboard_set_handler(viewer_key_handler);
    print_editor_status();
    clear_input_buffer();
    return 0;
}