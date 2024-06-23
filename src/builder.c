#include "builder.h"
#include <math.h>
#include <string.h>
#include "extern/clib.h"
#include "config.h"
#include "entities.h"
#include "game.h"
#include "level.h"
#include "raylib.h"
#include "raymath.h"
#include "textures.h"
#include "utils.h"
#include "ui.h"
#include <stdlib.h>
#include <time.h>

typedef enum {
    KEY_SAVE = KEY_S,
    KEY_TEST = KEY_T,
    KEY_DOOR = KEY_D,
    KEY_PLAYER = KEY_P,
    KEY_PLATFORM = KEY_F,
    KEY_GHOST = KEY_G,
    KEY_RESET = KEY_R,
    KEY_QUIT = KEY_Q,
    KEY_DEBUG = KEY_APOSTROPHE,
} BuilderKeys;


typedef enum {
    SELECTION_PLAYER,
    SELECTION_PLATFORM_START,
    SELECTION_PLATFORM_END,
    SELECTION_DOOR,
    SELECTION_GHOST
} Selection;

Cstr selection_to_string(Selection selection)
{
    switch (selection) {
    case SELECTION_PLAYER:
        return "Player";
    case SELECTION_PLATFORM_START:
        return "Platform Start";
    case SELECTION_PLATFORM_END:
        return "Platform End";
    case SELECTION_DOOR:
        return "Door";
    case SELECTION_GHOST:
        return "Ghost";
    default:
        return "None";
    }
}

#define MAX_PLATFORMS 200
#define MAX_GHOSTS 20

typedef struct {
    Vector2 start;
    Vector2 end;
} BuilderPlatform;

typedef struct {
    Vector2 player;
    Vector2 door;
    BuilderPlatform* platforms;
    GhostCollection ghosts;
    size_t platforms_count;
} Builder;


void place(Vector2 position, Vector2 size, Texture2D texture, float scale)
{
    DrawTextureRec(texture, (Rectangle) {0,0, size.x, size.y}, Vector2Scale(position, BASE * scale), WHITE);
}

void place_player(Vector2 position, Textures textures, float scale)
{
    draw_player((Player){
        .position = Vector2Scale(position, CELL_SIZE(scale)),
        .size = PLAYER_SIZE(scale),
        .color = PLAYER_COLOR,
        .sprite = &textures.items[PLAYER]
    });
}

void place_door(Vector2 position, Textures textures, float scale)
{
    draw_door((Door){
        .position = Vector2Scale(position, CELL_SIZE(scale)),
        .size = DOOR_SIZE(scale),
        .color = DOOR_COLOR,
        .sprite = &textures.items[DOOR]
    });
}

void place_platform_tile(Vector2 position, Textures textures, float scale)
{
    place(position, (Vector2) {BASE * scale, BASE * scale}, textures.items[PLATFORM], scale);
}

void place_platform(Vector2 start, Vector2 end, Textures textures, float scale)
{
    Vector2 left = (start.x >= end.x) ? end : start;
    for(size_t i = 0; i < fabsf(start.x - end.x) + 1; i++){
        Vector2 tile_position = {
            .x = left.x + i,
            .y = start.y
        };

        place_platform_tile(tile_position, textures, scale);
    }
}

void place_ghost(Ghost ghost, float scale)
{
    ghost.position = Vector2Scale(ghost.position, CELL_SIZE(scale));
    draw_ghost(ghost);
}

void reset_player(Builder* builder)
{
    builder->player.x = -1;
    builder->player.y = -1;
}

void reset_door(Builder* builder){
    builder->door.x = -1;
    builder->door.y = -1;
}

void reset_current_platform(BuilderPlatform* platform)
{
    platform->start = (Vector2) {0, 0};
    platform->end = (Vector2) {0, 0};
}

int is_player_reset(Builder builder)
{
    return builder.player.x == -1 ||
        builder.player.y == -1;
}

int is_door_reset(Builder builder)
{
    return builder.door.x == -1 ||
        builder.door.y == -1;
}

int is_platform_reset(BuilderPlatform platform)
{
    return platform.start.x == 0 ||
        platform.start.y == 0 ||
        platform.end.x == 0 ||
        platform.end.y == 0;
}


Vector2 expand_mouse_position(Vector2 position, float scale)
{
    return (Vector2) {position.x * BASE * scale, position.y * BASE * scale};
}

void add_platform_to_builder(BuilderPlatform platform, Builder* builder)
{
    if(builder->platforms_count >= MAX_PLATFORMS){
        ERRO("Max platform count reached");
        return;
    }
    
    builder->platforms[builder->platforms_count++] = platform;
}

void place_builder_platforms(Builder builder, Textures textures, float scale)
{
    for(size_t i = 0; i < builder.platforms_count; ++i){
        BuilderPlatform p = builder.platforms[i];
        place_platform(p.start, p.end, textures, scale);
    }
}

void place_builder_ghosts(Builder builder, Textures textures, float scale)
{
    for(size_t i = 0; i < builder.ghosts.count; ++i){
        builder.ghosts.items[i]->sprite = &textures.items[GHOST];
        place_ghost(*builder.ghosts.items[i], scale);
    }
}

Cstr player_to_string(Vector2 position)
{
    return TextFormat("player %.2f %.2f", position.x, position.y);
}

Cstr scale_to_string(float scale)
{
    return TextFormat("scale %.1f", scale);
}

Cstr door_to_string(Vector2 position)
{
    return TextFormat("door %.2f %.2f", position.x, position.y);
}

Cstr platform_to_string(BuilderPlatform platform)
{
    Vector2 left;
    if(platform.start.x <= platform.end.x) copy_vector2(&left, platform.start);
    else if(platform.start.x > platform.end.x) copy_vector2(&left, platform.end);
    return TextFormat("platform %.2f %.2f %.2f", left.x, left.y, fabsf(platform.end.x - platform.start.x) + 1);
}

Cstr ghost_to_string(Ghost ghost)
{
    return TextFormat("ghost %.2f %.2f", ghost.position.x, ghost.position.y);
}

char* export_level(Builder builder, float scale, Cstr creator)
{
    add_platform_to_builder((BuilderPlatform) {.start = {-1, -1}, .end = {-1, -1 }}, &builder);

    Cstr player_text = player_to_string(builder.player);
    Cstr door_text = door_to_string(builder.door);
    Cstr scale_text = scale_to_string(scale);

    char* buffer = (char*) malloc(1024);

    buffer[0] = '\0';
    strcat(buffer, CONCAT("creator ", creator, "\n"));
    strcat(buffer, CONCAT(scale_text, "\n"));
    strcat(buffer, CONCAT(player_text, "\n"));
    strcat(buffer, CONCAT(door_text, "\n"));

    for(size_t i = 0; i < builder.ghosts.count; ++i){
        strcat(buffer, ghost_to_string(*builder.ghosts.items[i]));
        if(i < builder.ghosts.count - 1)
            strcat(buffer, "\n");
    }
    if(builder.ghosts.count > 0)
        strcat(buffer, "\n");
    for(size_t i = 0; i < builder.platforms_count; ++i){
        strcat(buffer, platform_to_string(builder.platforms[i]));
        if(i < builder.platforms_count - 1)
            strcat(buffer, "\n");
    }

    return buffer;
}

void builder(Cstr creator, float scale)
{
    Textures textures = load_textures(
        "assets/images/jess-30x50.png",
        "assets/images/door-50x75.png",
        "assets/images/wood-25x25.png",
        "assets/images/ghost-50x50.png",
        "assets/images/key-25x25.png",
        NULL // Teriminate the list
    );

    Builder builder = {0};
    reset_door(&builder);
    reset_player(&builder);
    builder.platforms = (BuilderPlatform*) malloc(sizeof(BuilderPlatform) * MAX_PLATFORMS);
    builder.ghosts = allocate_ghost_collection(MAX_GHOSTS);

    BuilderPlatform current_platform = {
        .start = (Vector2) {-1, -1},
        .end = (Vector2) {-1, -1},
    };

    int selected = -1;
    while(!WindowShouldClose()){
        if(selected != SELECTION_PLATFORM_END && IsKeyPressed(KEY_PLAYER)) selected = SELECTION_PLAYER;
        if(IsKeyPressed(KEY_PLATFORM)) selected = SELECTION_PLATFORM_START;
        if(selected != SELECTION_PLATFORM_END && IsKeyPressed(KEY_DOOR)) selected = SELECTION_DOOR;
        if(IsKeyPressed(KEY_GHOST)) selected = SELECTION_GHOST;

        if(IsKeyPressed(KEY_RESET)){
            reset_player(&builder);
            reset_door(&builder);
            builder.platforms = (BuilderPlatform*) malloc(sizeof(BuilderPlatform) * MAX_PLATFORMS);
            builder.platforms_count = 0;
            builder.ghosts = allocate_ghost_collection(MAX_GHOSTS);
            builder.ghosts.count = 0;
        }

        if(IsKeyPressed(KEY_SAVE)){
            if(is_player_reset(builder)) ERRO("Did not add the player");
            else if(is_door_reset(builder)) ERRO("Did not add the door");
            else {
                Cstr file = CONCAT("assets/levels/level-", creator, "-", get_current_timestamp(), ".txt");
                SaveFileText(file, export_level(builder, scale, creator));
                INFO("File '%s' saved successfully", file);
                exit(0);
            }
        }

        if(IsKeyPressed(KEY_DEBUG)){
            while(!IsKeyPressed(KEY_QUIT) && !WindowShouldClose()){
                BeginDrawing();
                ClearBackground(GetColor(0x181818FF));

                for (size_t i = 0; i < builder.platforms_count; ++i) {
                    BuilderPlatform platform = builder.platforms[i];
                    Cstr platform_str = platform_to_string(platform);
                    DrawText(platform_str, 20, 20 + 40*i, 30, PLATFORM_COLOR);
                }    


                DrawText(player_to_string(builder.player), 600, 20, 30, PLAYER_COLOR);
                DrawText(door_to_string(builder.door), 600, 60, 30, DOOR_COLOR);
            
                for (size_t i = 0; i < builder.ghosts.count; ++i){
                    Cstr ghost_str = ghost_to_string(*builder.ghosts.items[i]);
                    DrawText(ghost_str, 900, 20 + 40*i, 30, GHOST_COLOR);
                }

                EndDrawing(); 
            }
        }

        if(IsKeyPressed(KEY_TEST)){
            Cstr level_str = export_level(builder, scale, creator);
            Game game = {
                .level = 0,
                .textures = load_textures(
                        "assets/images/jess-30x50.png",
                        "assets/images/door-50x75.png",
                        "assets/images/wood-25x25.png",
                        "assets/images/ghost-50x50.png",
                        NULL // Teriminate the list
                        ),
            };
            game.current_level = (Level){0};
            game.current_level.player = (Player) {
                .status = PLAYER_STATUS_IDLE,
                    .sprite = &game.textures.items[PLAYER],
                    .is_grounded = false,
                    .velocity = {0},
                    .position = {0},
                    .size = PLAYER_SIZE(1.0f),
                    .color = PLAYER_COLOR
            };
            game.current_level.ghosts = builder.ghosts;
            printf("%s\n", level_str);
            Level* level = load_level(level_str, game.textures);
            copy_vector2(&game.current_level.player.position, level->player.position);
            copy_vector2(&game.current_level.player.size, level->player.size);
            while(!IsKeyPressed(KEY_QUIT) && !WindowShouldClose()){
                BeginDrawing();
                run_level(&game);
                EndDrawing();
            }
            free(level);
        }

        BeginDrawing();
        ClearBackground(GetColor(0x181818FF));
        draw_grid(scale, 0, 0);

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
                    current_platform.end = (Vector2) { MOUSE_POSITION(scale).x, MOUSE_POSITION(scale).y };
                    add_platform_to_builder(current_platform, &builder);
                    reset_current_platform(&current_platform);
                    selected = SELECTION_PLATFORM_START;
                    break;
                case SELECTION_GHOST:
                    add_ghost(make_ghost((Vector2) { MOUSE_POSITION(scale).x, MOUSE_POSITION(scale).y }, scale), &builder.ghosts);
                    break;
                default:
                    break;
            }
        }

        if(!is_door_reset(builder)){
            place_door(builder.door, textures,scale);
        }

        if(selected == SELECTION_PLATFORM_END){
            place_platform_tile(current_platform.start, textures, scale);
            Vector2 start = Vector2Scale(current_platform.start, CELL_SIZE(scale));
            DrawLineEx((Vector2) {start.x + CELL_SIZE(scale) / 2, start.y + CELL_SIZE(scale) / 2}, (Vector2) {GetMousePosition().x, current_platform.start.y * CELL_SIZE(scale) + CELL_SIZE(scale) / 2}, 10.0f, YELLOW);
        }
        if(!is_platform_reset(current_platform)){
            place_platform(current_platform.start, current_platform.end, textures, scale);
        }
    
        place_builder_platforms(builder, textures, scale);
        place_builder_ghosts(builder, textures, scale);

        if(!is_player_reset(builder)){
            place_player(builder.player, textures, scale);
        }

        DrawText(TextFormat("Selected: %s", selection_to_string(selected)), 20, 20, 30, YELLOW); 
        show_coords_at_cursor(MOUSE_POSITION(scale), 20, WHITE);
        // DrawText(TextFormat("Mouse: x%.0f  y%.0f", MOUSE_POSITION(scale, camera).x, MOUSE_POSITION(scale, camera).y), 20, 60, 30, WHITE); 
        // DrawText(TextFormat("Mouse Real: x%.0f  y%.0f", GetMousePosition().x, GetMousePosition().y), 20, 100, 30, WHITE); 

        EndDrawing();
    }
}
