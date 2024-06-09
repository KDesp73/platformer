#include "level.h"
#include "textures.h"

void draw_level(Level level, Textures textures){
    draw_door(level.door);
    draw_player(level.player);
    draw_platforms(level.platforms, textures.items[PLATFORM]);
}
