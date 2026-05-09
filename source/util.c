

// void GRRLIB_PrintfTTF(int x, int y, GRRLIB_ttfFont *myFont, const char *string, unsigned int fontSize, const u32 color)
// Wrap this function so formatting is easier, so %s can be used.

#include <grrlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <ogc/lwp_watchdog.h>
#include <ogc/system.h>

// Version of GRRLIB_PrintfTTF that takes a format string and variable arguments, similar to printf.
void GRRLIB_PrintfTTF_f(int x, int y, GRRLIB_ttfFont *myFont, unsigned int fontSize, const u32 color, const char *string, ...)
{
    char buffer[256];
    va_list args;
    va_start(args, string);
    vsnprintf(buffer, sizeof(buffer), string, args);
    va_end(args);
    GRRLIB_PrintfTTF(x, y, myFont, buffer, fontSize, color);
}

void lower_string(char *str)
{
    for (int i = 0; str[i]; i++)
    {
        str[i] = tolower(str[i]);
    }
}

u64 map_u64(u64 v, u64 in_min, u64 in_max, u64 out_min, u64 out_max)
{
    if (in_min == in_max)
    {
        return out_min;
    }
    return (v - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

u64 getCurrentTimeMs()
{
    u64 ticks = gettime();

    // Convert ticks to milliseconds
    u64 msecs = ticks_to_millisecs(ticks);

    return msecs;
}

float lerp(float a, float b, float t)
{
    return a + t * (b - a);
}

float *lerp_vec3(const float *a, const float *b, float t)
{
    static float result[3];
    result[0] = lerp(a[0], b[0], t);
    result[1] = lerp(a[1], b[1], t);
    result[2] = lerp(a[2], b[2], t);
    return result;
}