#ifndef SETTINGS_H
#define SETTINGS_H

#define GAME_NAME "Platformer"
#define VERSION_MAJOR 0
#define VERSION_MINOR 0
#define VERSION_PATCH 6
#define VERSION_TAG "demo"
#define VERSION "0.0.6-demo"

#define FPS 60
#define DT GetFrameTime()
#define MIN_SCREEN_WIDTH 1600
#define MIN_SCREEN_HEIGHT 900
#define SCREEN_WIDTH GetScreenWidth()
#define SCREEN_HEIGHT GetScreenHeight()


typedef struct {
    int is_fullscreen;
} Settings;

#endif // SETTINGS_H
