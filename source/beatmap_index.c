
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <grrlib.h>
#include "beatmaps.h"

Beatmap *beatmap_index[128]; // 128 hard limit is big enough
int beatmap_count = 0;

void free_beatmap_index()
{
    for (int i = 0; i < 128; i++)
    {
        if (beatmap_index[i])
        {
            free_beatmap(beatmap_index[i]);
            beatmap_index[i] = NULL;
        }
    }
    beatmap_count = 0;
}

void load_beatmap_index_recursive(const char *dir_path, int *index)
{
    DIR *d = opendir(dir_path);
    if (!d)
    {
        return;
    }

    struct dirent *dir;
    while ((dir = readdir(d)) != NULL)
    {
        if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0)
        {
            continue;
        }

        if (dir->d_type == DT_DIR)
        { // Directory
            char sub_path[512];
            // subdirectory
            snprintf(sub_path, sizeof(sub_path), "%s/%s", dir_path, dir->d_name);
            load_beatmap_index_recursive(sub_path, index);
        }
        else if (dir->d_type == DT_REG)
        { // Regular file
            const char *filename = dir->d_name;
            size_t len = strlen(filename);
            if (len > 4 && strcmp(filename + len - 4, ".txt") == 0)
            {
                if (*index < 128)
                {
                    char path[512];
                    snprintf(path, sizeof(path), "%s/%s", dir_path, filename);
                    Beatmap *bm = load_beatmap(path);
                    if (bm)
                    {
                        beatmap_index[(*index)++] = bm;
                    }
                }
                else
                {
                    break; // Reached max beatmap limit
                }
            }
        }
    }
    closedir(d);
}

int load_beatmap_index()
{
    int index = 0;
    load_beatmap_index_recursive("/beatmaps", &index);
    beatmap_count = index;
    return index;
}

GRRLIB_texImg *get_beatmap_cover(int index)
{
    if (index < 0 || index >= beatmap_count)
    {
        return NULL;
    }

    GRRLIB_texImg *tex = GRRLIB_LoadTextureFromFile(beatmap_index[index]->cover_filename);

    if (tex && ((tex->w % 4) != 0 || (tex->h % 4) != 0))
    {
        int new_w = (tex->w + 3) & ~3; // Round up to nearest multiple of 4
        int new_h = (tex->h + 3) & ~3;

        GRRLIB_texImg *padded = GRRLIB_CreateEmptyTexture(new_w, new_h);
        if (padded)
        {
            // Copy the original image into the top-left of the padded texture
            for (int y = 0; y < tex->h; y++)
            {
                for (int x = 0; x < tex->w; x++)
                {
                    u32 col = GRRLIB_GetPixelFromtexImg(x, y, tex);
                    GRRLIB_SetPixelTotexImg(x, y, padded, col);
                }
            }
            GRRLIB_FlushTex(padded);
            GRRLIB_FreeTexture(tex);
            return padded;
        }
    }

    return tex;
}