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
    GHOST,
    KEY,
    BACKGROUND,

    TEXTURES_COUNT
} TextureId;

typedef struct {
    Texture2D* items;
    size_t count; 
} Textures;

#define PRINT_TEXTURE(tag, textures) \
    printf("[%s] width: %d, height: %d, id: %d, mipmaps: %d, format: %d\n", #tag, textures.items[tag].width, textures.items[tag].height, textures.items[tag].id, textures.items[tag].mipmaps, textures.items[tag].format)

Textures load_textures(Cstr first, ...);
void unload_textures(Textures textures);

#endif // TEXTURES_H
