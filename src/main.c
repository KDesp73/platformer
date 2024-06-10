#include <stdio.h>
#include <stdlib.h>
#include "entities.h"
#include "game.h"
#include "level.h"
#include "raylib.h"
#include "raymath.h"
#include "physics.h"
#include "textures.h"
#include "config.h"
#include "ui.h"
#include "builder.h"

#define CLIB_IMPLEMENTATION
#include "clib.h"

#define SET_FULLSCREEN(x) \
    do { \
        if(x) { \
            int display = GetCurrentMonitor(); \
            SetWindowSize(GetMonitorWidth(display), GetMonitorHeight(display)); \
        } else { \
            SetWindowSize(MIN_SCREEN_WIDTH, MIN_SCREEN_HEIGHT); \
        } \
        ToggleFullscreen(); \
    } while(0); 



void game(){
    Game game = {
        .level = 0,
        .is_over = false,
        .is_level_complete = false,
        .textures = load_textures(
            "assets/images/jess-30x50.png",
            "assets/images/door-50x80.png",
            "assets/images/wood-25x25.png",
            NULL // Teriminate the list
        )
    };

    PRINT_TEXTURE(PLAYER, game.textures);
    PRINT_TEXTURE(DOOR, game.textures);
    PRINT_TEXTURE(PLATFORM, game.textures);


    Levels levels = make_levels(
        make_level(
            (Vector2){SCREEN_WIDTH - 200, SCREEN_HEIGHT-10-50}, 
            make_platform_collection(
                make_platform((Vector2) {0, SCREEN_HEIGHT-10}, SCREEN_WIDTH, 25, WHITE),
                make_platform((Vector2) {600, 500}, 150, 25, WHITE),
                make_platform((Vector2) {100, 300}, 150, 25, WHITE),
                make_platform((Vector2) {400, 300}, 150, 25, WHITE),
                make_platform((Vector2) {500, 400}, 150, 25, WHITE),
                make_platform((Vector2) {800, 400}, 150, 25, WHITE),
                make_platform((Vector2) {100, 800}, 150, 25, WHITE),
                make_platform((Vector2) {200, 700}, 150, 25, WHITE),
                make_platform((Vector2) {900, 500}, 150, 25, WHITE),
                make_platform((Vector2) {300, 600}, 150, 25, WHITE),
                make_platform((Vector2) {200, 900}, 150, 25, WHITE),
                make_platform((Vector2) {600, 900}, 150, 25, WHITE),
                make_platform((Vector2) {800, 950}, 150, 25, WHITE),
                make_platform((Vector2) {1000, 1000}, 150, 25, WHITE),
                make_platform((Vector2) {1200, 1000}, 150, 25, WHITE),
                NULL // IMPORTANT!
            ), 
            (Vector2) {0,0},
            game.textures
        ),
        make_level(
            (Vector2){100, SCREEN_HEIGHT-10-50}, 
            make_platform_collection(
                make_platform((Vector2) {0, SCREEN_HEIGHT-10}, SCREEN_WIDTH, 25, WHITE),
                NULL
            ),
            (Vector2) {SCREEN_WIDTH - 70, SCREEN_HEIGHT - DOOR_SIZE.y - 10 - 25/2.0f},
            game.textures
        ),
        NULL
    );

    levels.items[0]->door.position = place_door_on_platform(*levels.items[0]->platforms.items[2]);

    game.player = levels.items[game.level]->player;

    while(!WindowShouldClose()){
        BeginDrawing();
        if(!game.is_over){
            if(!game.is_level_complete){
                update_player(&game.player, SCREEN_WIDTH, SCREEN_HEIGHT);
                check_and_resolve_platform_collisions(&game.player, levels.items[game.level]->platforms);

                if(check_door_collision(&game.player, levels.items[game.level]->door)){
                    DrawText("^", levels.items[game.level]->door.position.x + levels.items[game.level]->door.size.x / 2 - MeasureText("^", 70) / 2.0f, levels.items[game.level]->door.position.y - 70/2.0f - 10, 70, WHITE);
                    if(IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)){
                        game.is_level_complete = true;
                    }
                }

                ClearBackground(GetColor(0x181818FF));
                DrawText(TextFormat("Level %zu", game.level+1), 20, 20, 30, WHITE);
                draw_level(*(levels.items[game.level]), game.player, levels.items[game.level]->textures);
            } else {
                if(game.level == levels.count-1){
                    game.is_over = true;
                } else{
                    ClearBackground(GetColor(0x181818FF));
                    Cstr level_complete = TextFormat("LEVEL %zu COMPLETE", game.level+1);
                    DrawCenteredText(level_complete, SCREEN_HEIGHT/2 - 100, 100, WHITE);
                    DrawCenteredText("Press Enter to continue", SCREEN_HEIGHT/2 + 100, 70, WHITE);
                    if(IsKeyPressed(KEY_ENTER)){
                        game.level++;
                        game.is_level_complete = 0;
                        game.player = levels.items[game.level]->player;
                    }
                }
            }
        } else {
            ClearBackground(GetColor(0x181818FF));

            DrawCenteredText("Game Over", SCREEN_HEIGHT/2 - 100, 100, WHITE);
            DrawCenteredText("Press Enter to exit", SCREEN_HEIGHT/2 + 100, 70, WHITE);

            if(IsKeyPressed(KEY_ENTER)){
                for(size_t i = 0; i < levels.count; ++i){
                    free_platforms(levels.items[i]->platforms);
                }
                unload_textures(game.textures);

                EndMode2D();
                CloseWindow();
                exit(0);
            }
        }
        EndDrawing();
    }


    for(size_t i = 0; i < levels.count; ++i){
        free_platforms(levels.items[i]->platforms);
    }
    unload_textures(game.textures);

}

int main(int argc, char** argv){
    CliArguments args = clib_make_cli_arguments(4,
        clib_create_argument('h', "help", "Prints this message", no_argument),
        clib_create_argument('v', "version", "Prints the program version", no_argument),
        clib_create_argument('b', "builder", "Starts the level builder instead of the game", no_argument),
        clib_create_argument('c', "creator", "Prints this message", required_argument)
    );

    int is_builder = 0;
    Cstr creator = NULL; 


    struct option* opts = clib_get_options(args);
    int opt;
    while((opt = getopt_long(argc, argv, clib_generate_cli_format_string(args), opts, NULL)) != -1){
        switch (opt) {
            case 'h':
                clib_cli_help(args, CONCAT(argv[0], " [-b -c <Creator>] [-v | -h]"), "Made by KDesp73");
                exit(0);
            case 'v':
                printf("platformer %s\n", VERSION);
                exit(0);
            case 'b':
                is_builder = true;
                break;
            case 'c':
                creator = optarg;
                break;
            default:
                exit(1);
        }
    }

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, GAME_NAME);
    SetTargetFPS(FPS);

    Camera2D camera = { .zoom = 1.0f };
    BeginMode2D(camera);
    
    SET_FULLSCREEN(1);
    if(is_builder){
        if(creator != NULL)
            builder(creator);
        else {
            ERRO("Creator needs to be specified");
            exit(1);
        }
    } else {
        game();
    }

    EndMode2D();
    CloseWindow();
    return 0;
}

