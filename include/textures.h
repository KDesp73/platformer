#ifndef TEXTURES_H
#define TEXTURES_H

#include <stdlib.h>
#define CLIB_IMPLEMENTATION 
#include "clib.h"
#include "raylib.h"

typedef enum {
    PLAYER,
    DOOR,
    PLATFORM,
    BACKGROUND,
    TEXTURES_COUNT
} TextureId;

typedef struct {
    Texture2D* items;
    size_t count; 
} Textures;

Textures make_textures(Cstr first, ...);
void load_textures(Textures textures);
void unload_textures(Textures textures);

#endif // TEXTURES_H
