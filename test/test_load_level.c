#include "config.h"
#include "entities.h"
#include "game.h"
#include "raylib.h"
#include <stdio.h>
#define CLIB_IMPLEMENTATION
#include "clib.h"
#include "level.h"

#define PASS(fmt, ...) \
    LOG(stdout, "PASS", fmt, ##__VA_ARGS__)
#define FAIL(fmt, ...) \
    LOG(stderr, "FAIL", fmt, ##__VA_ARGS__)

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

char* floatToString(float number) {
    static char str[50]; // Static buffer, not thread-safe
    snprintf(str, sizeof(str), "%.2f", number); // Adjust precision as needed
    return str;
}

int test(int expression, Cstr tag, Cstr expected_msg, Cstr found_msg){
    if(expression)
        PASS("%s%s passed%s", COLOR_FG(2), tag, RESET);
    else{
        FAIL("%s%s failed%s", COLOR_FG(1), tag, RESET);
        if(expected_msg != NULL)
            printf("\tExpected: %s\n", expected_msg);
        if(found_msg != NULL)
            printf("\tGot: %s\n", found_msg);
    }


    return expression;
}

typedef struct {
    Vector2 start;
    float length;
} TestPlatform;

Cstr vec_to_string(Vector2 vec){
    return TextFormat("%.0f %.0f", vec.x, vec.y);
}

int platformcmp(TestPlatform p1, Platform p2){
    return p1.start.x == p2.start.x && p1.start.y == p2.start.y && p1.length == p2.length;
}

int vectorcmp(Vector2 v1, Vector2 v2){
    return v1.x == v2.x && v1.y == v2.y;
}

int test_platforms(TestPlatform* expected_platforms, Level* level){
    int passed = true;
    for(size_t i = 0; i < level->platforms.count; ++i){
        passed &= test(
            platformcmp(expected_platforms[i], *level->platforms.items[i]),
            TextFormat("Platform %zu", i),
            CONCAT(vec_to_string(expected_platforms[i].start), " and ", floatToString(expected_platforms[i].length)),
            CONCAT(vec_to_string(level->platforms.items[i]->start), " and ", floatToString(level->platforms.items[i]->length))
        );
    }
    return passed;
}

int main(){
    InitWindow(0, 0, "Test");
    Textures textures = load_textures(
        "assets/images/jess-30x50.png",
        "assets/images/door-50x80.png",
        "assets/images/wood-25x25.png",
        NULL // Teriminate the list
    );
    Level* level = load_level_from_file("assets/levels/test.txt", textures);

// creator test
// scale 1.0
// player 0 0
// door 10 9
// platform 20 20 10
// platform 4 2 5
// platform 30 10 1
// platform -1 -1 0

    float cell_size = CELL_SIZE(level->scale);
    INFO("cell size: %f", cell_size);
    Vector2 expected_player = {0};
    Vector2 expected_door = {10 * cell_size, 9 * cell_size};
    TestPlatform platforms[4] = {
        (TestPlatform){.start = (Vector2) {20 * cell_size, 20 * cell_size + cell_size / 2}, .length = 10 * cell_size},
        (TestPlatform){.start = (Vector2) {4 * cell_size, 2 * cell_size + cell_size / 2}, .length = 5 * cell_size},
        (TestPlatform){.start = (Vector2) {30 * cell_size, 10 * cell_size + cell_size / 2}, .length = 1 * cell_size},
        (TestPlatform){.start = (Vector2) {-1 * cell_size, -1 * cell_size + cell_size / 2}, .length = 0 * cell_size},
    };

    test((
        test(1, "Check", "1", "1") &
        test(
            level->scale == 1.0f,
            "Scale", 
            "1.0f", 
            floatToString(level->scale)
        ) &
        test(
            vectorcmp(expected_player, level->player.position),
            "Player", 
            vec_to_string(expected_player),
            vec_to_string(level->player.position)
        ) &
        test(
            vectorcmp(expected_door, level->door.position),
            "Door",
            vec_to_string(expected_door),
            vec_to_string(level->door.position)
        ) &
        test_platforms(platforms, level)
    ), "Test", NULL, NULL);

    // test(level->platforms.items[0]->start = 100,)

    CloseWindow();

    return 0;
}
