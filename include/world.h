#ifndef WORLD_H
#define WORLD_H
#include "raylib.h"
#include <stdbool.h>

#define MAX_LANES 12
#define MAX_CARS_PER_LANE 8

typedef struct {
    Rectangle box;
    float speed;
    int dir;
    bool active;
} Car;

typedef struct {
    int y;
    bool isRoad;
    int carCount;
    Car cars[MAX_CARS_PER_LANE];
} Lane;

typedef struct {
    int laneCount;
    float tile;
    Lane lanes[MAX_LANES];
} World;

void World_Init(World *w, int screenW, int screenH, float tile);
void World_Update(World *w, float dt, int screenW);
void World_Draw(const World *w, Vector2 cameraOffset);
bool World_CheckCollision(const World *w, Rectangle player);

#endif