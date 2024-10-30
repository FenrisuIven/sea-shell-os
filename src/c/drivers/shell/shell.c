#include "shell.h"

#include "../timer/system_timer_handler.h"
#include "../timer/timer.h"
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
    {"help",  "| help: list of all commands present"},
    {"clear", "| clear: clears the screen"},
    {"hello", "| wouldn't you want to say hi to your pc?"},
    {"mk",    "| mk <name>: create new file"},
    {"write", "| w <name>: write in file"},
    {"read",  "| read <name>: read a file"},
    {"dir",   "| dir: displays all currently created files"},
    {"rem",   "| rem <name>: delete file"},
    {"term",  "| term : terminates any ongoing input"}
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

int execute_command (char *cmd, char *args) {
    switch (get_command_idx(cmd)) {
        default: execute_not_found(); break;

        case 0: execute_help();  break;
        case 1: execute_clear(); break;
        case 2: execute_hello(); break;
        case 3: //mk
            if(create_file(args) == 1) {
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
                return 0;
            }
            return 4;
        case 5: // read
            execute_read(args);
            break;
        case 6: execute_dir(); break;
        case 7: execute_remove_file(args); break;
    }
    return 0;
}

int parse_command_and_args() {
    char command[MAX_INPUT_SIZE] = {};
    char args[MAX_INPUT_SIZE] = {};
    int i = 0, cmdIdx = 0, argsIdx = 0;

    while (input_buffer[i] != ' ' && input_buffer[i] != '\0') {
        command[cmdIdx++] = input_buffer[i++];
    }
    command[cmdIdx] = '\0';
    i++;

    bool first_space = false;
    while (input_buffer[i] != '\0') {
        if (input_buffer[i] == ' ') {
            if (first_space) break;
            first_space = true;
        }
        else {
            first_space = false;
        }
        args[argsIdx++] = input_buffer[i++];
    }
    args[--argsIdx] = '\0';

    int res = execute_command(command, args);
    clear_input_buffer();
    return res;
}
void enter_action_handler() {
    bool print_cmd_start = char_counter != 3 && line_counter != 1;
    if(parse_command_and_args() == 4) print_cmd_start = false;
    char_counter = 0;
    update_pointer_position(0);
    if (print_cmd_start) out_message_cmd_start();
}
void backspace_action_handler() {
    rem_last_elem();
}

void shell_key_handler(struct keyboard_event event) {
    if (event.key && event.type == EVENT_KEY_PRESSED) {
        //on each input, reset system ticks
        //basically -- reset screensaver timer
        reset_system_ticks();

        // if 'special' action -- handle it then return, don't output any character to the screen
        switch(event.key) {
            case KEY_ENTER:
                key_Enter_Action(enter_action_handler);
                return;
            case KEY_BACKSPACE:
                key_Backspace_Action(backspace_action_handler);
                return;
            case KEY_LEFT_ALT:
                //init_screensaver();
                return;
        }

        //output char of pressed key and then add the char to input buffer
        out_char((struct FramebufferChar){event.key_character, DEF_BG, DEF_FG, '\0'});
        set_buffer_elem(event.key_character);

        //if afterwards the 'amount of entered chars' in buffer is equal to FB_WIDTH:
        //start new line and output an error
        if (char_counter == FB_WIDTH) {
            start_next_line(true);
            out_message((struct Message){"Error: Input buffer overflow", ERR_BG, ERR_FG, true });
            char_counter = 0;
            update_pointer_position(0);
            out_message_cmd_start();
            clear_input_buffer();
        }
    }
}

bool file_system_init = false;
void init_shell() {
    if (!file_system_init) {
        init_file_system();
        file_system_init = true;
    }
    timer_set_handler(system_time_handler);
    keyboard_set_handler(shell_key_handler);
    framebuffer = (char*)FB_START;
    clear_frame(true);
    clear_input_buffer();
}