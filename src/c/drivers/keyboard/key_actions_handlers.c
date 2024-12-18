#include "key_actions_handlers.h"
#include "../../kernel/kernel.h"
#include "../vga/vga.h"
#include "../shell/shell.h"

bool acceptingFileContents = false;

void key_Enter_Action(void action()) {
    start_next_line();
    action();
}

void key_Backspace_Action(void action()) {
    if (char_counter < 4) return;
    current_address -= 2;
    char_counter--;
    update_framebuffer_pointer();
    out_char((struct FramebufferChar){'\0', DEF_BG, DEF_FG, '\0'});
    update_pointer_position(0);
    action();
}

void key_Escape_Action(void action()) {
    action();
}

