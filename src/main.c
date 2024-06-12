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
        .is_over = false,
        .is_level_complete = false,
        .textures = load_textures(
            "assets/images/jess-30x50.png",
            "assets/images/door-50x80.png",
            "assets/images/wood-25x25.png",
            NULL // Teriminate the list
        ),
    };
    game.player = (Player) {
        .status = IDLE,
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

    copy_player(&game.player, levels.items[0]->player);
    DEBU("level 1 player coords: %.0f %.0f", game.player.position.x, game.player.position.y);
    DEBU("level 1 door coords: %.0f %.0f", levels.items[0]->door.position.x, levels.items[0]->door.position.y);
    while(!WindowShouldClose()){
        BeginDrawing();

        if(!game.is_over){
            if(!game.is_level_complete){
                run_level(*levels.items[game.level], &game);
                show_mouse_coords_at_cursor();
            } else {
                if(game.level == levels.count-1){
                    game.is_over = true;
                } else{
                    ClearBackground(GetColor(0x181818FF));
                    Cstr level_complete = TextFormat("LEVEL %zu COMPLETE", game.level+1);
                    DrawCenteredText(level_complete, SCREEN_WIDTH, SCREEN_HEIGHT - 200, 100, WHITE);
                    DrawCenteredText("Press Enter to continue", SCREEN_WIDTH, SCREEN_HEIGHT + 200, 70, WHITE);
                    if(IsKeyPressed(KEY_ENTER)){
                        game.level++;
                        game.is_level_complete = 0;
                        copy_player(&game.player, levels.items[game.level]->player);
                        game.player = levels.items[game.level]->player;
                    }
                }
            }
        } else {
            ClearBackground(GetColor(0x181818FF));

            DrawCenteredText("Game Over", SCREEN_WIDTH, SCREEN_HEIGHT - 200, 100, WHITE);
            DrawCenteredText("Press Enter to exit", SCREEN_WIDTH, SCREEN_HEIGHT + 200, 70, WHITE);

            if(IsKeyPressed(KEY_ENTER)){
                clean(&levels, &game);
                exit(0);
            }
        }

        EndDrawing();
    }

    clean(&levels, &game);
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
                if(scale < 1.0f || scale > 2.0f) PANIC("Scale must be between 1 and 2");
                break;
            default:
                exit(1);
        }
    }

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

