#include "entities.h"
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#define CLIB_IMPLEMENTATION
#include "clib.h"
#include "raylib.h"

void draw_door(Door door){
    if(door.sprite->width <= 0 || door.sprite->height <= 0){
        DrawRectangleV(door.position, (Vector2) {door.size.x, door.size.y}, door.color);
    } else {
        if(IsTextureReady(*door.sprite)){
            DrawTextureV(*door.sprite, door.position, WHITE);
        }  
    }
}

void draw_player(Player player) {
    if(player.sprite->width <= 0 || player.sprite->height <= 0){
        DrawRectangleV(player.position, (Vector2) {player.size.x, player.size.y}, player.color);
    } else {
        if(IsTextureReady(*player.sprite)){
            DrawTextureV(*player.sprite, player.position, WHITE);
        } 
    }
}

void draw_platform(Platform platform, Texture2D sprite){
    if (sprite.width <= 0 || sprite.height <= 0){
        DrawLineEx(platform.start, (Vector2) {platform.start.x + platform.length, platform.start.y}, platform.thickness, platform.color);
    } else {
        if(IsTextureReady(sprite)){
            for(size_t i = 0; i < platform.length / sprite.width; ++i){
                DrawTextureV(sprite, (Vector2) {platform.start.x + (i * sprite.width), platform.start.y - platform.thickness / 2.0f}, WHITE);
            }
        }
    }
}

void draw_platforms(PlatformCollection collection, Texture2D sprite) {
    for (size_t i = 0; i < collection.count; ++i) {
        if(collection.items[i] != NULL)
            draw_platform(*collection.items[i], sprite);
    }
}

void free_platforms(PlatformCollection platforms){
    for(size_t i = 0; i < platforms.count; ++i){
        free(platforms.items[i]);
    }
}


Platform* make_platform(Vector2 start, float length, float thickness, Color color){
    Platform* platform = (Platform*) malloc(sizeof(Platform));
    
    if(!platform){
        ERRO("Could not allocate memory for platform!");
        return NULL;
    }

    platform->start = start;
    platform->length = length;
    platform->thickness = thickness;
    platform->color = color;

    return platform;
}

PlatformCollection make_platform_collection_empty(size_t capacity){
    PlatformCollection result = {.capacity = capacity};
    result.items = (Platform**) malloc(sizeof(Platform) * capacity);
    
    return result;
}

PlatformCollection make_platform_collection(Platform* first, ...) {
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

void add_platform(Platform* platform, PlatformCollection* collection){
    assert(platform != NULL);
    assert(collection->items != NULL);

    if(collection->count >= collection->capacity) {
        PANIC("Max capacity");
    }

    collection->items[collection->count++] = platform;
}

void print_player_position(Player player, int x, int y){
    DrawText(TextFormat("Player: (%.1f,  %.1f)", player.position.x, player.position.y), x, y, 20, WHITE);
}
void print_platform_position(Platform platform, int x, int y){
    DrawText(TextFormat("Platform: start(%.1f,  %.1f) end(%.1f,  %.1f)", platform.start.x, platform.start.y, platform.start.x + platform.length, platform.start.y), x, y, 20, WHITE);
}
void print_platforms_positions(PlatformCollection platforms, int x, int y){
    for(size_t i = 0; i < platforms.count; ++i){
        print_platform_position(*platforms.items[i], x, y + (30*i));
    }
}
