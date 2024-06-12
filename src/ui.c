#include "ui.h"
#include "utils.h"
#include "config.h"
#include "game.h"
#include "raylib.h"

void DrawCenteredText(Cstr text, int x, int y, int fontSize, Color color){
    int size = MeasureText(text, fontSize);
    DrawText(text, x / 2.0f - size / 2.0f, y/2.0f - fontSize / 2.0f, fontSize, color);
}


void draw_grid(float scale, int draw_coords, int draw_center){
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

Vector2 mouse_cell_position(float scale) {
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

void show_mouse_coords_at_cursor(int font_size, Color color){
    show_coords_at_cursor(GetMousePosition(), font_size, color);
}

void show_coords_at_cursor(Vector2 coords, int font_size, Color color){
    Cstr text = TextFormat("%.2f %.2f", coords.x, coords.y);
    DrawText(text, GetMousePosition().x + 20, GetMousePosition().y, font_size, color);
}
