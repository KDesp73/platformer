#ifndef LEVEL_H
#define LEVEL_H

#include "entities.h"
#include "textures.h"
typedef struct {
    size_t which;
    Player player;
    PlatformCollection platforms;
    Door door;
} Level;

void draw_level(Level level, Textures textures);

#endif // LEVEL_H
