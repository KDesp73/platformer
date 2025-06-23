#include "entities.h"
#include "config.h"
#include "raymath.h"
#include "utils.h"
#include "game.h"
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#define CLIB_IMPLEMENTATION
#include "extern/clib.h"
#include "raylib.h"

void draw_platform(Platform platform) 
{
    if (platform.sprite->width <= 0 || platform.sprite->height <= 0 || platform.sprite->width > SCREEN_WIDTH || platform.sprite->height > SCREEN_HEIGHT) {
        DrawLineEx(platform.start, (Vector2){platform.start.x + platform.length, platform.start.y}, platform.thickness, platform.color);
    } else {
        if (IsTextureReady(*platform.sprite)) {
            float scale = ceilf(platform.thickness / BASE);
            float cell_size = CELL_SIZE(scale);
            Rectangle src = {0, 0, platform.sprite->width, platform.sprite->height};
            for (float i = 0; i < (cell_size * platform.length / cell_size) - 1; i += cell_size) {
                Rectangle dest = {
                    platform.start.x + i, 
                    platform.start.y - platform.thickness / 2.0f, 
                    cell_size, 
                    platform.thickness
                };
                DrawTexturePro(*platform.sprite, src, dest, (Vector2){0, 0}, 0, WHITE);
            }
        }
    }
}

void draw_platforms(PlatformCollection collection) 
{
    for (size_t i = 0; i < collection.count; ++i) {
        if(collection.items[i] != NULL)
            draw_platform(*collection.items[i]);
    }
}

void free_platforms(PlatformCollection platforms) 
{
    if(platforms.items == NULL || platforms.count == 0) return;
    for (size_t i = 0; i < platforms.count; ++i) {
        if (platforms.items[i] != NULL) {
            clib_safe_free((void**) &platforms.items[i]);
            platforms.items[i] = NULL; // Optional: to avoid dangling pointers
        }
    }
    free(platforms.items);
}

Platform* make_platform(Vector2 start, float length, float thickness, Color color)
{
    Platform* platform = (Platform*) malloc(sizeof(Platform));
    
    if(!platform){
        ERRO("Could not allocate memory for platform!");
        return NULL;
    }

    if(length > 0){
        platform->start = start;
        platform->length = length;
    } else {
        platform->start = (Vector2){start.x + length, start.y};
        platform->length = -length;
    }
    platform->thickness = thickness;
    platform->color = color;

    return platform;
}

PlatformCollection allocate_platform_collection(size_t capacity)
{
    PlatformCollection result = {.capacity = capacity};
    result.items = (Platform**) malloc(sizeof(Platform) * capacity);
    
    return result;
}

PlatformCollection make_platform_collection(Platform* first, ...) 
{
    PlatformCollection result = {0};
    if (first == NULL) {
        ERRO("No platforms added");
        return result;
    }

    result.count = 1;
    va_list args;
    va_start(args, first);
    for (Platform* next = va_arg(args, Platform*); next != NULL; next = va_arg(args, Platform*)) {
        result.count += 1;
    }
    va_end(args);
    result.capacity = result.count;

    result.items = (Platform**) malloc(sizeof(Platform*) * result.count);
    if (result.items == NULL) {
        PANIC("Could not allocate memory: %s", strerror(errno));
    }

    result.count = 0;

    result.items[result.count++] = first;

    va_start(args, first);
    for (Platform* next = va_arg(args, Platform*); next != NULL; next = va_arg(args, Platform*)) {
        result.items[result.count++] = next;
    }
    va_end(args);

    return result;
}

void add_platform(Platform* platform, PlatformCollection* collection)
{
    assert(platform != NULL);
    assert(collection->items != NULL);

    if(collection->count >= collection->capacity) {
        ERRO("Max capacity");
        return;
    }

    collection->items[collection->count++] = platform;
}

void print_player_position(Player player, int x, int y)
{
    DrawText(TextFormat("Player: (%.1f,  %.1f)", player.position.x, player.position.y), x, y, 20, WHITE);
}

void print_platform_position(Platform platform, int x, int y)
{
    DrawText(TextFormat("Platform: start(%.1f,  %.1f) end(%.1f,  %.1f)", platform.start.x, platform.start.y, platform.start.x + platform.length, platform.start.y), x, y, 20, WHITE);
}

void print_platforms_positions(PlatformCollection platforms, int x, int y)
{
    for(size_t i = 0; i < platforms.count; ++i){
        print_platform_position(*platforms.items[i], x, y + (30*i));
    }
}
