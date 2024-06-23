#include <stdio.h>
#include <stdlib.h>
#include "game.h"
#include "raylib.h"
#include "config.h"
#include "builder.h"
#include "utils.h"

#define CLIB_IMPLEMENTATION
#include "extern/clib.h"

int main(int argc, char** argv)
{
    CliArguments args = clib_make_cli_arguments(5,
        clib_create_argument('h', "help", "Prints this message", no_argument),
        clib_create_argument('v', "version", "Prints the program version", no_argument),
        clib_create_argument('b', "builder", "Starts the level builder instead of the game", no_argument),
        clib_create_argument('c', "creator", "Prints this message", required_argument),
        clib_create_argument('s', "scale", "Sets the scale of the level (default: 1.0f)", required_argument)
    );

    int arg_builder = 0;
    float arg_scale = 1.0f;
    Cstr arg_creator = NULL; 


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
                arg_builder = true;
                break;
            case 'c':
                arg_creator = optarg;
                break;
            case 's':
                arg_scale = atof(optarg);
                if(arg_scale < .5f || arg_scale > 2.0f) {
                    PANIC("Scale must be between 0.5 and 2.0");
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
    if(arg_builder){
        if(arg_creator != NULL)
            builder(arg_creator, arg_scale);
        else {
            ERRO("Creator needs to be specified");
            exit(1);
        }
    } else {
        run_game();
    }

    return 0;
}

