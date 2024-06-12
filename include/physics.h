#ifndef PHYSICS_H
#define PHYSICS_H
#include "entities.h"

#define GRAVITY(scale) (800.0f * scale)
#define JUMP_FORCE(scale) (400.0f * scale)
#define PLAYER_STEP(scale) (400.0f * scale)
#define FRICTION(scale) (0.8f * (1.0f/scale))

int check_platform_collision(Player* player, Platform* platform);
void resolve_platform_collision(Player* player, Platform* platform);
void check_and_resolve_platform_collisions(Player* player, PlatformCollection platforms);
void update_player(Player* player, float windowWidth, float windowHeight, float scale);
int check_door_collision(Player* player, Door door);

#endif // PHYSICS_H
