#include <stdio.h>
#include "entities.h"
#include "game.h"
#include "raylib.h"
#include "raymath.h"
#include "physics.h"
#include "textures.h"

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

    Textures textures = make_textures(
        "assets/cat.png",
        "assets/door.png",
        "assets/dirt.png",
        NULL // Teriminate the list
    );
    
    Player player = {
        .position = (Vector2){w/2.0f, h/2.0f},
        .width= 20.0f,
        .height= 30.0f,
        .color = RED,
        .sprite = &textures.items[PLAYER]
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
        .sprite = &textures.items[DOOR]
    };

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

            DrawText(TextFormat("Level %zu", game.level), 20, 20, 30, WHITE);

            draw_door(door);
            draw_player(player);
            draw_platforms(platforms, textures.items[PLATFORM]);
        } else {
            ClearBackground(GetColor(0x181818FF));
            Cstr text = TextFormat("LEVEL %zu COMPLETE", game.level);
            int font_size = 100;
            int size = MeasureText(text, font_size);
            DrawText(text, w/2 - size / 2, h/2 - 100/2, font_size, WHITE);
        }
        EndDrawing();
    }
    EndMode2D();

    free_platforms(platforms);
    unload_textures(textures);
    CloseWindow();

    return 0;
}
