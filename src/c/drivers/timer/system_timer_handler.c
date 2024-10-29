#include "system_timer_handler.h"
#include "timer.h"

#include "../base_utils/base_utils.h"
#include "../vga/screensaver/screensaver.h"


int system_ticks = 0;
void system_time_handler() {
    if (system_ticks == 10 * 20) {
        system_ticks = 0;
        timer_set_handler(void_func);
        init_screensaver();
    }
    system_ticks++;
}

void reset_system_ticks() {
    system_ticks = 0;
}