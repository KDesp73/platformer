#ifndef GAME_H
#define GAME_H

#include "entities.h"
#include <stdlib.h>
#include "textures.h"

#define BASE ceilf(SCREEN_WIDTH / 50.0f)
#define CELL_SIZE(scale) (BASE * scale)
#define ROWS(scale) ((int) (SCREEN_WIDTH / CELL_SIZE(scale)))
#define COLS(scale) ((int) (SCREEN_HEIGHT / CELL_SIZE(scale)))

#define BASE_PLAYER_SIZE (Vector2) {BASE, 2 * BASE}
#define PLAYER_SIZE(scale) (Vector2) {CELL_SIZE(scale), 2 * CELL_SIZE(scale)}
#define PLAYER_COLOR RED

#define BASE_DOOR_SIZE (Vector2) {2 * BASE, 3 * BASE}
#define DOOR_SIZE(scale) (Vector2) {2 * CELL_SIZE(scale), 3 * CELL_SIZE(scale)}
#define DOOR_COLOR GREEN

#define BASE_PLATFORM_HEIGHT BASE
#define PLATFORM_HEIGHT(scale) CELL_SIZE(scale)
#define PLATFORM_COLOR WHITE

typedef struct {
    size_t level;
    int is_over;
    int is_level_complete;
    Player player;
    Textures textures;
} Game;

void run_game();

#endif // GAME_H
