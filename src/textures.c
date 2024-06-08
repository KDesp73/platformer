#include "textures.h"
#include "raylib.h"

Textures make_textures(Cstr first, ...){
    Textures result = {0};
    if (first == NULL) {
        ERRO("No platforms added");
        return result;
    }

    result.count = 1;
    va_list args;
    va_start(args, first);
    for (Cstr* next = va_arg(args, Cstr*); next != NULL; next = va_arg(args, Cstr*)) {
        result.count += 1;
    }
    va_end(args);

    result.items = (Texture2D*) malloc(sizeof(Texture2D*) * result.count);
    if (result.items == NULL) {
        PANIC("Could not allocate memory: %s", strerror(errno));
    }

    result.count = 0;

    result.items[result.count++] = LoadTexture(first);

    va_start(args, first);
    for (Cstr next = va_arg(args, Cstr); next != NULL; next = va_arg(args, Cstr)) {
        result.items[result.count++] = LoadTexture(next);
    }
    va_end(args);

    return result;
}

void unload_textures(Textures textures){
    for(size_t i = 0; i < textures.count; ++i){
        UnloadTexture(textures.items[i]);
    }
}
