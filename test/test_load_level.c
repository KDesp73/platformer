#include "config.h"
#include "entities.h"
#include "game.h"
#include "raylib.h"
#include "textures.h"
#include <stdio.h>
#define CLIB_IMPLEMENTATION
#include "extern/clib.h"
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
    return TextFormat("%.2f %.2f", vec.x, vec.y);
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
    SetTraceLogLevel(LOG_NONE);
    InitWindow(0, 0, "Test");
    Level* level = load_level_from_file("test/test.txt", (Textures) {.items = NULL});

    float cell_size = CELL_SIZE(level->scale);
    Vector2 expected_player = {0, 0};
    Vector2 expected_door = {10 * cell_size, 9 * cell_size};
    TestPlatform expected_platforms[30] = {
        (TestPlatform){.start = (Vector2) {20 * cell_size, 20 * cell_size + cell_size / 2}, .length = 10 * cell_size},
        (TestPlatform){.start = (Vector2) {4 * cell_size, 2 * cell_size + cell_size / 2}, .length = 5 * cell_size},
        (TestPlatform){.start = (Vector2) {30 * cell_size, 10 * cell_size + cell_size / 2}, .length = 1 * cell_size},
        (TestPlatform){.start = (Vector2) {24 * cell_size, 18 * cell_size + cell_size / 2}, .length = 7 * cell_size},
        (TestPlatform){.start = (Vector2) {35 * cell_size, 12 * cell_size + cell_size / 2}, .length = 3 * cell_size},
        (TestPlatform){.start = (Vector2) {40 * cell_size, 20 * cell_size + cell_size / 2}, .length = 5 * cell_size},
        (TestPlatform){.start = (Vector2) {22 * cell_size, 15 * cell_size + cell_size / 2}, .length = 9 * cell_size},
        (TestPlatform){.start = (Vector2) {38 * cell_size, 25 * cell_size + cell_size / 2}, .length = 2 * cell_size},
        (TestPlatform){.start = (Vector2) {31 * cell_size, 19 * cell_size + cell_size / 2}, .length = 6 * cell_size},
        (TestPlatform){.start = (Vector2) {27 * cell_size, 14 * cell_size + cell_size / 2}, .length = 4 * cell_size},
        (TestPlatform){.start = (Vector2) {29 * cell_size, 21 * cell_size + cell_size / 2}, .length = 8 * cell_size},
        (TestPlatform){.start = (Vector2) {33 * cell_size, 23 * cell_size + cell_size / 2}, .length = 1 * cell_size},
        (TestPlatform){.start = (Vector2) {36 * cell_size, 11 * cell_size + cell_size / 2}, .length = 10 * cell_size},
        (TestPlatform){.start = (Vector2) {42 * cell_size, 16 * cell_size + cell_size / 2}, .length = 7 * cell_size},
        (TestPlatform){.start = (Vector2) {39 * cell_size, 13 * cell_size + cell_size / 2}, .length = 5 * cell_size},
        (TestPlatform){.start = (Vector2) {28 * cell_size, 24 * cell_size + cell_size / 2}, .length = 3 * cell_size},
        (TestPlatform){.start = (Vector2) {34 * cell_size, 22 * cell_size + cell_size / 2}, .length = 9 * cell_size},
        (TestPlatform){.start = (Vector2) {30 * cell_size, 17 * cell_size + cell_size / 2}, .length = 6 * cell_size},
        (TestPlatform){.start = (Vector2) {37 * cell_size, 26 * cell_size + cell_size / 2}, .length = 4 * cell_size},
        (TestPlatform){.start = (Vector2) {25 * cell_size, 20 * cell_size + cell_size / 2}, .length = 8 * cell_size},
        (TestPlatform){.start = (Vector2) {32 * cell_size, 18 * cell_size + cell_size / 2}, .length = 2 * cell_size},
        (TestPlatform){.start = (Vector2) {41 * cell_size, 15 * cell_size + cell_size / 2}, .length = 10 * cell_size},
        (TestPlatform){.start = (Vector2) {23 * cell_size, 19 * cell_size + cell_size / 2}, .length = 1 * cell_size},
        (TestPlatform){.start = (Vector2) {-1 * cell_size, -1 * cell_size + cell_size / 2}, .length = 0 * cell_size},
    };

    test((
        test(1, "Check", "1", "1") &
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
        test_platforms(expected_platforms, level)
    ), "Test", NULL, NULL);


    CloseWindow();
    return 0;
}


// creator test
// scale 1.0
// player 0 0
// door 10 9
// platform 20 20 10
// platform 4 2 5
// platform 30 10 1
// platform 24 18 7
// platform 35 12 3
// platform 40 20 5
// platform 22 15 9
// platform 38 25 2
// platform 31 19 6
// platform 27 14 4
// platform 29 21 8
// platform 33 23 1
// platform 36 11 10
// platform 42 16 7
// platform 39 13 5
// platform 28 24 3
// platform 34 22 9
// platform 30 17 6
// platform 37 26 4
// platform 25 20 8
// platform 32 18 2
// platform 41 15 10
// platform 23 19 1
// platform -1 -1 0
