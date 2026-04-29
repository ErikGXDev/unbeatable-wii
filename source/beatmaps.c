// Beatmap parser, soon in a github repo near you! - Erik

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "beatmaps.h"

// Extract a value from a line in the osu file.
//
// Example:
// line -> "AudioFilename: audio.mp3"
// key -> "AudioFilename:"
// returns "audio.mp3"
char *extract_value(const char *line, const char *key) {
    size_t key_len = strlen(key);

    if (strncmp(line, key, key_len) != 0) {
      return NULL;
    
    }
    const char *value = line + key_len;

    // Sort of "walk up to the value", trim spaces
    while (*value == ' ' || *value == ':') {
      value++;
    } 

    size_t len = strcspn(value, "\r\n");
    char *result = malloc(len + 1); // TODO: Check for malloc failure?

    strncpy(result, value, len);
    result[len] = 0;
    return result;
}

// example "/beatmaps/star.txt"
Beatmap *load_beatmap(const char *filename) {
  FILE *fp = fopen(filename, "r");

  if (!fp) {
    return NULL;
  }

  Beatmap *bm = malloc(sizeof(Beatmap));

  if (!bm) {
    fclose(fp);
    return NULL;
  }

  bm->artist = NULL;
  bm->title = NULL;
  bm->difficulty = NULL;
  bm->audio_filename = NULL;
  bm->cover_filename = strdup("cover.jpg"); // Dont want to search for this right now

  bm->hit_objects = NULL;
  bm->hit_object_count = 0;


  char last_section[64] = {0};

  char line[2048];
  while (fgets(line, sizeof(line), fp)) {
    
    if (line[0] == '[') {
      // New section
      sscanf(line, "[%63[^]]]", last_section);
      continue;
    }

    if (line[0] == '\n' || line[0] == '\r') {
      continue; // Skip empty lines
    }


    if (strcmp(last_section, "General") == 0) {
      char *val;
      if ((val = extract_value(line, "AudioFilename:"))) {
          bm->audio_filename = val;
          continue;
      }
    } else if (strcmp(last_section, "Metadata") == 0) {
      char *val;
      if ((val = extract_value(line, "Title:"))) {
          bm->title = val;
          continue;
      }
      if ((val = extract_value(line, "Artist:"))) {
          bm->artist = val;
          continue;
      }
      if ((val = extract_value(line, "Version:"))) {
          bm->difficulty = val;
          continue;
      }
    } else if (strcmp(last_section, "Difficulty") == 0) {
    } else if (strcmp(last_section, "HitObjects") == 0) {
      // Single note
      // 308,192,168837,1,0,0:0:0:100:

      // Hold note
      // 205,192,169130,128,0,169862.3529399582:0:0:0:100:

      int x, y, time, type;

      // Shoutout to whoever invented this function
      // never saw a higher level language with something like this lol
      int res = sscanf(line, "%d,%d,%d,%d", &x, &y, &time, &type);

      if (res != 4) {
        continue; // Skip weird lines
      }

      int end_time = time;
      if (type == 128) { // Hold note
        float end_time_f;
        int res2 = sscanf(line, "%*d,%*d,%*d,%*d,%*d,%f", &end_time_f);
        if (res2 != 1) {
          continue; // Skip weird lines
        }
        end_time = (int)end_time_f;
      }

      int lane = x * 6 / 512 + 1;

      HitObject new_hitobject = {
        /* .x = x,
        .y = y, */
        .lane = lane,
        .time = time,
        .end_time = end_time,
        .type = type
      };

      HitObject *temp = realloc(bm->hit_objects, sizeof(HitObject) * (bm->hit_object_count + 1));
      
      if (!temp) { // realloc failed
        free_beatmap(bm);
        fclose(fp);
        return NULL;
      }

      bm->hit_objects = temp;
      bm->hit_objects[bm->hit_object_count] = new_hitobject;
      bm->hit_object_count++;
    }

    
  }
  fclose(fp);

  return bm; // be a good boy and free this when you're done with it
}

void free_beatmap(Beatmap *bm) {
    if (!bm) return;
    free(bm->artist);
    free(bm->title);
    free(bm->difficulty);
    free(bm->audio_filename);
    free(bm->cover_filename);
    free(bm->hit_objects);
    free(bm);
}