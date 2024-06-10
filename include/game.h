#ifndef GAME_H
#define GAME_H

#include "entities.h"
#include <stdlib.h>
#include "textures.h"

#define PLAYER_SIZE (Vector2) {30.0f, 50.0f}
#define PLAYER_COLOR RED

// Should be { 45, 75 }
#define DOOR_SIZE (Vector2) {50.0f, 80.0f}
#define DOOR_COLOR GREEN

#define PLATFORM_HEIGHT 25

typedef struct {
    size_t level;
    int is_over;
    int is_level_complete;
    Player player;
    Textures textures;
} Game;


#endif // GAME_H
