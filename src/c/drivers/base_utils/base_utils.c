#include "../../kernel/kernel.h"
#include "base_utils.h"

void void_func() {}

bool compare_strings(char* str1, char* str2) {
    int i = 0;
    while (str1[i] != '\0' && str2[i] != '\0') {
        if (str1[i] != str2[i]) {
            return false;
        }
        i++;
    }
    return str1[i] == '\0' && str2[i] == '\0';
}

void copy_string(char * str1, char * str2) {
    while (*str2 != '\0') {
        *str1++ = *str2++;
    }
    *str1 = '\0';
}

void concat_strings(char* str1, char* str2) {
    while (*str1 != '\0') str1++;

    while(*str2 != '\0') {
        *str1++ = *str2++;
    }
    *str1 = '\0';
}

void trim_to_len(char* str, int len) {
    str[len] = '\0';
}

int str_len(char* str) {
    int len = 0;
    while (str[len] != '\0') len++;
    return len;
}

int ceil(int num, int div) {
    return (num + div - 1) / div;
}