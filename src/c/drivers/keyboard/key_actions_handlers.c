#include "key_actions_handlers.h"
#include "../../kernel/kernel.h"
#include "../vga/vga.h"
#include "../bash/bash.h"

bool acceptingFileContents = false;

void key_Enter_Action() {
    start_next_line();
    handle_command();
    if (acceptingFileContents) {
        char_counter = 0;
        update_pointer_position(0);
        return;
    }
    if (char_counter != 3 && line_counter != 1) out_message_cmd_start();
}

void key_Backspace_Action() {
    if (char_counter < 4 && !acceptingFileContents) return;
    current_address -= 2;
    char_counter--;
    update_framebuffer_pointer();
    out_char((struct FramebufferChar){'\0', DEF_BG, DEF_FG, '\0'});
    update_pointer_position(0);
    rem_last_elem();
}

void key_Escape_Action() {
    if (acceptingFileContents) {
        char_counter = 0;
        update_pointer_position(0);
        acceptingFileContents = false;
        out_message_cmd_start();
    }
}