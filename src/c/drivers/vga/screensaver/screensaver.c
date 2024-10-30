#include "screensaver.h"

#include "../../serial_port/serial_port.h"
#include "../../bash/bash.h"
#include "../../keyboard/keyboard.h"
#include "../../timer/timer.h"
#include "../../base_utils/base_utils.h"
#include "../../base_utils/int_to_char.h"
#include "../../base_utils/random.h"
#include "../vga.h"

#define MAX_CHARS 20

enum COLORS characters_colors[] = {
    BLUE,
    L_BLUE,
    RED,
    L_RED,
    GREEN,
    L_GREEN,
    YELLOW,
    BROWN,
};

struct Char_To_Display {
    char character;
    enum COLORS bg;
    enum COLORS fg;
    int row_index;
    int col_index;
    int current_tick;
    int move_offset;
    bool movable;
};

char get_char_by_col(int col_idx) {
    return (col_idx + 1) + 64;
    // return 0xB0;
}

int count_present_chars = 0;
struct Char_To_Display screensaver_chars[FB_WIDTH];

struct Char_To_Display init_char() {
    int col_idx = custom_rand(80);
    bool unique = false;
    while (!unique) {
        unique = true;
        for (int j = 0; j < MAX_CHARS; j++) {
            if (col_idx == screensaver_chars[j].col_index || col_idx == 80) {
                change_seed(j);
                col_idx = custom_rand(80);
                unique = false;
                break;
            }
        }
    }

    int move_offset = custom_rand(10);
    if (move_offset < 3) move_offset == 0 ? move_offset = 3 : move_offset == 1 ? move_offset == 5 : move_offset;
    return (struct Char_To_Display) {
        .character = get_char_by_col(col_idx),
        .bg = BLACK,
        .fg = characters_colors[col_idx / 10],
        .col_index = col_idx,
        .row_index = 0,
        .current_tick = 0,
         move_offset,
        .movable = true
    };
}

void init_array_of_chars() {
    int count = 0;
    count_present_chars = 0;
    for (count = 0; count != MAX_CHARS / 5; count++) {
        change_seed(count);
        screensaver_chars[count] = init_char();
        count_present_chars++;
    }
    for (; count != MAX_CHARS; count++) {
        screensaver_chars[count] = (struct Char_To_Display) { .movable = false };
    }
}

void draw_char(struct Char_To_Display* current_char) {
    framebuffer_set_char_at((struct FramebufferChar){
        current_char->character,
        current_char->bg,
        current_char->fg,
        '\0'},
        (FB_WIDTH * current_char->row_index + current_char->col_index) * 2);
}

void display_chars() {
    for(int i = 0; i != MAX_CHARS; i++) {
        draw_char(&screensaver_chars[i]);
    }
}

int calc_fb_index(int row_idx, int col_idx) {
    return (FB_WIDTH * row_idx + col_idx) * 2;
}

bool scrolling = false;
void scroll_screensaver() {
    scrolling = true;

    char* framebuffer_pointer = (char*)FB_START;
    for(int i = 24; i != 0; i--) {
        for (int j = 79; j != -1; j--) {
            framebuffer_pointer[calc_fb_index(i, j)] = framebuffer_pointer[calc_fb_index(i - 1, j)];
            framebuffer_pointer[calc_fb_index(i,j) + 1] = framebuffer_pointer[calc_fb_index(i - 1, j) + 1];
        }
    }

    for (int i = 0; i != count_present_chars; i ++) {
        screensaver_chars[i].row_index++;
    }

    scrolling = false;
}

void set_not_movable(int idx) {
    for (int i = idx; i != count_present_chars; i++) {
        screensaver_chars[i] = screensaver_chars[i + 1];
    }
    screensaver_chars[count_present_chars - 1] = init_char();
}

bool check_for_null_bellow(int row_idx, int col_idx) {
    char* temp_framebuffer_pointer = (char*)FB_START;
    char target_char = temp_framebuffer_pointer[(FB_WIDTH * (row_idx + 1) + col_idx) * 2];
    return target_char == get_char_by_col(col_idx);
}

bool check_if_line_clear(int row_idx) {
    char* temp_framebuffer_pointer = (char*)FB_START;
    int count_elems = 0;
    for(int i = 0; i != 80; i++) {
        char fb_char = temp_framebuffer_pointer[calc_fb_index(row_idx, i)];
        if (fb_char != '\0' && fb_char != ' ') count_elems++;
    }
    if (count_elems > 70) {
        return false;
    }
    return true;
}

int local_ticks = 0;
void screensaver_timer_handler() {
    if (scrolling) return;

    for (int i = 0; i != count_present_chars && i < MAX_CHARS; i++) {
        struct Char_To_Display* curr = &screensaver_chars[i];
        if (!curr->movable) continue;

        if (curr->current_tick == curr->move_offset) {
            draw_char(&(struct Char_To_Display){'\0',BLACK, BLACK, curr->row_index, curr->col_index});
            if(check_for_null_bellow(curr->row_index, curr->col_index) || curr->row_index == 24) {
                draw_char(curr);
                set_not_movable(i);

                while (!check_if_line_clear(24)) {
                    scroll_screensaver();
                }

                continue;
            }
            curr->row_index += 1;
            draw_char(curr);
            curr->current_tick = 0;
        }
        if(count_present_chars < MAX_CHARS && local_ticks % MAX_CHARS * 5 == 0) {
            screensaver_chars[count_present_chars++] = init_char();
        }
        curr->current_tick++;
    }
    local_ticks++;
}

void screensaver_key_handler(struct keyboard_event event) {
    if (event.key && event.type == EVENT_KEY_PRESSED) {
        timer_set_handler(void_func);
        init_bash();
    }
}

void init_screensaver() {
    init_array_of_chars();
    clear_frame(false);
    paint_frame(BLACK);
    timer_set_handler(screensaver_timer_handler);
    keyboard_set_handler(screensaver_key_handler);
}