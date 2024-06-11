#ifndef GAME_H
#define GAME_H

#include "entities.h"
#include <stdlib.h>
#include "textures.h"

// #define CELL_SIZE (SCREEN_WIDTH / 50.0f)
#define CELL_SIZE 50.0f

#define BASE 25.0f
#define SCALE CELL_SIZE/BASE

#define BASE_PLAYER_SIZE (Vector2) {BASE, 2 * BASE}
#define PLAYER_SIZE (Vector2) {CELL_SIZE, 2 * CELL_SIZE}
#define PLAYER_COLOR RED

#define BASE_DOOR_SIZE (Vector2) {2 * BASE, 3 * BASE}
#define DOOR_SIZE (Vector2) {2 * CELL_SIZE, 3 * CELL_SIZE}
#define DOOR_COLOR GREEN

#define DEFAULT_PLATFORM_HEIGHT BASE
#define PLATFORM_HEIGHT CELL_SIZE

typedef struct {
    size_t level;
    int is_over;
    int is_level_complete;
    Player player;
    Textures textures;
} Game;


#endif // GAME_H
