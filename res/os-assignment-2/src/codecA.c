#define CODEC_BUILD_STATIC 1

#include "codec.h"

void encode(char* str)
{
    while (*str != '\0') {
        if ('a' <= *str && *str <='z') {
            *str = *str + 'A' - 'a';
        } else if ('A' <= *str && *str <='Z') {
            *str = *str + 'a' - 'A';
        }
        str++;
    }
}

void decode(char* str)
{
    encode(str);
}
