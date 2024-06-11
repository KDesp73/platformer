#include "builder.h"
#include <string.h>
#include "config.h"
#include "game.h"
#include "raylib.h"
#include "raymath.h"
#include "textures.h"
#include "utils.h"
#include <stdlib.h>
#include <time.h>

#define ROWS SCREEN_HEIGHT / CELL_SIZE
#define COLS SCREEN_WIDTH / CELL_SIZE


typedef enum {
    SELECTION_PLAYER,
    SELECTION_PLATFORM_START,
    SELECTION_PLATFORM_END,
    SELECTION_DOOR
} Selection;

Cstr selection_to_string(Selection selection){
    switch (selection) {
    case SELECTION_PLAYER:
        return "Player";
    case SELECTION_PLATFORM_START:
        return "Platform Start";
    case SELECTION_PLATFORM_END:
        return "Platform End";
    case SELECTION_DOOR:
        return "Door";
    default:
        return "None";
    }
}

void draw_grid(){
    Color color = GRAY;

    for(size_t y = 0; y < SCREEN_HEIGHT; ++y){
        DrawLineV((Vector2) {0, y*CELL_SIZE}, (Vector2){SCREEN_WIDTH, y*CELL_SIZE}, color);
    }
    for(size_t x = 0; x < SCREEN_WIDTH; ++x){
        DrawLineV((Vector2) {x * CELL_SIZE, 0}, (Vector2) {x*CELL_SIZE, SCREEN_HEIGHT}, color);
    }
}

#define MAX_PLATFORMS 200

typedef struct {
    Vector2 start;
    Vector2 end;
} BuilderPlatform;

typedef struct {
    Vector2 player;
    Vector2 door;
    BuilderPlatform* platforms;
    size_t platforms_count;
} Builder;


void place(Vector2 position, Vector2 size, Texture2D texture){
    DrawTextureRec(texture, (Rectangle) {0,0, size.x, size.y}, Vector2Scale(position, CELL_SIZE), WHITE);
}

void place_player(Vector2 position, Textures textures){
    place(position, PLAYER_SIZE, textures.items[PLAYER]);
}

void place_door(Vector2 position, Textures textures){
    place(position, DOOR_SIZE, textures.items[DOOR]);
}

void place_platform_tile(Vector2 position, Textures textures){
    place(position, (Vector2) {CELL_SIZE, CELL_SIZE}, textures.items[PLATFORM]);
}
void place_platform(Vector2 start, Vector2 end, Textures textures){
    if(start.x > end.x){
        Vector2 temp = start;
        start = end;
        end = temp;
    }

    for(size_t i = start.x; i <= end.x; ++i){
        place_platform_tile((Vector2) {i, start.y}, textures);
    }
}


void reset_player(Builder* builder){
    builder->player.x = -1;
    builder->player.y = -1;
}

void reset_door(Builder* builder){
    builder->door.x = -1;
    builder->door.y = -1;
}

void reset_current_platform(BuilderPlatform* platform){
    platform->start = (Vector2) {0, 0};
    platform->end = (Vector2) {0, 0};
}
int is_player_reset(Builder builder){
    return builder.player.x == -1 ||
        builder.player.y == -1;
}
int is_door_reset(Builder builder){
    return builder.door.x == -1 ||
        builder.door.y == -1;
}

int is_platform_reset(BuilderPlatform platform){
    return platform.start.x == 0 ||
        platform.start.y == 0 ||
        platform.end.x == 0 ||
        platform.end.y == 0;
}

long map(long x, long in_min, long in_max, long out_min, long out_max){
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


#define MOUSE_POSITION \
    (Vector2) { (float) Clamp(map(GetMouseX(), 0, SCREEN_WIDTH, 0, SCREEN_WIDTH / CELL_SIZE), 0, SCREEN_WIDTH), (float) Clamp(map(GetMouseY(), 0, SCREEN_HEIGHT, 0, SCREEN_HEIGHT / CELL_SIZE), 0, SCREEN_HEIGHT) }

Vector2 expand_mouse_position(Vector2 position){
    return (Vector2) {position.x * CELL_SIZE, position.y * CELL_SIZE};
}

void add_platform_to_builder(BuilderPlatform platform, Builder* builder){
    if(builder->platforms_count >= MAX_PLATFORMS){
        ERRO("Max platform count reached");
        return;
    }
    
    builder->platforms[builder->platforms_count++] = platform;
}

void place_builder_platforms(Builder builder, Textures textures){
    for(size_t i = 0; i < builder.platforms_count; ++i){
        BuilderPlatform p = builder.platforms[i];
        place_platform(p.start, p.end, textures);
    }
}

Cstr player_to_string(Vector2 position){
    return TextFormat("player %.0f %.0f", position.x, position.y);
}

Cstr door_to_string(Vector2 position){
    return TextFormat("door %.0f %.0f", position.x, position.y);
}

Cstr platform_to_string(BuilderPlatform platform){
    Vector2 left;
    if(platform.start.x <= platform.end.y) left = platform.start;
    else if(platform.start.x > platform.end.y) left = platform.end;
    return TextFormat("platform %.0f %.0f %.0f", left.x, left.y, fabsf(platform.start.x - platform.end.x));
}

void builder(Cstr creator){
    Textures textures = load_textures(
        "assets/images/jess-30x50.png",
        "assets/images/door-50x80.png",
        "assets/images/wood-25x25.png",
        NULL // Teriminate the list
    );

    Builder builder = {0};
    reset_door(&builder);
    reset_player(&builder);
    builder.platforms = (BuilderPlatform*) malloc(sizeof(BuilderPlatform) * MAX_PLATFORMS);

    BuilderPlatform current_platform = {
        .start = (Vector2) {-1, -1},
        .end = (Vector2) {-1, -1},
    };

    int selected = -1;
    while(!WindowShouldClose()){
        if(selected != SELECTION_PLATFORM_END && IsKeyPressed(KEY_P)) selected = SELECTION_PLAYER;
        if(IsKeyPressed(KEY_F)) selected = SELECTION_PLATFORM_START;
        if(selected != SELECTION_PLATFORM_END && IsKeyPressed(KEY_D)) selected = SELECTION_DOOR;

        if(IsKeyPressed(KEY_R)){
            reset_player(&builder);
            reset_door(&builder);
            builder.platforms = (BuilderPlatform*) malloc(sizeof(BuilderPlatform) * MAX_PLATFORMS);
            builder.platforms_count = 0;
        }

        if(IsKeyPressed(KEY_S)){
            add_platform_to_builder((BuilderPlatform) {.start = {-1, -1}, .end = {-1, -1 }}, &builder);

            Cstr player_text = player_to_string(builder.player);
            Cstr door_text = door_to_string(builder.door);

            char buffer[1024];

            buffer[0] = '\0';
            strcat(buffer, CONCAT("creator ", creator, "\n"));
            strcat(buffer, CONCAT(player_text, "\n"));
            strcat(buffer, CONCAT(door_text, "\n"));

            for(size_t i = 0; i < builder.platforms_count; ++i){
                strcat(buffer, platform_to_string(builder.platforms[i]));
                if(i < builder.platforms_count - 1)
                    strcat(buffer, "\n");
            }

            SaveFileText(CONCAT("assets/levels/level-", creator, "-", get_current_timestamp(), ".txt"), buffer);
            exit(0);
        }

        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
            switch (selected) {
                case SELECTION_PLAYER:
                    builder.player = (MOUSE_POSITION);
                    break;
                case SELECTION_DOOR:
                    builder.door = (MOUSE_POSITION);
                    break;
                case SELECTION_PLATFORM_START:
                    current_platform.start = (MOUSE_POSITION);
                    selected = SELECTION_PLATFORM_END;
                    break;
                case SELECTION_PLATFORM_END:
                    current_platform.end = (MOUSE_POSITION);
                    add_platform_to_builder(current_platform, &builder);
                    reset_current_platform(&current_platform);
                    selected = SELECTION_PLATFORM_START;
                    break;
                default:
                    break;
            }
        }

        BeginDrawing();

        ClearBackground(GetColor(0x181818FF));
        draw_grid();
        DrawText(TextFormat("Selected: %s", selection_to_string(selected)), 20, 20, 30, WHITE); 
        DrawText(TextFormat("Mouse: x%.0f  y%.0f", MOUSE_POSITION.x, MOUSE_POSITION.y), 20, 60, 30, WHITE); 

        if(!is_player_reset(builder)){
            place_player(builder.player, textures);
        }

        if(!is_door_reset(builder)){
            place_door(builder.door, textures);
        }

        if(selected == SELECTION_PLATFORM_END){
            place_platform_tile(current_platform.start, textures);
        }
        if(!is_platform_reset(current_platform)){
            place_platform(current_platform.start, current_platform.end, textures);
        }
    
        place_builder_platforms(builder, textures);

        EndDrawing();
    }
}
