#include "bash.h"

#include "../keyboard/keyboard.h"
#include "../keyboard/key_actions_handlers.h"
#include "../../kernel/kernel.h"
#include "../../drivers/base_utils/base_utils.h"
#include "../file_system/file_viewer/file_viewer.h"
#include "../../drivers/vga/vga.h"
#include "../file_system/file_system.h"
#include "./command_handlers/command_handlers.h"
#include "../vga/screensaver/screensaver.h"

char input_buffer[MAX_INPUT_SIZE];
int buffer_idx = 0;

struct Command COMMANDS[COMMANDS_COUNT] = {
    {"help", "| help: list of all commands present"},
    {"clear", "| clear: clears the screen"},
    {"hello", "| wouldn't you want to say hi to your pc?"},
    {"mk", "| mk <name>: create new file"},
    {"write", "| w <name>: write in file"},
    {"read", "| read <name>: read a file"},
    {"dir", "| dir: displays all currently created files"},
    {"rem", "| rem <name>: delete file"},
    {"term", "| term : terminates any ongoing input"}
};

void set_buffer_elem(char value) {
    input_buffer[buffer_idx++] = value;
}
void rem_last_elem() {
    input_buffer[--buffer_idx] = '\0';
}

void clear_input_buffer() {
    int i = 0;
    while (i != MAX_INPUT_SIZE) {
        input_buffer[i++] = ' ';
    }
    buffer_idx = 0;
    input_buffer[i] = '\0';
}

int get_command_idx(char *cmd) {
    for (int i = 0; i != COMMANDS_COUNT * 1; i++) {
        if (compare_strings(cmd, COMMANDS[i].name)) {
            return i;
        }
    }
    return -1;
}

void execute_command (char *cmd, char *args) {
    switch (get_command_idx(cmd)) {
        default: execute_not_found(); break;

        case 0: execute_help();  break;
        case 1: execute_clear(); break;
        case 2: execute_hello(); break;
        case 3: //mk
            if(create_file(args, "") == 1) {
                out_message((struct Message){
                    "Error: Could not create file",
                    ERR_BG, ERR_FG, true,
                });
                break;
            }
            out_message((struct Message){
                " - File created successfully",
                DEF_BG, DEF_FG, true,
            });
            break;
        case 4: //write
            if (init_viewer(args) == 1) {
                out_message((struct Message){
                    "Error: Could not read file",
                    ERR_BG, ERR_FG, true,
                });
            }
            break;
        case 5: // read
            execute_read(args);
            break;
        case 6: execute_dir(); break;
        case 7: acceptingFileContents = false; break;
    }
}

void handle_command() {
    char command[MAX_INPUT_SIZE] = {};
    char args[MAX_INPUT_SIZE] = {};
    int i = 0, cmdIdx = 0, argsIdx = 0;

    while (input_buffer[i] != ' ' && input_buffer[i] != '\0') {
        command[cmdIdx++] = input_buffer[i++];
    }
    command[cmdIdx] = '\0';
    i++;

    while (input_buffer[i] != '\0') {
        args[argsIdx++] = input_buffer[i++];
    }
    args[argsIdx] = '\0';

    execute_command(command, args);
    clear_input_buffer();
}

void bash_key_handler(struct keyboard_event event) {
    if (event.key && event.type == EVENT_KEY_PRESSED) {
        switch(event.key) {
            case KEY_ESC:
                key_Escape_Action();
                return;
            case KEY_ENTER:
                key_Enter_Action();
                return;
            case KEY_BACKSPACE:
                key_Backspace_Action();
                return;
            case KEY_LEFT_ALT:
                init_screensaver();
                return;
        }

        out_char((struct FramebufferChar){event.key_character, DEF_BG, DEF_FG, '\0'});
        set_buffer_elem(event.key_character);

        if (char_counter == 80) {
            start_next_line(true);
        }
    }
}

void bash_entry() {
    keyboard_set_handler(bash_key_handler);
}