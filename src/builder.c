#include "builder.h"
#include <math.h>
#include <string.h>
#include "clib.h"
#include "config.h"
#include "entities.h"
#include "game.h"
#include "raylib.h"
#include "raymath.h"
#include "textures.h"
#include "utils.h"
#include "ui.h"
#include <stdlib.h>
#include <time.h>



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


void place(Vector2 position, Vector2 size, Texture2D texture, float scale){
    DrawTextureRec(texture, (Rectangle) {0,0, size.x, size.y}, Vector2Scale(position, BASE * scale), WHITE);
}

void place_player(Vector2 position, Textures textures, float scale){
    draw_player((Player){
        .position = Vector2Scale(position, CELL_SIZE(scale)),
        .size = PLAYER_SIZE(scale),
        .color = PLAYER_COLOR,
        .sprite = &textures.items[PLAYER]
    });
}

void place_door(Vector2 position, Textures textures, float scale){
    draw_door((Door){
        .position = Vector2Scale(position, CELL_SIZE(scale)),
        .size = DOOR_SIZE(scale),
        .color = DOOR_COLOR,
        .sprite = &textures.items[DOOR]
    });
}

void place_platform_tile(Vector2 position, Textures textures, float scale){
    place(position, (Vector2) {BASE * scale, BASE * scale}, textures.items[PLATFORM], scale);
}

void place_platform(Vector2 start, Vector2 end, Textures textures, float scale){
    draw_platform(
        (Platform) { 
            .sprite = &textures.items[PLATFORM], 
            .thickness = PLATFORM_HEIGHT(scale) * scale, 
            .start = Vector2Scale(start, CELL_SIZE(scale)),
            .color = PLATFORM_COLOR,
            .length = fabsf(start.x - end.x) * CELL_SIZE(scale),
        }, 
        textures.items[PLATFORM]
    );
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


Vector2 expand_mouse_position(Vector2 position, float scale){
    return (Vector2) {position.x * BASE * scale, position.y * BASE * scale};
}

void add_platform_to_builder(BuilderPlatform platform, Builder* builder){
    if(builder->platforms_count >= MAX_PLATFORMS){
        ERRO("Max platform count reached");
        return;
    }
    
    builder->platforms[builder->platforms_count++] = platform;
}

void place_builder_platforms(Builder builder, Textures textures, float scale){
    for(size_t i = 0; i < builder.platforms_count; ++i){
        BuilderPlatform p = builder.platforms[i];
        place_platform(p.start, p.end, textures, scale);
    }
}

Cstr player_to_string(Vector2 position){
    return TextFormat("player %.0f %.0f", position.x, position.y);
}

Cstr scale_to_string(float scale){
    return TextFormat("scale %.1f", scale);
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

void builder(Cstr creator, float scale){
    DEBU("scale: %f", scale);
    DEBU("Cell Size: %f", CELL_SIZE(scale));
    DEBU("Rows: %d", ROWS(scale));
    DEBU("Cols: %d", COLS(scale));
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
            Cstr scale_text = scale_to_string(scale);

            char buffer[1024];

            buffer[0] = '\0';
            strcat(buffer, CONCAT("creator ", creator, "\n"));
            strcat(buffer, CONCAT(scale_text, "\n"));
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
                    builder.player = (MOUSE_POSITION(scale));
                    break;
                case SELECTION_DOOR:
                    builder.door = (MOUSE_POSITION(scale));
                    break;
                case SELECTION_PLATFORM_START:
                    current_platform.start = (MOUSE_POSITION(scale));
                    selected = SELECTION_PLATFORM_END;
                    break;
                case SELECTION_PLATFORM_END:
                    current_platform.end = (MOUSE_POSITION(scale));
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
        draw_grid(scale, 0, 0);


        if(!is_door_reset(builder)){
            place_door(builder.door, textures,scale);
        }

        if(selected == SELECTION_PLATFORM_END){
            place_platform_tile(current_platform.start, textures, scale);
        }
        if(!is_platform_reset(current_platform)){
            place_platform(current_platform.start, current_platform.end, textures, scale);
        }
    
        place_builder_platforms(builder, textures, scale);

        if(!is_player_reset(builder)){
            place_player(builder.player, textures, scale);
        }

        DrawText(TextFormat("Selected: %s", selection_to_string(selected)), 20, 20, 30, YELLOW); 
        DrawText(TextFormat("Mouse: x%.0f  y%.0f", MOUSE_POSITION(scale).x, MOUSE_POSITION(scale).y), 20, 60, 30, WHITE); 
        // DrawText(TextFormat("Mouse Real: x%.0f  y%.0f", GetMousePosition().x, GetMousePosition().y), 20, 100, 30, WHITE); 

        EndDrawing();
    }
}
