#include <stdio.h>
#include "entities.h"
#include "game.h"
#include "level.h"
#include "raylib.h"
#include "raymath.h"
#include "physics.h"
#include "textures.h"
#include "config.h"

#define CLIB_IMPLEMENTATION
#include "clib.h"



#define SET_FULLSCREEN(x) \
    do { \
        if(x) { \
            DEBU("FULL"); \
            int display = GetCurrentMonitor(); \
            SetWindowSize(GetMonitorWidth(display), GetMonitorHeight(display)); \
        } else { \
            SetWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT); \
        } \
        if(!IsWindowFullscreen()){ \
            ToggleFullscreen(); \
        } \
    } while(0); 


void DrawCenteredText(Cstr text, int y, int fontSize, Color color){
    int size = MeasureText(text, fontSize);
    DrawText(text, GetScreenWidth() / 2.0f - size / 2.0f, y - fontSize / 2.0f, fontSize, color);
}

int main(){
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, GAME_NAME);
    SetTargetFPS(FPS);
    Camera2D camera = { .zoom = 1.0f };
    BeginMode2D(camera);
    
    // TODO: Save and Load from file
    Settings settings = {
        .is_fullscreen = 0
    };

    INFO("is fullscreen: %d", settings.is_fullscreen);
    // SET_FULLSCREEN(settings.is_fullscreen);

    int w = GetScreenWidth();
    int h = GetScreenHeight();

    Game game = {
        .level = 1,
        .is_over = false
    };

    Textures textures = load_textures(
#ifndef NO_ASSETS
        "assets/player-30x50.png",
        "assets/door-50x80.png",
        "assets/wood-25x25.png",
#endif
        NULL // Teriminate the list
    );

    PRINT_TEXTURE(PLAYER, textures);
    PRINT_TEXTURE(DOOR, textures);
    PRINT_TEXTURE(PLATFORM, textures);

    Level level = {
        .which = 1,
        .player = {
            .position = (Vector2){w/2.0f, h/2.0f},
            .size = (Vector2) {30.0f, 50.0f},
            .color = RED,
            .sprite = &textures.items[PLAYER],
            .status = IDLE
        },
        .platforms = make_platform_collection(
            make_platform((Vector2) {0, h-10}, w, 25, WHITE),
            make_platform((Vector2) {600, 500}, 150, 25, WHITE),
            make_platform((Vector2) {100, 300}, 150, 25, WHITE),
            make_platform((Vector2) {400, 300}, 150, 25, WHITE),
            make_platform((Vector2) {500, 400}, 150, 25, WHITE),
            make_platform((Vector2) {800, 400}, 150, 25, WHITE),
            make_platform((Vector2) {100, 800}, 150, 25, WHITE),
            make_platform((Vector2) {200, 700}, 150, 25, WHITE),
            make_platform((Vector2) {900, 500}, 150, 25, WHITE),
            make_platform((Vector2) {300, 600}, 150, 25, WHITE),
            NULL // IMPORTANT!
        ),
        .door = {
            .size = (Vector2) {50.0f, 80.0f},
            .color = GREEN,
            .sprite = &textures.items[DOOR]
        }
    };

    // Player player = {
    //     .position = (Vector2){w/2.0f, h/2.0f},
    //     .size = (Vector2) {30.0f, 50.0f},
    //     .color = RED,
    //     .sprite = &textures.items[PLAYER],
    //     .status = IDLE
    // };


    // PlatformCollection platforms = make_platform_collection(
    //     make_platform((Vector2) {0, h-10}, w, 25, WHITE),
    //     make_platform((Vector2) {600, 500}, 150, 25, WHITE),
    //     make_platform((Vector2) {100, 300}, 150, 25, WHITE),
    //     make_platform((Vector2) {400, 300}, 150, 25, WHITE),
    //     make_platform((Vector2) {500, 400}, 150, 25, WHITE),
    //     make_platform((Vector2) {800, 400}, 150, 25, WHITE),
    //     make_platform((Vector2) {100, 800}, 150, 25, WHITE),
    //     make_platform((Vector2) {200, 700}, 150, 25, WHITE),
    //     make_platform((Vector2) {900, 500}, 150, 25, WHITE),
    //     make_platform((Vector2) {300, 600}, 150, 25, WHITE),
    //     NULL // IMPORTANT!
    // );

    // Door door = {
    //     .size = (Vector2) {50.0f, 80.0f},
    //     .color = GREEN,
    //     .sprite = &textures.items[DOOR]
    // };

    place_door_on_platform(&level.door, *level.platforms.items[2]);
    while(!WindowShouldClose()){
        BeginDrawing();
        if(!game.is_over){
            update_player(&level.player, w, h);
            check_and_resolve_platform_collisions(&level.player, level.platforms);

            if(check_door_collision(&level.player, level.door)){
                DrawText("^", level.door.position.x + level.door.size.x / 2 - MeasureText("^", 70) / 2.0f, level.door.position.y - 70/2.0f - 10, 70, WHITE);
                if(IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)){
                    game.is_over = true;
                }
            }

            ClearBackground(GetColor(0x181818FF));

            DrawText(TextFormat("Level %zu", game.level), 20, 20, 30, WHITE);

            draw_level(level, textures);
        } else {
            ClearBackground(GetColor(0x181818FF));
            Cstr level_complete = TextFormat("LEVEL %zu COMPLETE", game.level);
            
            DrawCenteredText(level_complete, h/2 - 100, 100, WHITE);
            DrawCenteredText("Press Enter to continue", h/2 + 100, 70, WHITE);

            if(IsKeyPressed(KEY_ENTER)){
                game.level++;
                game.is_over = 0;
            }
        }
        EndDrawing();
    }

    free_platforms(level.platforms);
    unload_textures(textures);

    EndMode2D();
    CloseWindow();

    return 0;
}

