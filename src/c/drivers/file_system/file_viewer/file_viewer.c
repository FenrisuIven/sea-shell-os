#include "file_viewer.h"

#include "../../bash/bash.h"
#include "../../keyboard/keyboard.h"
#include "../../vga/vga.h"

void clear_screen() {
    clear_frame(false);
    out_message((struct Message) {
        .message = "Viewer v0.0.1",
        .bg = DEF_BG,
        .fg = DEF_FG,
        true
    });
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

void init_viewer() {
    keyboard_set_handler(viewer_key_handler);
    clear_screen();
    clear_input_buffer();
}