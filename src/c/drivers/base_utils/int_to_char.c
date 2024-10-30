#include "int_to_char.h"

char* to_char(int num) {
    static char res[36];
    int i = 0;
    int isNegative = 0;

    if (num < 0) {
        isNegative = 1;
        num = -num;
    }

    do {
        res[i++] = (num % 10) + '0';
        num /= 10;
    } while (num > 0);

    if (isNegative) {
        res[i++] = '-';
    }

    res[i] = '\0';

    int start = 0;
    int end = i - 1;
    while (start < end) {
        char temp = res[start];
        res[start] = res[end];
        res[end] = temp;
        start++;
        end--;
    }
    return res;
}