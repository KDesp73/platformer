#ifndef UI_H
#define UI_H

#include "clib.h"
#include "raylib.h"

void DrawCenteredText(Cstr text, int x, int y, int fontSize, Color color);
void draw_grid(float scale, int draw_coords, int draw_center);

Vector2 mouse_cell_position(float scale);

#define MOUSE_POSITION(scale) \
    mouse_cell_position(scale)

#endif // UI_H
