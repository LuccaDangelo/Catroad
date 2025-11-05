#ifndef WORLD_H
#define WORLD_H
#include "raylib.h"
#include <stdbool.h>

#define MAX_LANES 12
#define MAX_CARS_PER_LANE 8

typedef struct {
    Rectangle box;
    float speed;     // pixels/s
    int dir;         // -1 esquerda, +1 direita
    bool active;
} Car;

typedef struct {
    int y;           // posição Y da faixa (topo da faixa)
    bool isRoad;     // true = tem carros; false = gramado
    int carCount;
    Car cars[MAX_CARS_PER_LANE];
} Lane;

typedef struct {
    int laneCount;
    float tile;      // tamanho de um “quadrado”
    Lane lanes[MAX_LANES];
} World;

void World_Init(World *w, int screenW, int screenH, float tile);
void World_Update(World *w, float dt, int screenW);
void World_Draw(const World *w);
bool World_CheckCollision(const World *w, Rectangle player);

#endif
