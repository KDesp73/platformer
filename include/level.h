#ifndef LEVEL_H
#define LEVEL_H

#include "entities.h"
#include "textures.h"
typedef struct {
    Cstr creator;
    Player player;
    PlatformCollection platforms;
    GhostCollection ghosts;
    Door door;
    Textures textures;
    float scale; // based on 25.0f
} Level;

typedef struct {
    Level** items;
    size_t count;
    size_t capacity;
} Levels;

void draw_level(Level level, Player player);
Level* make_level(Vector2 player_position, PlatformCollection platforms, GhostCollection ghosts, Vector2 door_position, Textures textures, float scale);
Levels make_levels(Level* first, ...);
Levels allocate_levels(size_t capacity);
void add_level(Levels* levels, Level* level);
Level* load_level(Cstr text, Textures textures);
Level* load_level_from_file(Cstr path, Textures textures);

Levels load_levels_from_dir(Cstr path, Textures textures);
Levels load_levels_from_table(const char** table, Textures textures);


#endif // LEVEL_H
