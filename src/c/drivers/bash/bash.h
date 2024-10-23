#ifndef CMD_H
#define CMD_H

#define COMMANDS_COUNT 9
#define COMMANDS_SYSTEM_RESERVED 1
#define MAX_INPUT_SIZE 60

struct Command {
    char *name;
    char *description;
};
extern struct Command COMMANDS[COMMANDS_COUNT];

extern void clear_input_buffer();
extern void set_buffer_elem(char value);
extern void rem_last_elem();

extern int handle_command();

extern void init_bash();

#endif //CMD_H