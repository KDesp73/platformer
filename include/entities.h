#ifndef ENTITIES_H
#define ENTITIES_H

#include <stdlib.h>
#include "raylib.h"
#include "raymath.h"

#define BASE_PLAYER_SIZE (Vector2) {BASE, 2 * BASE}
#define PLAYER_SIZE(scale) (Vector2) {CELL_SIZE(scale), 2 * CELL_SIZE(scale)}
#define PLAYER_COLOR RED

#define BASE_DOOR_SIZE (Vector2) {2 * BASE, 3 * BASE}
#define DOOR_SIZE(scale) (Vector2) {2 * CELL_SIZE(scale), 3 * CELL_SIZE(scale)}
#define DOOR_COLOR GREEN

#define BASE_PLATFORM_HEIGHT BASE
#define PLATFORM_HEIGHT(scale) CELL_SIZE(scale)
#define PLATFORM_COLOR WHITE

#define GHOST_COLOR ORANGE
#define GHOST_SIZE(scale) (Vector2) {2 * CELL_SIZE(scale), 2 * CELL_SIZE(scale)}


typedef enum {
    PLAYER_STATUS_IDLE,
    PLAYER_STATUS_RUNNING_LEFT,
    PLAYER_STATUS_RUNNING_RIGHT,
    PLAYER_STATUS_JUMPING
} PlayerStatus;

typedef enum {
    GHOST_STATUS_IDLE,
    GHOST_STATUS_RUNNING_LEFT,
    GHOST_STATUS_RUNNING_RIGHT,
    GHOST_STATUS_AGRO
} GhostStatus;

// Serialized: player position_x position_y 
typedef struct {
    Vector2 position;
    Vector2 velocity;
    Vector2 size;
    Color color;
    Texture2D* sprite;
    int is_grounded;
    PlayerStatus status;
} Player;

// Serialized: platform start_x start_y length thickness
typedef struct {
    Vector2 start;
    float length;
    float thickness;
    Color color;
    Texture2D* sprite;
} Platform;

// Serialized: door position_x position_y
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

// Serialized: ghost position_x position_y
typedef struct {
    Vector2 position;
    Vector2 velocity;
    Vector2 size;
    Color color;
    Texture2D* sprite;
    GhostStatus status;
} Ghost;

typedef struct {
    Ghost** items;
    size_t count;
    size_t capacity;
} GhostCollection;

typedef struct {
    Vector2 position;
    Vector2 size;
    Color color;
    Texture2D* sprite;
} Key;

void draw_platform(Platform platform);
void draw_platforms(PlatformCollection collection);
Platform* make_platform(Vector2 start, float length, float thickness, Color color);
PlatformCollection make_platform_collection(Platform* first, ...);
PlatformCollection allocate_platform_collection(size_t capacity);
void add_platform(Platform* platform, PlatformCollection* collection);
void free_platforms(PlatformCollection platforms);

void draw_ghost(Ghost ghost);
void draw_ghosts(GhostCollection collection);
Ghost* make_ghost(Vector2 start, float scale);
GhostCollection make_ghost_collection(Ghost* first, ...);
GhostCollection allocate_ghost_collection(size_t capacity);
void add_ghost(Ghost* ghost, GhostCollection* collection);
void free_ghosts(GhostCollection ghosts);

void draw_door(Door door);
Vector2 place_door_on_platform(Platform platform);

void draw_player(Player player);
void print_player_position(Player player, int x, int y);
void print_platform_position(Platform platform, int x, int y);
void print_platforms_positions(PlatformCollection platforms, int x, int y);

#endif // ENTITIES_H
