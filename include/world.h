#ifndef WORLD_H
#define WORLD_H
#include "raylib.h"
#include <stdbool.h>

#define MAX_LANES 100
#define MAX_CARS_PER_LANE 10
#define NUM_CAR_TEXTURES 3 // Quantos modelos de carro vamos carregar

typedef struct {
    Rectangle box;      // posição no mundo
    float baseSpeed;
    int dir;            // -1 esquerda, +1 direita
    bool active;
    int textureIndex;   // Qual textura de carro usar (0, 1, 2...)
} Car;

typedef struct {
    float yTop;         // topo da faixa em coords de mundo (y cresce p/ baixo)
    bool isRoad;        // true = tem carros; false = gramado
    int carCount;
    Car cars[MAX_CARS_PER_LANE];
} Lane;

typedef struct {
    int laneCount;
    float tile;         // altura de cada faixa
    float worldWidth;
    Lane lanes[MAX_LANES];

    // --- NOSSAS TEXTURAS ---
    Texture2D texPavement; // Textura do "cimento"
    Texture2D texRoad;     // Textura da estrada
    Texture2D texCar[NUM_CAR_TEXTURES]; // Array de texturas de carro

} World;

void World_Init(World *w, float worldWidth, float tile);
void World_Update(World *w, float dt, float difficulty);
void World_Draw(const World *w, const Rectangle visibleWorldRect);
bool World_CheckCollision(const World *w, Rectangle player);
bool World_CheckCollisionInset(const World *w, Rectangle player, float inset);

#endif