#include "graphics.h"

void DrawCenteredText(Cstr text, int y, int fontSize, Color color){
    int size = MeasureText(text, fontSize);
    DrawText(text, GetScreenWidth() / 2.0f - size / 2.0f, y - fontSize / 2.0f, fontSize, color);
}
