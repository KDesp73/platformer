#include "entities.h"
#include <assert.h>
#include <stdarg.h>

#define CLIB_IMPLEMENTATION
#include "extern/clib.h"
#include "raylib.h"

void draw_door(Door door)
{
    if(door.sprite->width <= 0 || door.sprite->height <= 0){
        DrawRectangleV(door.position, (Vector2) {door.size.x, door.size.y}, door.color);
    } else {
        if(IsTextureReady(*door.sprite)){
            Rectangle src = {0,0,50.0f, 75.0f};
            Rectangle dest = {door.position.x, door.position.y, door.size.x, door.size.y};
            DrawTexturePro(*door.sprite, src , dest, (Vector2) {0}, 0, WHITE);
        }  
    }
}
