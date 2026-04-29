#include <math.h>
#include <gccore.h>


void DrawSunlitVertex(const float *pos, const float *normal, const float *uv, const float *sun_dir, unsigned int sun_color, unsigned int ambient_color) {
    // Normalize sunlight direction
    float sdir[3] = { sun_dir[0], sun_dir[1], sun_dir[2] };
    float sun_len = sqrtf(sdir[0]*sdir[0] + sdir[1]*sdir[1] + sdir[2]*sdir[2]);
    sdir[0] /= sun_len;
    sdir[1] /= sun_len;
    sdir[2] /= sun_len;
    // Dot product
    float diff = normal[0]*sdir[0] + normal[1]*sdir[1] + normal[2]*sdir[2];
    if (diff < 0) diff = 0;
    // Extract ambient
    unsigned int amb_r = (ambient_color >> 24) & 0xFF;
    unsigned int amb_g = (ambient_color >> 16) & 0xFF;
    unsigned int amb_b = (ambient_color >> 8) & 0xFF;
    // Extract sunlight
    unsigned int sun_r = (sun_color >> 24) & 0xFF;
    unsigned int sun_g = (sun_color >> 16) & 0xFF;
    unsigned int sun_b = (sun_color >> 8) & 0xFF;
    // Final color
    unsigned int r = amb_r + (unsigned int)((float)sun_r * diff);
    unsigned int g = amb_g + (unsigned int)((float)sun_g * diff);
    unsigned int b = amb_b + (unsigned int)((float)sun_b * diff);
    if (r > 255) r = 255;
    if (g > 255) g = 255;
    if (b > 255) b = 255;
    unsigned int color = (r << 24) | (g << 16) | (b << 8) | 0xFF;
    GX_Position3f32(pos[0], pos[1], pos[2]);
    GX_Normal3f32(normal[0], normal[1], normal[2]);
    GX_Color1u32(color);
    GX_TexCoord2f32(uv[0], uv[1]);
}
