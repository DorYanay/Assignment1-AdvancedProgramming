#define CODEC_BUILD_STATIC 1

#include "codec.h"

void encode(char* str)
{
    while (*str != '\0') {
        *str = *str + 3;
        str++;
    }
}

void decode(char* str)
{
    while (*str != '\0') {
        *str = *str - 3;
        str++;
    }
}
