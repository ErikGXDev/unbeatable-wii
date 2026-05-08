

// void GRRLIB_PrintfTTF(int x, int y, GRRLIB_ttfFont *myFont, const char *string, unsigned int fontSize, const u32 color)
// Wrap this function so formatting is easier, so %s can be used.

#include <grrlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>

// Version of GRRLIB_PrintfTTF that takes a format string and variable arguments, similar to printf.
void GRRLIB_PrintfTTF_f(int x, int y, GRRLIB_ttfFont *myFont, unsigned int fontSize, const u32 color, const char *string, ...) {
    char buffer[256];
    va_list args;
    va_start(args, string);
    vsnprintf(buffer, sizeof(buffer), string, args);
    va_end(args);
    GRRLIB_PrintfTTF(x, y, myFont, buffer, fontSize, color);
}

void lower_string(char *str) {
    for (int i = 0; str[i]; i++) {
      str[i] = tolower(str[i]);
    }
}