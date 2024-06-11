#include "textures.h"
#include "clib.h"
#include "config.h"
#include "game.h"
#include "raylib.h"
#include <stdarg.h>
#include <stdio.h>


Textures load_textures(Cstr first, ...) {
    Textures result = {0};

    result.items = (Texture2D*) malloc(sizeof(Texture2D) * TEXTURES_COUNT);
    if (result.items == NULL) {
        PANIC("Could not allocate memory: %s\n", strerror(errno));
    }

    result.count = 0;

    va_list args;
    va_start(args, first);
    for (Cstr current = first; current != NULL; current = va_arg(args, Cstr)) {
        if(current != NULL){
            result.items[result.count++] = LoadTexture(current);
        } else
            result.items[result.count++] = (Texture2D){0};

        if (result.count >= TEXTURES_COUNT) {
            break; // Prevent overflow
        }
    }
    va_end(args);

    return result;
}

void unload_textures(Textures textures) {
    for (size_t i = 0; i < textures.count; ++i) {
        UnloadTexture(textures.items[i]);
    }
    free(textures.items);
}


