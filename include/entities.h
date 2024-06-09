#ifndef ENTITIES_H
#define ENTITIES_H

#include <stdlib.h>
#include "raylib.h"
#include "raymath.h"

typedef enum {
    IDLE,
    RUNNING_LEFT,
    RUNNING_RIGHT,
    JUMPING
} PlayerStatus;

typedef struct {
    Vector2 position;
    Vector2 velocity;
    Vector2 size;
    Color color;
    Texture2D* sprite;
    int is_grounded;
    int status;
} Player;

typedef struct {
    Vector2 start;
    float length;
    float thickness;
    Color color;
    Texture2D* sprite;
} Platform;

typedef struct {
    Vector2 position;
    Vector2 size;
    Color color;
    Texture2D* sprite;
} Door;

typedef struct {
    Platform** items;
    size_t count;
    size_t capacity;
} PlatformCollection;

Platform* make_platform(Vector2 start, float length, float thickness, Color color);
PlatformCollection make_platform_collection(Platform* first, ...);
PlatformCollection make_platform_collection_empty(size_t capacity);
void add_platform(Platform* platform, PlatformCollection* collection);
void free_platforms(PlatformCollection platforms);

void draw_player(Player player);
void draw_platform(Platform platform, Texture2D sprite);
void draw_platforms(PlatformCollection collection, Texture2D sprite);
void draw_door(Door door);
void place_door_on_platform(Door* door, Platform platform);

void print_player_position(Player player, int x, int y);
void print_platform_position(Platform platform, int x, int y);
void print_platforms_positions(PlatformCollection platforms, int x, int y);

#endif // ENTITIES_H
