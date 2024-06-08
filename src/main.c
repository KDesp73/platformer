#include <stdio.h>
#include "entities.h"
#include "game.h"
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

    Game game = {
        .level = 1,
        .game_over = false
    };

    Texture2D cat_texture = LoadTexture("assets/cat.png");
    Texture2D door_texture = LoadTexture("assets/door.png");
    Player player = {
        .position = (Vector2){w/2.0f, h/2.0f},
        .width= 20.0f,
        .height= 30.0f,
        .color = RED,
        .sprite = &cat_texture
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

    Door door = {
        .position = (Vector2) {150, 300-60-10},
        .size = (Vector2) {40.0f, 60.0f},
        .color = GREEN,
        .sprite = &door_texture
    };

    DEBU("count: %zu", platforms.count);

    while(!WindowShouldClose()){
        BeginDrawing();
        if(!game.game_over){
            update_player(&player, w, h);
            check_and_resolve_platform_collisions(&player, platforms);

            if(check_door_collision(&player, door)){
                DrawText("^", door.position.x + door.size.x / 2 - MeasureText("^", 70) / 2.0f, door.position.y - 70/2.0f - 10, 70, WHITE);
                if(IsKeyPressed(KEY_UP)){
                    game.game_over = true;
                }
            }

            ClearBackground(GetColor(0x181818FF));

            draw_door(door);
            draw_player(player);
            draw_platforms(platforms);

#ifdef DEBUG
            print_player_position(player, 10, 10);
#endif
        } else {
            ClearBackground(GetColor(0x181818FF));
            Cstr text = "LEVEL COMPLETE";
            int font_size = 100;
            int size = MeasureText(text, font_size);
            DrawText(text, w/2 - size / 2, h/2 - 100/2, font_size, WHITE);
        }


        EndDrawing();
    }

    free_platforms(platforms);
    UnloadTexture(cat_texture);
    UnloadTexture(door_texture);
    CloseWindow();

    return 0;
}
