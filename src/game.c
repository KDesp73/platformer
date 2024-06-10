#include "game.h"
#include "textures.h"

void init_textures(Game* game){
    game->textures = load_textures(
        "assets/jess-30x50.png",
        "assets/door-50x80.png",
        "assets/wood-25x25.png",
        NULL // Teriminate the list
    );
}
