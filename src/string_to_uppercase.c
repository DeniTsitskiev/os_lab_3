#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "../include/string_to_uppercase.h"

void string_to_uppercase(char *s) {
    for (size_t i = 0; s[i] != '\0'; i++) {
        if (s[i] >= 'a' && s[i] <= 'z') {
            s[i] = s[i] - 'a' + 'A';
        }
    }
}