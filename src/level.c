#include "level.h"
#include "clib.h"
#include "entities.h"
#include "game.h"
#include "raylib.h"
#include "textures.h"
#include <assert.h>
#include <stdlib.h>

void draw_level(Level level, Player player, Textures textures){
    draw_door(level.door);
    draw_player(player);
    draw_platforms(level.platforms, textures.items[PLATFORM]);
}

void add_level(Levels* levels, Level* level){
    assert(levels->items != NULL);
    assert(level!= NULL);

    if(levels->count >= levels->capacity) {
        PANIC("Max capacity");
    }

    levels->items[levels->count++] = level;
}

Level* make_level(Vector2 player_position, PlatformCollection platforms, Vector2 door_position, Textures textures){
    Level* level = (Level*) malloc(sizeof(Level)); 

    if(level == NULL) {
        ERRO("Failed to allocate level");
        return NULL;
    }

    level->player = (Player) {
        .position = player_position,
        .size = PLAYER_SIZE,
        .color = PLAYER_COLOR,
        .sprite = &textures.items[PLAYER],
        .status = IDLE
    };

    level->door = (Door) {
        .position = door_position,
        .size = DOOR_SIZE,
        .color = DOOR_COLOR,
        .sprite = &textures.items[DOOR]
    };

    level->platforms = platforms;
    level->textures = textures;

    return level;
}

Levels allocate_levels(size_t capacity){
    Levels result = { .capacity = capacity };
    result.items = (Level**) malloc(sizeof(Level) * capacity);
    
    return result;
}

Levels make_levels(Level* first, ...) {
    Levels result = {0};
    if (first == NULL) {
        ERRO("No platforms added");
        return result;
    }

    result.count = 1;
    va_list args;
    va_start(args, first);
    for (Level* next = va_arg(args, Level*); next != NULL; next = va_arg(args, Level*)) {
        result.count += 1;
    }
    va_end(args);
    result.capacity = result.count;

    result.items = (Level**) malloc(sizeof(Level*) * result.count);
    if (result.items == NULL) {
        PANIC("Could not allocate memory: %s", strerror(errno));
    }

    result.count = 0;

    result.items[result.count++] = first;

    va_start(args, first);
    for (Level* next = va_arg(args, Level*); next != NULL; next = va_arg(args, Level*)) {
        result.items[result.count++] = next;
    }
    va_end(args);

    return result;
}
