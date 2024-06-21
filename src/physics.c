#include "physics.h"
#include "config.h"
#include "raylib.h"
#include "raymath.h"
#include <assert.h>
#define CLIB_IMPLEMENTATION
#include "clib.h"
#include "entities.h"

#define PADDING 10 // Should scale

#define PLAYER_TOP(player) (player->position.y + PADDING)
#define PLAYER_BOTTOM(player) (player->position.y + player->size.y)
#define PLAYER_LEFT(player) (player->position.x + PADDING)
#define PLAYER_RIGHT(player) (player->position.x + player->size.x - PADDING)

#define PLATFORM_TOP(platform) (platform->start.y - platform->thickness / 2.0f)
#define PLATFORM_BOTTOM(platform) (platform->start.y + platform->thickness / 2.0f)
#define PLATFORM_LEFT(platform) (platform->start.x)
#define PLATFORM_RIGHT(platform) (platform->start.x + platform->length)

#define DOOR_TOP(door) (door.position.y)
#define DOOR_BOTTOM(door) (door.position.y + door.size.y)
#define DOOR_LEFT(door) (door.position.x)
#define DOOR_RIGHT(door) (door.position.x + door.size.x)

#define PLAYER_IS_INSIDE_PLATFORM(player, platform) \
    (PLAYER_RIGHT(player) >= PLATFORM_LEFT(platform) && PLAYER_LEFT(player) <= PLATFORM_RIGHT(platform))

int check_door_collision(Player* player, Door door) {
    return (PLAYER_LEFT(player) <= DOOR_RIGHT(door)) &&
        (PLAYER_RIGHT(player) >= DOOR_LEFT(door)) && 
        (PLAYER_TOP(player) <= DOOR_BOTTOM(door)) &&
        (PLAYER_BOTTOM(player) >= DOOR_TOP(door));
}

int check_platform_collision(Player* player, Platform* platform) {
    assert(platform != NULL);
    assert(player != NULL);

    return (PLAYER_LEFT(player) <= PLATFORM_RIGHT(platform) &&
             PLAYER_RIGHT(player) >= PLATFORM_LEFT(platform) &&
             PLAYER_TOP(player) <= PLATFORM_BOTTOM(platform) &&
             PLAYER_BOTTOM(player) >= PLATFORM_TOP(platform));
}

void resolve_platform_collision(Player* player, Platform* platform) {
    float overlapTop = PLAYER_BOTTOM(player) - PLATFORM_TOP(platform);
    float overlapBottom = PLATFORM_BOTTOM(platform) - PLAYER_TOP(player);
    float overlapLeft = PLAYER_RIGHT(player) - PLATFORM_LEFT(platform);
    float overlapRight = PLATFORM_RIGHT(platform) - PLAYER_LEFT(player);
    
    // Determine the direction of collision
    float minOverlap = fmin(fmin(overlapTop, overlapBottom), fmin(overlapLeft, overlapRight));

    if (minOverlap == overlapTop) {
        // Player is above the platform
        player->position.y = PLATFORM_TOP(platform) - player->size.y;
        player->velocity.y = 0;
        player->is_grounded = true;
    } else if (minOverlap == overlapBottom) {
        // Player is below the platform
        player->position.y = PLATFORM_BOTTOM(platform);
        player->velocity.y = 0;
    } else if (minOverlap == overlapLeft) {
        // Player is colliding from the left
        player->position.x = PLATFORM_LEFT(platform) - player->size.x;
        player->velocity.x = 0;
    } else if (minOverlap == overlapRight) {
        // Player is colliding from the right
        player->position.x = PLATFORM_RIGHT(platform);
        player->velocity.x = 0;
    }
}

void check_and_resolve_platform_collisions(Player* player, PlatformCollection platforms) {
     if(PLAYER_BOTTOM(player) < GetScreenHeight())
        player->is_grounded = false;  // Reset grounding status
    for (size_t i = 0; i < platforms.count; ++i) {
        Platform* platform = platforms.items[i];
        if (check_platform_collision(player, platform)) {
            resolve_platform_collision(player, platform);
        }
    }
}

void update_player(Player* player, float windowWidth, float windowHeight, float scale) {
    if (player->is_grounded && IsKeyPressed(KEY_SPACE)) {
        player->velocity.y = -JUMP_FORCE(scale);
        player->is_grounded = false;
    }

    if (!player->is_grounded) {
        player->velocity.y += GRAVITY(scale) * DT;
    }

    player->position.y += player->velocity.y * DT;

    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
        player->velocity.x = -PLAYER_STEP(scale);
        player->status = RUNNING_LEFT;
    } else if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
        player->velocity.x = PLAYER_STEP(scale);
        player->status = RUNNING_RIGHT;
    } else {
        // Apply friction when no input is given
        player->velocity.x *= FRICTION(scale);
    }

    player->position.x += player->velocity.x * DT;

    // Clamp player position within the screen bounds
    player->position = Vector2Clamp(player->position, (Vector2){0}, (Vector2){windowWidth - player->size.x, windowHeight - player->size.y});

    if (player->position.y >= windowHeight - player->size.y) {
        player->position.y = windowHeight - player->size.y;
        player->velocity.y = 0;
        player->is_grounded = true;
    }
}

