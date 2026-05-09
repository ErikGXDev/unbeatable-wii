
void DrawSunlitVertex(const float *pos, const float *normal, const float *uv, const float *sun_dir, unsigned int sun_color, unsigned int ambient_color);

void DrawUnlitVertex(const float *pos, const float *uv, unsigned int color);

void DrawSquare(const float *pos, GRRLIB_texImg *tex, unsigned int color, float scaleX, float scaleY);