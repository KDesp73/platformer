#include <stdio.h>
#include "raylib.h"
#include "raymath.h"

#define CLIB_IMPLEMENTATION
#include "clib.h"

int main(){
    INFO("Hello World");
    InitWindow(100, 100, "test");

    while(!WindowShouldClose()){
        BeginDrawing();

        EndDrawing();
    }

    return 0;
}
