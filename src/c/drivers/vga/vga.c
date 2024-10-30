#include "vga.h"

#include "../../kernel/kernel.h"
#include "../../drivers/keyboard/keyboard.h"
#include "../../drivers/timer/timer.h"
#include "../../drivers/serial_port/serial_port.h"
#include "../../drivers/base_utils/base_utils.h"
#include "../../drivers/shell/shell.h"

#define MAX_INPUT_SIZE 60


struct FramebufferString {
    char *message;
    enum COLORS bg;
    enum COLORS fg;
    u8 calculatedColorsByte;
};

int line_counter = 0;
int char_counter = 0;
int current_address = 0xb8000;

char* framebuffer;
void update_framebuffer_pointer() {
    framebuffer = (char *)current_address;
}
void framebuffer_set_char_at(struct FramebufferChar FBChar, int pos) {
    char* fb_pointer = (char*)FB_START;
    fb_pointer[pos] = FBChar.character;
    fb_pointer[pos + 1] = FBChar.calculated_colors_byte == '\0' ? (FBChar.bg << 4 | FBChar.fg) : FBChar.calculated_colors_byte;
}

void framebuffer_set_char(struct FramebufferChar FBChar) {
    *framebuffer = FBChar.character;
    *(framebuffer + 1) = FBChar.calculated_colors_byte == '\0' ? (FBChar.bg << 4 | FBChar.fg) : FBChar.calculated_colors_byte;
}

void put_cursor(unsigned short pos) {
    out(0x3D4, 14);
    out(0x3D5, ((pos >> 8) & 0x00FF));
    out(0x3D4, 15);
    out(0x3D5, pos & 0x00FF);
}

void update_pointer_position(int increment) {
    put_cursor((FB_WIDTH / 2) * line_counter + char_counter + increment);
}
void set_cursor(int row, int col) {
    put_cursor(FB_WIDTH * row + col);
}

void out_char (struct FramebufferChar FBChar) {
    int increment = 1;
    if (FBChar.character != '\0') {
        char_counter++;
        current_address+=2;
        increment = 0;
    }
    framebuffer_set_char(FBChar);
    update_framebuffer_pointer();
    update_pointer_position(increment);
}

void out_message_cmd_start() {
    out_message((struct Message){
        .message = " > ",
        DEF_BG,
        DEF_FG,
        false
    });
}

void out_tabulation() {
    int amountToPut = ceil(char_counter, 10) * 10;
    if (amountToPut == char_counter) amountToPut *= 2;

    for (int i = char_counter; i < amountToPut; i++) {
        out_char((struct FramebufferChar){' ', DEF_BG, DEF_FG, '\0'});
    }
}
void print_top_bar(char* message) {
    out_message((struct Message) { message, SHELL_BG, SHELL_FG, false});
    char* fb_pointer = (char*)FB_START;
    for(int i = 0; i < FB_WIDTH * 2; i+=2) {
        fb_pointer[i + 1] = SHELL_BG << 4 | SHELL_FG;
    }
    start_next_line();
}
void print_shell_version() {
    print_top_bar("Sea-Shell v0.0.1");
    out_message_cmd_start();
}

void scroll() {
    framebuffer = (char *) (FB_START + (FB_WIDTH * 2));
    int lastLine = 48 * FB_HEIGHT * 2;

    int i = 0;
    while (framebuffer != (char *) (FB_START + (FB_WIDTH * 2 * (FB_HEIGHT - 1)))) {
        framebuffer_set_char((struct FramebufferChar){
            *(framebuffer + FB_WIDTH * 2),
            0,
            0,
            *(framebuffer + FB_WIDTH * 2 + 1)
        });
        framebuffer += 2;
        i++;
    }

    while (framebuffer != (char *) (FB_START + (FB_WIDTH * 2 * FB_HEIGHT ))) {
        framebuffer_set_char((struct FramebufferChar){
            ' ',
            DEF_BG,
            DEF_FG,
            '\0'
        });
        framebuffer += 2;
    }

    current_address = FB_START + (FB_WIDTH * 2 * (FB_HEIGHT - 1));
    line_counter = 48;
    char_counter = 0;
    put_cursor(lastLine);
}

void scroll_needed() {
    scroll();
    line_counter = FB_HEIGHT * 2 - 2;
    update_framebuffer_pointer();
}

void start_next_line() {
    line_counter += 2;
    current_address = FB_START + (FB_WIDTH * line_counter);
    update_framebuffer_pointer();

    if (line_counter >= FB_HEIGHT * 2) {
        scroll_needed();
        return;
    }

    update_pointer_position(0);
    char_counter = 0;
}

void out_message (struct Message config) {
    char* message_copy = config.message;
    while (*message_copy != '\0') {
        if (*message_copy == '\n') {
            start_next_line();
            message_copy++;
        }
        out_char((struct FramebufferChar){*message_copy, config.bg, config.fg, '\0'});
        if (line_counter >= FB_HEIGHT * 2) {
            scroll_needed();
            return;
        }
        message_copy++;
    }
    if (config.terminate) start_next_line();
}

void out_message_command (int cmdIdx, struct Message config) {
    char *cmdInfo[2];
    cmdInfo[0] = COMMANDS[cmdIdx].name;
    cmdInfo[1] = COMMANDS[cmdIdx].description;
    config.terminate = false;

    for (int i = 0; i < 2; i++) {
        config.message = cmdInfo[i];
        out_message(config);
        if (i != 1) out_tabulation();

    }
    start_next_line();

    update_pointer_position(-char_counter);
}

void clear_frame(bool print_shell_ver) {
    line_counter = 0;
    char_counter = 0;
    set_buffer_elem('\0');
    current_address = FB_START;
    update_framebuffer_pointer();

    char* fb_pointer = (char*)current_address;
    while (fb_pointer != (char *)(current_address + (160 * 25))) {
        *fb_pointer = ' ';
        *(fb_pointer + 1) = DEF_BG << 4 | DEF_FG;
        fb_pointer += 2;
    }

    if(print_shell_ver) print_shell_version();
}

void paint_frame(enum COLORS color) {
    char* fb_pointer = (char*)FB_START;
    for (int i = 0; i < FB_WIDTH * FB_HEIGHT * 2; i+=2) {
        fb_pointer[i + 1] = color;
    }
}

void init_framebuffer() {
    line_counter = 0;
    char_counter = 0;
    current_address = FB_START;
    update_framebuffer_pointer();
}