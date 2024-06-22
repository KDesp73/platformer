#ifndef GAME_H
#define GAME_H

#include "entities.h"
#include <stdlib.h>
#include "textures.h"

#define BASE ceilf(SCREEN_WIDTH / 48.0f)
#define CELL_SIZE(scale) (BASE * scale)
#define ROWS(scale) ((int) (SCREEN_WIDTH / CELL_SIZE(scale)))
#define COLS(scale) ((int) (SCREEN_HEIGHT / CELL_SIZE(scale)))

typedef enum {
    GAME_STATUS_ACTIVE = -1,
    GAME_STATUS_GAME_OVER,
    GAME_STATUS_NEXT_LEVEL,
    GAME_STATUS_PREV_LEVEL,
    GAME_STATUS_PLAYER_DIED
} GameStatus;

typedef struct {
    size_t level;
    GameStatus status;
    Player player;
    GhostCollection ghosts;
    Textures textures;
} Game;

void run_game();

#endif // GAME_H
