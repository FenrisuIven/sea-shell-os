#include "../../kernel/kernel.h"
#include "base_utils.h"

bool compareStrings(char* str1, char* str2) {
    int i = 0;
    while (str1[i] != '\0' && str2[i] != '\0') {
        if (str1[i] != str2[i]) {
            return false;
        }
        i++;
    }
    return str1[i] == '\0' && str2[i] == '\0';
}

void copyString(char * str1, char * str2) {
    while (*str2 != '\0') {
        *str1++ = *str2++;
    }
    *str1 = '\0';
}

void concatStrings(char* str1, char* str2) {
    while (*str1 != '\0') str1++;

    while(*str2 != '\0') {
        *str1++ = *str2++;
    }
    *str1 = '\0';
}

int ceil(int num, int div) {
    return (num + div - 1) / div;
}