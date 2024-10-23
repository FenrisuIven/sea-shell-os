#ifndef KEYHANDLERS_H
#define KEYHANDLERS_H
#include "../../kernel/kernel.h"

extern bool acceptingFileContents;

extern void key_Enter_Action(void handler());
extern void key_Backspace_Action(void handler());
extern void key_Escape_Action(void action());

#endif //KEYHANDLERS_H