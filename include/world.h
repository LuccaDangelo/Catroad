#ifndef WORLD_H
#define WORLD_H
#include "raylib.h"
#include <stdbool.h>

#define MAX_LANES 12
#define MAX_CARS_PER_LANE 8
#define NUM_CAR_TEXTURES 3

typedef struct {
    Rectangle box;
    float baseSpeed;     // era "speed" no seu header
    int dir;
    bool active;
    int textureIndex;    // usado no world.c
} Car;

typedef struct {
    float yTop;                  // era "int y" no seu header
    bool isRoad;
    int carCount;
    Car cars[MAX_CARS_PER_LANE];
} Lane;

typedef struct {
    int laneCount;
    float tile;
    float worldWidth;            // usado no world.c

    // Texturas usadas no world.c
    Texture2D texPavement;
    Texture2D texRoad;
    Texture2D texCar[NUM_CAR_TEXTURES];

    Lane lanes[MAX_LANES];
} World;

// Assinaturas como no world.c
void World_Init(World *w, float worldWidth, float tile);
void World_Update(World *w, float dt, float difficulty);
void World_Draw(const World *w, Rectangle visibleWorldRect);
bool World_CheckCollision(const World *w, Rectangle player);
bool World_CheckCollisionInset(const World *w, Rectangle player, float inset);

// Se quiser usar fora do .c, não deixe static:
float randf(float a, float b);
void DrawTextureTiledCompat(Texture2D tex,
                            Rectangle src,
                            Rectangle dest,
                            Vector2 origin,
                            float rotation,
                            float scale,
                            Color tint);

#endif
