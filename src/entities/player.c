#include "entities.h"
#include <assert.h>
#include <stdarg.h>

#define CLIB_IMPLEMENTATION
#include "extern/clib.h"
#include "raylib.h"

void draw_player(Player player) 
{
    if(player.sprite->width <= 0 || player.sprite->height <= 0){
        DrawRectangleV(player.position, (Vector2) {player.size.x, player.size.y}, player.color);
    } else {
        if(IsTextureReady(*player.sprite)){
            switch (player.status) {
                case PLAYER_STATUS_RUNNING_LEFT:
                    // DrawTextureRec(*player.sprite, (Rectangle) {0,0, -player.size.x, player.size.y}, player.position, WHITE);
                    DrawTexturePro(*player.sprite, (Rectangle) {0,0, -25.0f, 50.0f} , (Rectangle) {player.position.x, player.position.y, player.size.x, player.size.y}, (Vector2) {0}, 0, WHITE);
                    break;
                case PLAYER_STATUS_IDLE:
                case PLAYER_STATUS_RUNNING_RIGHT:
                    // DrawTextureRec(*player.sprite, (Rectangle) {0,0, player.size.x, player.size.y}, player.position, WHITE);
                    DrawTexturePro(*player.sprite, (Rectangle) {0,0, 25.0f, 50.0f} , (Rectangle) {player.position.x, player.position.y, player.size.x, player.size.y}, (Vector2) {0}, 0, WHITE);
                    break;
                default:
                    ERRO("Unknown status");
                    break;
            }
        } 
    }
}
