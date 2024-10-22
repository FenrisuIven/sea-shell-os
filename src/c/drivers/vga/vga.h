#ifndef DISPLAY_H
#define DISPLAY_H
#include "../../kernel/kernel.h"

#define FB_START 0xb8000
#define FB_WIDTH 80
#define FB_HEIGHT 25

struct Message {
    char* message;
    int bg;
    int fg;
    bool terminate;
};

enum COLORS {
    DEF_BG      = 0x3,  // cyan
    DEF_FG      = 0xf,  // white
    SYSTEM_FG   = 0xf,  // white
    SHELL_BG    = 0x9,  // l_blue
    SHELL_FG    = 0xf,  // white
    ERR_BG      = 0x4,  // red
    ERR_FG      = 0xc,  // l_red

    BLACK       = 0x0,
    BLUE        = 0x1,
    GREEN       = 0x2,
    CYAN        = 0x3,
    RED         = 0x4,
    MAGENTA     = 0x5,
    BROWN       = 0x6,
    L_GRAY      = 0x7,
    D_GRAY      = 0x8,
    L_BLUE      = 0x9,
    L_GREEN     = 0xa,
    L_CYAN      = 0xb,
    L_RED       = 0xc,
    PINK        = 0xd,
    YELLOW      = 0xe,
    WHITE       = 0xf
};

struct FramebufferChar {
    char character;
    enum COLORS bg;
    enum COLORS fg;
    u8 calculatedColorsByte;
};

extern int line_counter;
extern int char_counter;
extern int current_address;
extern char* framebuffer;

extern void paint_frame(enum COLORS color);
extern void framebuffer_set_char_at(struct FramebufferChar FBChar, int pos);
extern void update_framebuffer_pointer();
extern void update_pointer_position(int increment);
extern void out_char (struct FramebufferChar);
extern void out_message (struct Message config);
extern void out_message_command (int cmdIdx, struct Message config);
extern void out_message_cmd_start();
extern void clear_frame(bool print_shell_ver);
extern void start_next_line();
extern void scroll_needed();

#endif //DISPLAY_H