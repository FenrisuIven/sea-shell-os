#include "../base_utils/base_utils.h"
#include "../bash/bash.h"
#include "../bash/command_handlers/command_handlers.h"
#include "../../kernel/kernel.h"
#include "../vga/vga.h"
#include "file_system.h"
#include "../keyboard/key_actions_handlers.h"

struct File* current_file;
bool file_is_set = false;

void init_content_input_handler() {
    current_file = get_file_at(0);
}

void add_file_content(char * args) {
    concatStrings(args, "\n");
    // out_message(args, DEBUG_BG, DEBUG_FG, true);
    concatStrings(current_file->content, args);
}

void execute_accept_file_input(char * args) {
    if(!file_is_set) {
        current_file = get_file_at(get_index_by_name(args));
        file_is_set = true;
        return;
    }
    // out_message(args, ERROR_BG, ERROR_FG, true);
    add_file_content(args);
}