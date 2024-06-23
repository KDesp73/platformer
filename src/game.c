#include "game.h"
#include "config.h"
#include "level.h"
#include "textures.h"
#include "utils.h"
#include "ui.h"

void run_game()
{
    Game game = {
        .level = 0,
        .status = GAME_STATUS_ACTIVE,
        .textures = load_textures(
            "assets/images/jess-30x50.png",
            "assets/images/door-50x75.png",
            "assets/images/wood-25x25.png",
            "assets/images/ghost-50x50.png",
            NULL // Teriminate the list
        ),
    };

    Camera2D camera = { 0 };
    camera.zoom = 1.0f;
    BeginMode2D(camera);


    Levels levels = load_levels_from_dir("assets/levels", game.textures);

    if(levels.items == NULL){
        PANIC("Couldn't load levels");
    }

    game.current_level = *levels.items[0];
    while(!WindowShouldClose()){
        BeginDrawing();

        if(game.status != GAME_STATUS_GAME_OVER && game.status != GAME_STATUS_PLAYER_DIED){
            if(game.status != GAME_STATUS_NEXT_LEVEL && game.status != GAME_STATUS_PREV_LEVEL){
                run_level(&game);
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
                            game.current_level = *levels.items[game.level];
                        }
                    }
                } else {
                    Cstr level_complete = TextFormat("You fell");
                    DrawCenteredText(level_complete, SCREEN_WIDTH, SCREEN_HEIGHT - 200, 100, WHITE);
                    DrawCenteredText("Press Enter to continue", SCREEN_WIDTH, SCREEN_HEIGHT + 200, 70, WHITE);

                    if(IsKeyPressed(KEY_ENTER)){
                        game.level--;
                        game.status = -1;
                        game.current_level = *levels.items[game.level];
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
                (game.status == GAME_STATUS_PLAYER_DIED) ? INFO("Too bad...") : INFO("Congratulations!");
                exit(0);
            }
        }

        EndDrawing();
    }

    clean_and_exit(&levels, &game);
}
