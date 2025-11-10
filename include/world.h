#ifndef WORLD_H
#define WORLD_H

#include "raylib.h"
#include <stdbool.h>

#define MAX_LANES          12
#define MAX_CARS_PER_LANE  8
#define NUM_CAR_TEXTURES   3

typedef struct {
    Rectangle box;
    float     baseSpeed;     // velocidade base do carro (px/s)
    int       dir;           // -1 esquerda, +1 direita
    bool      active;        // se está em uso
    int       textureIndex;  // opcional (se usar texturas)
} Car;

typedef struct {
    float yTop;                        // topo (y) da faixa no mundo
    bool  isRoad;                      // true = rua, false = gramado/calçada
    int   carCount;
    Car   cars[MAX_CARS_PER_LANE];
} Lane;

typedef struct {
    int       laneCount;
    float     tile;                    // tamanho do “bloco”/grade
    float     worldWidth;              // largura lógica do mundo (px)

    // Texturas (opcional usar); se não usar, ignore no .c
    Texture2D texPavement;
    Texture2D texRoad;
    Texture2D texCar[NUM_CAR_TEXTURES];

    Lane lanes[MAX_LANES];
} World;

// ========== API ==========
void World_Init   (World *w, float worldWidth, float tile);
void World_Update (World *w, float dt, float difficulty);

// Desenha o mundo dentro do retângulo visível (mundo) — use visibleWorldRect.x/y como “offset”
void World_Draw   (const World *w, Rectangle visibleWorldRect);

bool World_CheckCollision      (const World *w, Rectangle player);
bool World_CheckCollisionInset (const World *w, Rectangle player, float inset);

// Utilidades expostas (se precisar no .c de fora)
float randf(float a, float b);

void DrawTextureTiledCompat(
    Texture2D tex,
    Rectangle src,
    Rectangle dest,
    Vector2   origin,
    float     rotation,
    float     scale,
    Color     tint
);

#endif // WORLD_H
