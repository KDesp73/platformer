#ifndef PHYSICS_H
#define PHYSICS_H
#include "entities.h"

int check_collision(Player* player, Platform* platform);
void resolve_collision(Player* player, Platform* platform);
void check_and_resolve_collisions(Player* player, PlatformCollection platforms);
void update_player(Player* player, float windowWidth, float windowHeight);

#endif // PHYSICS_H
