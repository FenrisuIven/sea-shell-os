#include "drivers/timer/system_timer_handler.h"
#include "drivers/base_utils/base_utils.h"
#include "kernel/kernel.h"
#include "drivers/keyboard/keyboard.h"
#include "drivers/timer/timer.h"
#include "drivers/serial_port/serial_port.h"
#include "drivers/vga/vga.h"
#include "drivers/shell/shell.h"
#include "drivers/vga/screensaver/screensaver.h"

_Noreturn void halt_loop() {
    while (1) { halt(); }
}

void exception_handler(u32 interrupt, u32 error, char *message) {
    serial_log(LOG_ERROR, message);
}

void init_kernel() {
    init_gdt();
    init_idt();
    init_exception_handlers();
    init_interrupt_handlers();
    register_timer_interrupt_handler();
    register_keyboard_interrupt_handler();
    configure_default_serial_port();
    set_exception_handler(exception_handler);
    enable_interrupts();
}

void kernel_entry() {
    init_kernel();
    timer_set_handler(system_time_handler);

    init_shell();

    halt_loop();
}