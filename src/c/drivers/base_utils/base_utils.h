#ifndef BASE_UTILS_H
#define BASE_UTILS_H
#include "../../kernel/kernel.h"

extern bool compareStrings(char* str1, char* str2);
extern void copyString(char* str1, char* str2);
extern void concatStrings(char* str1, char* str2);
extern int ceil(int num, int div);

#endif //BASE_UTILS_H