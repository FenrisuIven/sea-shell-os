#include "../shell.h"
#include "command_handlers.h"

#include "../../base_utils/base_utils.h"
#include "../../../kernel/kernel.h"
#include "../../vga/vga.h"
#include "../../file_system/file_system.h"

void execute_not_found() {
    out_message((struct Message){
        .message = "Error: Command was not found",
        ERR_BG,
        ERR_FG,
        true
    });
}

void execute_help() {
    int i = 0;
    while (i != COMMANDS_COUNT - (COMMANDS_SYSTEM_RESERVED * 1)) {
        out_message_command(i++, (struct Message) {
            .message = "",
            SYSTEM_BG,
            SYSTEM_FG,
            true
        });
    }
}

void execute_clear() {
    out_message((struct Message) {
            .message = "clear!",
            DEF_BG,
            DEF_FG,
            true
        });
    clear_frame(true);
}

void execute_hello() {
    out_message((struct Message) {
            .message = " - Well hello to you as well!",
            SYSTEM_BG,
            SYSTEM_FG,
            true
        });
}

void execute_dir() {
    out_message((struct Message){"| root:", SYSTEM_BG, SYSTEM_FG, true});
    int i = 0;
    while (i != MAX_FILES * 1 && !compare_strings("", get_file_name_at(i))) {
        char message[MAX_FILE_NAME + 5] = "-- | ";
        concat_strings(message, get_file_name_at(i++));
        out_message((struct Message) {
            message,
            SYSTEM_BG,
            SYSTEM_FG,
            true
        });
    }
}

void execute_read(char* name) {
    trim_to_len(name, MAX_FILE_NAME);
    int target_index = get_index_by_name(name);
    if (target_index == -1) {
        out_message((struct Message) {"Error: File was not found",ERR_BG,ERR_FG,true});
        return;
    }
    struct File* target = get_file_at(target_index);
    out_message((struct Message) {
        target->content,
        SYSTEM_BG,
        SYSTEM_FG,
        true
    });
}

void execute_remove_file(char* name) {
    trim_to_len(name, MAX_FILE_NAME);
    int target_index = get_index_by_name(name);
    if (target_index == -1) {
        out_message((struct Message) {"Error: File was not found",ERR_BG,ERR_FG,true});
        return;
    }
    delete_file(name);
}