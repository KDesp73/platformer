#include "ui.h"
#include "utils.h"
#include "config.h"
#include "game.h"
#include "raylib.h"

void DrawCenteredText(Cstr text, Vector2 offset, int width, int y, int fontSize, Color color)
{
    int size = MeasureText(text, fontSize);
    DrawText(text, offset.x + (width - size) / 2.0f, offset.y + (y - fontSize) / 2.0f, fontSize, color);
}


void draw_grid(float scale, int draw_coords, int draw_center)
{
    Color color = GRAY;

    for(size_t y = 0; y < SCREEN_HEIGHT; ++y){
        DrawLineV((Vector2) {0, y*BASE * scale}, (Vector2){SCREEN_WIDTH, y*BASE * scale}, color);
    }
    for(size_t x = 0; x < SCREEN_WIDTH; ++x){
        DrawLineV((Vector2) {x * BASE * scale, 0}, (Vector2) {x*BASE * scale, SCREEN_HEIGHT}, color);
    }

    if(draw_coords){
        for(size_t row = 0; row <= ROWS(scale); row++){
            for(size_t col= 0; col <= COLS(scale); ++col){
                DrawText(TextFormat("%zu %zu", row, col), row * CELL_SIZE(scale) + CELL_SIZE(scale) / 2, col * CELL_SIZE(scale) + CELL_SIZE(scale) / 2, 10, WHITE);
            }
        }
    }

    if(draw_center){
        for(size_t row = 0; row <= ROWS(scale); row++){
            for(size_t col= 0; col <= COLS(scale); ++col){
                DrawCircle(row * CELL_SIZE(scale) + CELL_SIZE(scale) / 2, col * CELL_SIZE(scale) + CELL_SIZE(scale) / 2, 5.0f, RED);
            }
        }
        
    }
}

Vector2 mouse_cell_position(float scale) 
{
    float cellSize = CELL_SIZE(scale);

    // Map mouse coordinates to cell grid
    int x_cell_over = (int)map((float)GetMousePosition().x, 0, (float)SCREEN_WIDTH, 0, (float)(SCREEN_WIDTH / cellSize));
    x_cell_over = Clamp(x_cell_over, 0, SCREEN_WIDTH / cellSize - 1);

    int y_cell_over = (int)map((float)GetMousePosition().y, 0, (float)SCREEN_HEIGHT, 0, (float)(SCREEN_HEIGHT / cellSize));
    y_cell_over = Clamp(y_cell_over, 0, SCREEN_HEIGHT / cellSize - 1);

    Vector2 result;
    result.x = x_cell_over;
    result.y = y_cell_over;
    return result;
}

void show_mouse_coords_at_cursor(int font_size, Color color)
{
    show_coords_at_cursor(GetMousePosition(), font_size, color);
}

void show_coords_at_cursor(Vector2 coords, int font_size, Color color)
{
    Cstr text = TextFormat("%.2f %.2f", coords.x, coords.y);
    DrawText(text, GetMousePosition().x + 20, GetMousePosition().y, font_size, color);
}

void PressUpTooltip(Game* game)
{
    DrawText("^", game->current_level.door.position.x + game->current_level.door.size.x / 2 - MeasureText("^", 70) / 2.0f, game->current_level.door.position.y - 70/2.0f - 10, 70, WHITE);
}



int IsHover(Rectangle rec)
{
    return CheckCollisionPointRec(GetMousePosition(), rec);
}

Rectangle Button(float x, float y, int width, int height, Color color, Cstr text)
{
    Rectangle rec ={x - width/2.0f, y - height / 2.0f, width, height};
    DrawRectangleRounded(rec, 20, 10, color);
    DrawCenteredText(text, (Vector2) {x - width / 2.0f, y - height/2.0f}, width, height, 20, WHITE);

    return rec;
}

void HomePage()
{
    while(!WindowShouldClose()){
        BeginDrawing();
        ClearBackground(GetColor(0x181818FF));
        DrawCenteredText(GAME_NAME, (Vector2) {0}, SCREEN_WIDTH, SCREEN_HEIGHT - 200, 100, WHITE);

        Rectangle button = Button(SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT/ 2.0f + 400, 300, 100, RED, "PLAY");

        if(
            (IsHover(button) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) || 
            IsKeyPressed(KEY_ENTER)
        ){
            EndDrawing(); 
            return;
        }
        EndDrawing();
    }         
}


