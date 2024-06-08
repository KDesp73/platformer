#ifndef SETTINGS_H
#define SETTINGS_H

#define GAME_NAME "Platformer"
#define VERSION_MAJOR 0
#define VERSION_MINOR 0
#define VERSION_PATCH 1
#define VERSION "0.0.1"

#define FPS 60
#define DT GetFrameTime()
#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 900


typedef struct {
    int is_fullscreen;
} Settings;

#endif // SETTINGS_H
