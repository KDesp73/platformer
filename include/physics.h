#ifndef PHYSICS_H
#define PHYSICS_H
#include "entities.h"
#include "game.h"

#define GRAVITY(scale) (800.0f * scale)
#define JUMP_FORCE(scale) (400.0f * scale)
#define PLAYER_STEP(scale) (400.0f * scale)
#define GHOST_STEP(scale) (150.0f * scale)
#define GHOST_AGRO_RANGE(scale) (CELL_SIZE(scale) * 10)
#define FRICTION(scale) (0.8f * (scale >= 1.0f ? 1.0f/scale : scale))

int check_platform_collision(Player* player, Platform* platform);
void resolve_platform_collision(Player* player, Platform* platform);
void check_and_resolve_platform_collisions(Player* player, PlatformCollection platforms);
void update_player(Player* player, float windowWidth, float windowHeight, float scale);
int check_door_collision(Player* player, Door door);
void move_ghost(Ghost* ghost, Vector2 playerPos, float scale);
void move_ghosts(GhostCollection* ghosts, Vector2 playerPos, float scale);
int check_ghost_collision(Player* player, Ghost ghost);
int check_ghost_collisions(Player* player, GhostCollection ghosts);

#endif // PHYSICS_H
