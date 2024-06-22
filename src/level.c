#include "level.h"
#include "clib.h"
#include "config.h"
#include "entities.h"
#include "game.h"
#include "physics.h"
#include "raylib.h"
#include "raymath.h"
#include "textures.h"
#include "ui.h"
#include "utils.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

void draw_level(Level level, Player player, Textures textures){
    assert(level.platforms.items != NULL);
    draw_door(level.door);
    draw_player(player);
    draw_platforms(level.platforms, textures.items[PLATFORM]);
    draw_ghosts(level.ghosts);
}

void add_level(Levels* levels, Level* level){
    assert(levels->items != NULL);
    assert(level!= NULL);

    if(levels->count >= levels->capacity) {
        PANIC("Max capacity");
    }

    levels->items[levels->count++] = level;
}

Level* make_level(Vector2 player_position, PlatformCollection platforms, GhostCollection ghosts, Vector2 door_position, Textures textures, float scale){
    Level* level = (Level*) malloc(sizeof(Level)); 

    if(level == NULL) {
        ERRO("Failed to allocate level");
        return NULL;
    }

    level->player = (Player) {
        .position = player_position,
        .size = PLAYER_SIZE(scale),
        .color = PLAYER_COLOR,
        .sprite = &textures.items[PLAYER],
        .status = PLAYER_STATUS_IDLE
    };

    level->door = (Door) {
        .position = door_position,
        .size = DOOR_SIZE(scale),
        .color = DOOR_COLOR,
        .sprite = &textures.items[DOOR]
    };

    level->platforms = platforms;
    level->ghosts = ghosts;
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

// const char** split(const char* str, char delimiter, int* count);
int split (const char *str, char c, char ***arr){
    int count = 1;
    int token_len = 1;
    int i = 0;
    char *p;
    char *t;

    p = str;
    while (*p != '\0')
    {
        if (*p == c)
            count++;
        p++;
    }

    *arr = (char**) malloc(sizeof(char*) * count);
    if (*arr == NULL)
        exit(1);

    p = str;
    while (*p != '\0')
    {
        if (*p == c)
        {
            (*arr)[i] = (char*) malloc( sizeof(char) * token_len );
            if ((*arr)[i] == NULL)
                exit(1);

            token_len = 0;
            i++;
        }
        p++;
        token_len++;
    }
    (*arr)[i] = (char*) malloc( sizeof(char) * token_len );
    if ((*arr)[i] == NULL)
        exit(1);

    i = 0;
    p = str;
    t = ((*arr)[i]);
    while (*p != '\0')
    {
        if (*p != c && *p != '\0')
        {
            *t = *p;
            t++;
        }
        else
        {
            *t = '\0';
            i++;
            t = ((*arr)[i]);
        }
        p++;
    }

    return count;
}

Level* load_level_from_file(Cstr path, Textures textures){
    Cstr contents = clib_read_file(path);
    if (contents == NULL) {
        PANIC("Failed to read file: %s\n", path);
    }

    return load_level(contents, textures);
}

Level* load_level(Cstr text, Textures textures) {
    char** lines;
    int lines_count = split(text, '\n', &lines) - 1;
    if (lines == NULL) {
        PANIC("Failed to split file contents\n");
    }

    // Allocate memory for Level
    Level* level = (Level*) malloc(sizeof(Level));
    if (level == NULL) {
        PANIC("Failed to allocate memory for level\n");
    }

    // First pass: Count ghosts
    level->ghosts.count = 0;
    for (size_t i = 0; i < lines_count; ++i) {
        if(
            lines[i] == NULL || 
            lines[i][0] == EOF || 
            lines[i][0] == '\0' || 
            lines[i][0] == '\n'
        ) continue;

        char** parts;
        int words = split(lines[i], ' ', &parts);
        if (parts == NULL) continue;
        if (strcmp(parts[0], "ghost") == 0) level->ghosts.count++;

        for (int j = 0; j < words; j++) {
            free((char*)parts[j]);
        }
        free(parts);
    }

    DEBU("ghost.count: %zu", level->ghosts.count);
    if(level->ghosts.count > 0)
        level->ghosts = allocate_ghost_collection(level->ghosts.count); 
    level->ghosts.count = 0;
    
    // First pass: Count platforms
    level->platforms.count = 0;
    for (size_t i = 0; i < lines_count; ++i) {
        if(
            lines[i] == NULL || 
            lines[i][0] == EOF || 
            lines[i][0] == '\0' || 
            lines[i][0] == '\n'
        ) continue;

        char** parts;
        int words = split(lines[i], ' ', &parts);
        if (parts == NULL) continue;
        if (strcmp(parts[0], "platform") == 0) level->platforms.count++;

        for (int j = 0; j < words; j++) {
            free((char*)parts[j]);
        }
        free(parts);
    }

    DEBU("platform.count: %zu", level->platforms.count);
    level->platforms = allocate_platform_collection(level->platforms.count + 1); // +1 for base platform
    level->platforms.count = 0;

    // Second pass: Process each line
    for (size_t i = 0; i < lines_count; ++i) {
        if (lines[i] == NULL) continue;

        char** parts;
        int words = split(lines[i], ' ', &parts);
        if (parts == NULL) {
            ERRO("Couldn't split line %zu\n", i + 1);
            continue;
        }
        if (words == 0) {
            free(parts);
            continue;
        }

        if (strcmp(parts[0], "creator") == 0) {
            if (words != 2) PANIC("Invalid number of values in line %zu", i + 1);
            level->creator = strdup(parts[1]);
        } else if (strcmp(parts[0], "scale") == 0) {
            if (words != 2) PANIC("Invalid number of values in line %zu", i + 1);
            level->scale = atof(parts[1]);
        } else if (strcmp(parts[0], "player") == 0) {
            if (words != 3) PANIC("Invalid number of values in line %zu", i + 1);
            level->player.position = (Vector2){ atof(parts[1]), atof(parts[2]) };
        } else if (strcmp(parts[0], "door") == 0) {
            if (words != 3) PANIC("Invalid number of values in line %zu", i + 1);
            level->door.position = (Vector2){ atof(parts[1]), atof(parts[2]) };
        } else if (strcmp(parts[0], "ghost") == 0) {
            if(words != 3) PANIC("Invalig number of values in line %zu", i + 1);
            Vector2 pos  = { atof(parts[1]), atof(parts[2]) };
            pos = Vector2Scale(pos, CELL_SIZE(level->scale));
            if(level->ghosts.items != NULL)
                add_ghost(make_ghost(pos, level->scale), &level->ghosts);
        } else if (strcmp(parts[0], "platform") == 0) {
            if (words != 4) PANIC("Invalid number of values in line %zu", i + 1);
            Vector2 start = { atof(parts[1]), atof(parts[2]) };
            start = Vector2Scale(start, CELL_SIZE(level->scale));
            add_platform(make_platform((Vector2) {start.x, start.y + CELL_SIZE(level->scale) / 2}, atof(parts[3]) * CELL_SIZE(level->scale), PLATFORM_HEIGHT(level->scale), WHITE), &level->platforms);
        } else {
            PANIC("Invalid key '%s' in line %zu\n", parts[0], i + 1);
        }

        // Free parts after use
        for (int j = 0; j < words; j++) {
            free((char*)parts[j]);
        }
        free(parts);
    }

    // Free lines after use
    for (int i = 0; i < lines_count; i++) {
        free((char*)lines[i]);
    }
    free(lines);

    level->player.position = Vector2Scale(level->player.position, CELL_SIZE(level->scale));
    level->player.size = Vector2Scale(BASE_PLAYER_SIZE, (level->scale));
    level->player.color = PLAYER_COLOR;
    if(textures.items != NULL)
        level->player.sprite = &textures.items[PLAYER];
    level->player.status = PLAYER_STATUS_IDLE;

    level->door.position = Vector2Scale(level->door.position, CELL_SIZE(level->scale));
    level->door.size = Vector2Scale(BASE_DOOR_SIZE, (level->scale));
    level->door.color = DOOR_COLOR;
    if(textures.items != NULL)
        level->door.sprite = &textures.items[DOOR];

    if(textures.items != NULL)
        level->textures = textures;

    for(size_t i = 0; i < level->ghosts.count; ++i){
        level->ghosts.items[i]->sprite = &textures.items[GHOST];
    }

    INFO("level scale: %.1f", level->scale);

    return level;
}

Cstr* get_files(Cstr dir, int* count){
    struct dirent *de;

    *count = 0;
    DIR *dr = opendir(dir);

    if (dr == NULL) {
        ERRO("Could not open current directory" );
        return NULL;
    }

    size_t total_size = 0;
    while ((de = readdir(dr)) != NULL){
        total_size += sizeof(char) * strlen(de->d_name);
    }

    Cstr* items = (Cstr*) malloc(total_size);  

    dr = opendir(dir);
    while ((de = readdir(dr)) != NULL) {
        if(strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") != 0){
            items[(*count)++] = de->d_name;
        }
    }
    closedir(dr);  

    return items;
}

Levels load_levels_from_dir(Cstr path, Textures textures){
    int count;
    
    Cstr* files = get_files(path, &count);
    
    if(files == NULL || count <= 0) {
        ERRO("Could not get files");
        return (Levels){.items = NULL};
    } 

    Levels levels = allocate_levels(count);
    for(size_t i = 0; i < count; ++i){
        add_level(&levels, load_level_from_file(PATH(path, files[i]), textures));
        INFO("Loaded %s", PATH(path, files[i]));
    }
    
    return levels;
}

void run_level(Level level, Game* game){
    // Logic
    update_player(&game->player, SCREEN_WIDTH, SCREEN_HEIGHT, level.scale);
    check_and_resolve_platform_collisions(&game->player, level.platforms);

    if(game->player.position.y + game->player.size.y == SCREEN_HEIGHT){
        // TODO: fall to previous level
    }

    if(check_door_collision(&game->player, level.door)){
        DrawText("^", level.door.position.x + level.door.size.x / 2 - MeasureText("^", 70) / 2.0f, level.door.position.y - 70/2.0f - 10, 70, WHITE);
        if(IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)){
            game->is_level_complete = true;
        }
    }

    // CHEAT
    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) game->player.position = GetMousePosition();
    if(IsKeyPressed(KEY_N)) game->is_level_complete = true;
    

    // Draw
    ClearBackground(GetColor(0x181818FF));
    draw_level(level, game->player, level.textures);
    DrawText(TextFormat("Level %zu", game->level+1), 20, 20, 30, WHITE);
}
