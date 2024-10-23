#ifndef BASE_UTILS_H
#define BASE_UTILS_H
#include "../../kernel/kernel.h"

extern void void_func();
extern bool compare_strings(char* str1, char* str2);
extern void copy_string(char* str1, char* str2);
extern void concat_strings(char* str1, char* str2);
extern void trim_to_len(char* str, int len);
extern int str_len(char* str);
extern int ceil(int num, int div);

#endif //BASE_UTILS_H