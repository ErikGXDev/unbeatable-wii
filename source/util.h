

#include <grrlib.h>

void GRRLIB_PrintfTTF_f(int x, int y, GRRLIB_ttfFont *myFont, unsigned int fontSize, const u32 color, const char *string, ...);

void lower_string(char *str);

u64 map_u64(u64 v, u64 in_min, u64 in_max, u64 out_min, u64 out_max);

u64 getCurrentTimeMs();

float lerp(float a, float b, float t);
float *lerp_vec3(const float *a, const float *b, float t);

#define UNPACK3(arr) (arr)[0], (arr)[1], (arr)[2]

#define ASSIGN3(dest, src) \
  do                       \
  {                        \
    (dest)[0] = (src)[0];  \
    (dest)[1] = (src)[1];  \
    (dest)[2] = (src)[2];  \
  } while (0)
