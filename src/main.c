#include <stdio.h>
#include <stdlib.h>
#include "entities.h"
#include "game.h"
#include "level.h"
#include "raylib.h"
#include "textures.h"
#include "config.h"
#include "ui.h"
#include "builder.h"
#include "utils.h"

#define CLIB_IMPLEMENTATION
#include "clib.h"


void run_game(){
    Game game = {
        .level = 0,
        .status = -1,
        .textures = load_textures(
            "assets/images/jess-30x50.png",
            "assets/images/door-50x75.png",
            "assets/images/wood-25x25.png",
            "assets/images/ghost-50x50.png",
            NULL // Teriminate the list
        ),
    };
    game.player = (Player) {
        .status = PLAYER_STATUS_IDLE,
        .sprite = &game.textures.items[PLAYER],
        .is_grounded = false,
        .velocity = {0},
        .position = {0},
        .size = PLAYER_SIZE(1.0f),
        .color = PLAYER_COLOR
    };

    Camera2D camera = { 0 };
    camera.zoom = 1.0f;
    BeginMode2D(camera);


    Levels levels = load_levels_from_dir("assets/levels", game.textures);

    if(levels.items == NULL){
        PANIC("Couldn't load levels");
    }

    COPY_PLAYER(game.player, levels.items[0]->player);
    print_vector2(game.player.position, "game.player");
    while(!WindowShouldClose()){
        BeginDrawing();

        if(game.status != GAME_STATUS_GAME_OVER && game.status != GAME_STATUS_PLAYER_DIED){
            if(game.status != GAME_STATUS_NEXT_LEVEL && game.status != GAME_STATUS_PREV_LEVEL){
                run_level(*levels.items[game.level], &game);
            } else {
                ClearBackground(GetColor(0x181818FF));
                if(game.status == GAME_STATUS_NEXT_LEVEL){
                    if(game.level == levels.count-1){
                        game.status = GAME_STATUS_GAME_OVER;
                    } else{
                        Cstr level_complete = TextFormat("LEVEL %zu COMPLETE", game.level+1);
                        DrawCenteredText(level_complete, SCREEN_WIDTH, SCREEN_HEIGHT - 200, 100, WHITE);
                        DrawCenteredText("Press Enter to continue", SCREEN_WIDTH, SCREEN_HEIGHT + 200, 70, WHITE);

                        if(IsKeyPressed(KEY_ENTER)){
                            game.level++;
                            game.status = -1;
                            COPY_PLAYER(game.player, levels.items[game.level]->player);
                        }
                    }
                } else {
                    Cstr level_complete = TextFormat("You fell");
                    DrawCenteredText(level_complete, SCREEN_WIDTH, SCREEN_HEIGHT - 200, 100, WHITE);
                    DrawCenteredText("Press Enter to continue", SCREEN_WIDTH, SCREEN_HEIGHT + 200, 70, WHITE);

                    if(IsKeyPressed(KEY_ENTER)){
                        game.level--;
                        game.status = -1;
                        COPY_PLAYER(game.player, levels.items[game.level]->player);
                    }
                }
            }
        } else {
            ClearBackground(GetColor(0x181818FF));

            if(game.status == GAME_STATUS_GAME_OVER){
                DrawCenteredText("Game Over", SCREEN_WIDTH, SCREEN_HEIGHT - 200, 100, WHITE);
            } else {
                DrawCenteredText("YOU DIED", SCREEN_WIDTH, SCREEN_HEIGHT - 200, 100, WHITE);
            }
            DrawCenteredText("Press Enter to exit", SCREEN_WIDTH, SCREEN_HEIGHT + 200, 70, WHITE);

            if(IsKeyPressed(KEY_ENTER)){
                clean_and_exit(&levels, &game);
                (game.status = GAME_STATUS_PLAYER_DIED) ? INFO("Too bad...") : INFO("Congratulations!");
                exit(0);
            }
        }

        EndDrawing();
    }

    clean_and_exit(&levels, &game);
}

int main(int argc, char** argv){
    CliArguments args = clib_make_cli_arguments(5,
        clib_create_argument('h', "help", "Prints this message", no_argument),
        clib_create_argument('v', "version", "Prints the program version", no_argument),
        clib_create_argument('b', "builder", "Starts the level builder instead of the game", no_argument),
        clib_create_argument('c', "creator", "Prints this message", required_argument),
        clib_create_argument('s', "scale", "Sets the scale of the level (default: 1.0f)", required_argument)
    );

    int is_builder = 0;
    Cstr creator = NULL; 
    float scale = 1.0f;


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
            case 's':
                scale = atof(optarg);
                if(scale < .5f || scale > 2.0f) {
                    PANIC("Scale must be between 0.5 and 2.0");
                    exit(1);
                }
                break;
            default:
                exit(1);
        }
    }

    SetTraceLogLevel(LOG_WARNING);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, GAME_NAME);
    SetTargetFPS(FPS);

    SET_FULLSCREEN(1);
    if(is_builder){
        if(creator != NULL)
            builder(creator, scale);
        else {
            ERRO("Creator needs to be specified");
            exit(1);
        }
    } else {
        run_game();
    }

    return 0;
}

