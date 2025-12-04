#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "../include/del_space.h"

void del_space(char *s) {
    char new_str[1024];
    int j = 0;
    int previous_is_space = 0;

    for (size_t i = 0; s[i] != '\0' && j < 1023; i++) {
        if (s[i] == ' ') {
            if (!previous_is_space) {
                new_str[j++] = s[i];
                previous_is_space = 1;
            }
        } else {
            new_str[j++] = s[i];
            previous_is_space = 0;
        }
    }
    new_str[j] = '\0';
    strcpy(s, new_str);
}