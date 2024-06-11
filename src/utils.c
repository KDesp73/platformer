#include "utils.h"
#include "level.h"
#include <time.h>


char* get_current_timestamp() {
    // Get the current time
    time_t current_time = time(NULL);
    if (current_time == ((time_t)-1)) {
        perror("Failed to get the current time");
        return NULL;
    }

    // Convert the time to a readable format
    struct tm *local_time = localtime(&current_time);
    if (local_time == NULL) {
        perror("Failed to convert the current time");
        return NULL;
    }

    // Allocate memory for the time string
    char *time_string = (char*) malloc(100 * sizeof(char));
    if (time_string == NULL) {
        perror("Failed to allocate memory for the time string");
        return NULL;
    }

    // Format the time as a string
    if (strftime(time_string, 100, "%Y-%m-%d %H:%M:%S", local_time) == 0) {
        fprintf(stderr, "Failed to format the time string");
        free(time_string);
        return NULL;
    }

    return time_string;
}

void copy_vector2(Vector2* dest, const Vector2 src){
    dest->x = src.x;
    dest->y = src.y;
}

void copy_color(Color* dest, const Color src){
    dest->a = src.a;
    dest->r = src.r;
    dest->g = src.g;
    dest->b = src.b;
}

void copy_texture2D(Texture2D* dest, const Texture2D src){
    dest->height = src.height;
    dest->width = src.width;
    dest->format = src.format;
    dest->mipmaps = src.mipmaps;
    dest->id = src.id;
}

void copy_player(Player* dest, const Player src){
    copy_vector2(&dest->position, src.position);
    copy_vector2(&dest->size, src.size);
    copy_color(&dest->color, src.color);
    copy_vector2(&dest->velocity, src.velocity);
    dest->is_grounded = src.is_grounded;
    copy_texture2D(dest->sprite, *src.sprite);
}


void clean(Levels* levels, Game* game){
    for(size_t i = 0; i < levels->count; ++i){
        free_platforms(levels->items[i]->platforms);
    }
    unload_textures(game->textures);

    EndMode2D();
    CloseWindow();
}