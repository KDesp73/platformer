#include "raylib.h"
#include <stdio.h>
#define CLIB_IMPLEMENTATION
#include "clib.h"
#include "level.h"

#define PASS(str, ...) \
    LOG(stdout, "PASS", str, ##__VA_ARGS__)
#define FAIL(str, ...) \
    LOG(stdout, "FAIL", str, ##__VA_ARGS__)

int test(int expression, Cstr tag){
    if(expression)
        PASS("%s%s passed%s", GREEN, tag, RESET);
    else
        FAIL("%s%s failed%s", RED, tag, RESET);

    return expression;
}

int test_level_player_door(Level* level, Vector2 expected_player, Vector2 expected_door){
    return test(level->player.position.x == expected_player.x && level->player.position.y == expected_player.y, "Player") &&
        test(level->door.position.x == expected_door.x && level->door.position.y == expected_door.y, "Door");
}

typedef struct {
    Vector2 start;
    float length;
} TestPlatform;

int main(){
    InitWindow(0, 0, "Test");
    Textures textures = load_textures(
        "assets/images/jess-30x50.png",
        "assets/images/door-50x80.png",
        "assets/images/wood-25x25.png",
        NULL // Teriminate the list
    );
    /*
    creator KDesp73
    player 125 900
    door 600 250
    platform 100 950 325
    platform 475 825 250
    platform 1025 725 125
    platform 1300 550 125
    platform 1650 700 125
    platform 1800 875 175
    platform 1300 925 175
    platform 575 325 350
    */
    Level* level = load_level_from_file("assets/levels/level-KDesp73-2024-06-11 14:07:39.txt", textures);

    test(test_level_player_door(level, (Vector2) {125, 900}, (Vector2) {600, 250}), "Test");

    CloseWindow();

    return 0;
}
