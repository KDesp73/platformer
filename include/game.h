#ifndef GAME_H
#define GAME_H

#include "entities.h"
#include <stdlib.h>
#include "textures.h"

#define BASE ceilf(SCREEN_WIDTH / 48.0f)
#define CELL_SIZE(scale) (BASE * scale)
#define ROWS(scale) ((int) (SCREEN_WIDTH / CELL_SIZE(scale)))
#define COLS(scale) ((int) (SCREEN_HEIGHT / CELL_SIZE(scale)))


typedef struct {
    size_t level;
    int is_over;
    int is_level_complete;
    Player player;
    Textures textures;
} Game;

void run_game();

#endif // GAME_H
