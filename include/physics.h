#ifndef PHYSICS_H
#define PHYSICS_H
#include "entities.h"

int check_platform_collision(Player* player, Platform* platform);
void resolve_platform_collision(Player* player, Platform* platform);
void check_and_resolve_platform_collisions(Player* player, PlatformCollection platforms);
void update_player(Player* player, float windowWidth, float windowHeight);
int check_door_collision(Player* player, Door door);

#endif // PHYSICS_H
