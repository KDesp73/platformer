#ifndef UI_H
#define UI_H

#define CLIB_IMPLEMENTATION
#include "extern/clib.h"
#include "game.h"
#include "raylib.h"

void DrawCenteredText(Cstr text, Vector2 offset, int width, int y, int fontSize, Color color);
void draw_grid(float scale, int draw_coords, int draw_center);

// Vector2 mouse_cell_position(float scale);
Vector2 mouse_cell_position(float scale);

void show_mouse_coords_at_cursor(int font_size, Color color);
void show_coords_at_cursor(Vector2 coords, int font_size, Color color);

#define MOUSE_POSITION(scale) \
    mouse_cell_position(scale)

void PressUpTooltip(Game* game);




int IsHover(Rectangle rec);
Rectangle Button(float x, float y, int width, int height, Color color, Cstr text);
void HomePage();

#endif // UI_H
