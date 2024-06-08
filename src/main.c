#include <stdio.h>
#include "entities.h"
#include "raylib.h"
#include "raymath.h"
#include "physics.h"

#define CLIB_IMPLEMENTATION
#include "clib.h"

#include "config.h"


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



int main(){
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, GAME_NAME);
    SetTargetFPS(FPS);
    Camera2D camera = { .zoom = 1.0f };
    BeginMode2D(camera);
    
    // TODO: Save and Load from file
    Settings settings = {
        .is_fullscreen = 1
    };

    INFO("is fullscreen: %d", settings.is_fullscreen);
    // SET_FULLSCREEN(settings.is_fullscreen);

    int w = GetScreenWidth();
    int h = GetScreenHeight();
    Player player = {
        .position = (Vector2){w/2.0f, h/2.0f},
        .width= 20.0f,
        .height= 30.0f,
        .color = RED
    };

    PlatformCollection platforms = make_platform_collection(
        make_platform((Vector2) {100, 300}, 200, 20, WHITE),
        make_platform((Vector2) {400, 300}, 200, 20, WHITE),
        make_platform((Vector2) {500, 400}, 200, 20, WHITE),
        make_platform((Vector2) {800, 400}, 200, 20, WHITE),
        make_platform((Vector2) {100, 800}, 200, 20, WHITE),
        make_platform((Vector2) {200, 700}, 200, 20, WHITE),
        make_platform((Vector2) {300, 600}, 200, 20, WHITE),
        make_platform((Vector2) {900, 500}, 200, 20, WHITE),
        make_platform((Vector2) {600, 500}, 200, 20, WHITE),
        make_platform((Vector2) {w/2.0f, h-20}, 200, 20, WHITE),
        make_platform((Vector2) {0, h}, 200, 20, WHITE),
        NULL // Teriminate the list
    );

    DEBU("count: %zu", platforms.count);

    while(!WindowShouldClose()){
        update_player(&player, w, h);
        check_and_resolve_collisions(&player, platforms);

        ClearBackground(GetColor(0x181818FF));
        BeginDrawing();

        draw_player(player);
        draw_platforms(platforms);

#ifdef DEBUG
        print_player_position(player, 10, 10);
#endif

        EndDrawing();
    }

    free_platforms(platforms);
    CloseWindow();

    return 0;
}
