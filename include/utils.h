#ifndef UTILS_H
#define UTILS_H

#include "entities.h"
#include "game.h"
#include "level.h"
#include "raylib.h"


long map(long x, long in_min, long in_max, long out_min, long out_max);
char* get_current_timestamp();
void copy_vector2(Vector2* dest, const Vector2 src);
void copy_color(Color* dest, const Color src);
void copy_texture2D(Texture2D* dest, const Texture2D src);
void copy_player(Player* dest, const Player src);
void copy_level(Level* dest, const Level src);
void print_vector2(Vector2 vec, Cstr tag);
Vector2 NormalizeVector2(Vector2 v);

#define COPY_PLAYER(dest, src) \
    do { \
        dest.position = src.position; \
        dest.size = src.size; \
        dest.sprite = src.sprite; \
        dest.color = src.color; \
        dest.status = src.status; \
        dest.velocity = src.velocity; \
        dest.is_grounded = src.is_grounded; \
    } while(0);

void clean_and_exit(Levels* levels, Game* game);

#define SET_FULLSCREEN(x) \
    do { \
        if(x) { \
            int display = GetCurrentMonitor(); \
            SetWindowSize(GetMonitorWidth(display), GetMonitorHeight(display)); \
        } else { \
            SetWindowSize(MIN_SCREEN_WIDTH, MIN_SCREEN_HEIGHT); \
        } \
        ToggleFullscreen(); \
    } while(0); 


#endif // UTILS_H
