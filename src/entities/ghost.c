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

void draw_ghost(Ghost ghost)
{
    if(ghost.sprite->width <= 0 || ghost.sprite->height <= 0){
        DrawRectangleV(ghost.position, (Vector2) {ghost.size.x, ghost.size.y}, ghost.color);
    } else {
        if(IsTextureReady(*ghost.sprite)){
            switch (ghost.status) {
                case GHOST_STATUS_AGRO:
                case GHOST_STATUS_RUNNING_LEFT:
                    // DrawTextureRec(*ghost.sprite, (Rectangle) {0,0, -ghost.size.x, ghost.size.y}, ghost.position, WHITE);
                    DrawTexturePro(*ghost.sprite, (Rectangle) {0,0, -50.0f, 50.0f} , (Rectangle) {ghost.position.x, ghost.position.y, ghost.size.x, ghost.size.y}, (Vector2) {0}, 0, WHITE);
                    break;
                case GHOST_STATUS_IDLE:
                case GHOST_STATUS_RUNNING_RIGHT:
                    // DrawTextureRec(*ghost.sprite, (Rectangle) {0,0, ghost.size.x, ghost.size.y}, ghost.position, WHITE);
                    DrawTexturePro(*ghost.sprite, (Rectangle) {0,0, 50.0f, 50.0f} , (Rectangle) {ghost.position.x, ghost.position.y, ghost.size.x, ghost.size.y}, (Vector2) {0}, 0, WHITE);
                    break;
                default:
                    ERRO("Unknown status");
                    break;
            }
        } 
    }
}

void draw_ghosts(GhostCollection collection)
{
    for (size_t i = 0; i < collection.count; ++i) {
        if(collection.items[i] != NULL)
            draw_ghost(*collection.items[i]);
    }
}

Ghost* make_ghost(Vector2 start, float scale)
{
    Ghost* ghost = (Ghost*) malloc(sizeof(Ghost));

    ghost->position = start;
    ghost->size = GHOST_SIZE(scale);
    ghost->status = GHOST_STATUS_IDLE;
    ghost->color = GHOST_COLOR;

    return ghost;
}

GhostCollection make_ghost_collection(Ghost* first, ...)
{
    GhostCollection result = {0};
    if (first == NULL) {
        ERRO("No platforms added");
        return result;
    }

    result.count = 1;
    va_list args;
    va_start(args, first);
    for (Ghost* next = va_arg(args, Ghost*); next != NULL; next = va_arg(args, Ghost*)) {
        result.count += 1;
    }
    va_end(args);
    result.capacity = result.count;

    result.items = (Ghost**) malloc(sizeof(Ghost*) * result.count);
    if (result.items == NULL) {
        PANIC("Could not allocate memory: %s", strerror(errno));
    }

    result.count = 0;

    result.items[result.count++] = first;

    va_start(args, first);
    for (Ghost* next = va_arg(args, Ghost*); next != NULL; next = va_arg(args, Ghost*)) {
        result.items[result.count++] = next;
    }
    va_end(args);

    return result;
}

GhostCollection allocate_ghost_collection(size_t capacity)
{
    GhostCollection result = {.capacity = capacity};
    result.items = (Ghost**) malloc(sizeof(Ghost) * capacity);
    
    return result;
}

void add_ghost(Ghost* ghost, GhostCollection* collection)
{
    assert(ghost != NULL);
    assert(collection->items != NULL);

    if(collection->count >= collection->capacity) {
        PANIC("Max capacity");
    }

    collection->items[collection->count++] = ghost;
}

void free_ghosts(GhostCollection ghosts)
{
    for (size_t i = 0; i < ghosts.count; ++i) {
        if (ghosts.items[i] != NULL) {
            free(ghosts.items[i]);
            ghosts.items[i] = NULL; // Optional: to avoid dangling pointers
        }
    }
    free(ghosts.items);
}

