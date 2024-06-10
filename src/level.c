#include "level.h"
#include "clib.h"
#include "entities.h"
#include "game.h"
#include "raylib.h"
#include "textures.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void draw_level(Level level, Player player, Textures textures){
    assert(level.platforms.items != NULL);
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

const char** split(const char* str, char delimiter, int* count) {
    assert(str != NULL);
    *count = 0;

    // Create a copy of the input string to tokenize
    char* str_copy = strdup(str);
    if (str_copy == NULL) {
        perror("Failed to duplicate input string");
        exit(EXIT_FAILURE);
    }

    // Count the number of tokens
    char* temp = str_copy;
    while (*temp) {
        if (*temp == delimiter) {
            (*count)++;
        }
        temp++;
    }
    (*count)++;

    // Allocate memory for the array of strings
    const char** result = (const char**) malloc((*count) * sizeof(char*));
    if (result == NULL) {
        perror("Failed to allocate memory for result");
        free(str_copy);
        exit(EXIT_FAILURE);
    }

    // Tokenize the string
    int index = 0;
    char* token = strtok(str_copy, &delimiter);
    while (token != NULL) {
        result[index] = strdup(token);
        if (result[index] == NULL) {
            perror("Failed to duplicate token");
            for (int i = 0; i < index; i++) {
                free((char*)result[i]);
            }
            free(result);
            free(str_copy);
            exit(EXIT_FAILURE);
        }
        index++;
        token = strtok(NULL, &delimiter);
    }

    free(str_copy);
    return result;
}

Level* load_level(Cstr path, Textures textures) {
    Cstr contents = clib_read_file(path);
    if (contents == NULL) {
        PANIC("Failed to read file: %s\n", path);
    }

    int count = 0;
    const char** lines = split(contents, '\n', &count);
    if (lines == NULL) {
        PANIC("Failed to split file contents\n");
    }

    // Count platforms
    Level* level = (Level*) malloc(sizeof(Level));
    if (level == NULL) {
        PANIC("Failed to allocate memory for level\n");
    }

    level->platforms.count = 0;
    count--;
    DEBU("count: %d", count);
    for (size_t i = 0; i < count; ++i) {
        if (lines[i] == NULL || lines[i][0] == '\0') continue;
        DEBU(lines[i]);

        int words = 0;
        const char** parts = split(lines[i], ' ', &words);
        if (parts == NULL) continue;
        if (strcmp(parts[0], "platform") == 0) level->platforms.count++;

        // Free parts after use
        for (int j = 0; j < words; j++) {
            free((char*)parts[j]);
        }
        free(parts);
    }

    level->platforms = allocate_platform_collection(level->platforms.count);
    level->platforms.count = 0;

    for (size_t i = 0; i < count; ++i) {
        if (lines[i] == NULL) continue;

        int words = 0;
        const char** parts = split(lines[i], ' ', &words);
        if (parts == NULL) {
            ERRO("Couldn't split line %zu\n", i + 1);
            continue;
        }
        if (words == 0) continue;

        if (strcmp(parts[0], "creator") == 0) {
            if (words == 1 || words > 2) PANIC("Invalid number of values in line %zu\n", i + 1);
            level->creator = parts[1];
        } else if (strcmp(parts[0], "player") == 0) {
            if (words == 1 || words > 3) PANIC("Invalid number of values in line %zu\n", i + 1);
            level->player.position = (Vector2){atof(parts[1]), atof(parts[2])};
        } else if (strcmp(parts[0], "door") == 0) {
            if (words == 1 || words > 3) PANIC("Invalid number of values in line %zu\n", i + 1);
            level->door.position = (Vector2){atof(parts[1]), atof(parts[2])};
        } else if (strcmp(parts[0], "platform") == 0) {
            if (words == 1 || words > 4) PANIC("Invalid number of values in line %zu\n", i + 1);
            Vector2 start = {atof(parts[1]), atof(parts[2])};
            add_platform(make_platform(start, atof(parts[3]), PLATFORM_HEIGHT, WHITE), &level->platforms);
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
    for (int i = 0; i < count; i++) {
        free((char*)lines[i]);
    }
    free(lines);

    level->player.size = PLAYER_SIZE;
    level->player.color = PLAYER_COLOR;
    level->player.sprite = &textures.items[PLAYER];
    level->player.status = IDLE;

    level->door.size = DOOR_SIZE;
    level->door.color = DOOR_COLOR;
    level->door.sprite = &textures.items[DOOR];

    level->textures = textures;

    DEBU("creator: %s\n", level->creator);
    DEBU("platform count: %zu\n", level->platforms.count);
    DEBU("player position: (%.0f, %.0f)\n", level->player.position.x, level->player.position.y);
    DEBU("door position: (%.0f, %.0f)\n", level->door.position.x, level->door.position.y);

    return level;
}
