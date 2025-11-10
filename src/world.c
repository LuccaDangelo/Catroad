#include "world.h"
#include "raylib.h"
#include <stdlib.h>

// randf é declarado no header
float randf(float a, float b) {
    return a + (float)GetRandomValue(0, 10000) / 10000.0f * (b - a);
}

void World_Init(World *w, float worldWidth, float tile) {
    w->tile       = tile;
    w->worldWidth = worldWidth;
    w->laneCount  = MAX_LANES;

    // Faixas de baixo pra cima em coordenadas de mundo
    for (int i = 0; i < w->laneCount; i++) {
        Lane *ln   = &w->lanes[i];
        ln->yTop   = i * tile;
        ln->isRoad = (i % 2 == 1);     // alterna: gramado/rua
        ln->carCount = 0;

        if (ln->isRoad) {
            int cars = 2 + GetRandomValue(0, 2);
            if (cars > MAX_CARS_PER_LANE) cars = MAX_CARS_PER_LANE;
            ln->carCount = cars;

            int   dir       = (GetRandomValue(0, 1) == 0) ? -1 : +1;
            float speedBase = randf(90.0f, 160.0f);

            for (int c = 0; c < cars; c++) {
                float wcar = tile * randf(1.2f, 1.8f);
                float hcar = tile * 0.8f;
                float x    = randf(-w->worldWidth, w->worldWidth);
                float y    = ln->yTop + (tile - hcar) * 0.5f;

                Car *car = &ln->cars[c];
                car->box = (Rectangle){ x, y, wcar, hcar };
                car->baseSpeed    = speedBase * randf(0.9f, 1.2f);
                car->dir          = dir;
                car->active       = true;
                car->textureIndex = 0;
            }
        }
    }
}

void World_Update(World *w, float dt, float difficulty) {
    float worldW = (w->worldWidth > 0) ? w->worldWidth : difficulty;

    for (int i = 0; i < w->laneCount; i++) {
        Lane *ln = &w->lanes[i];
        if (!ln->isRoad) continue;

        for (int c = 0; c < ln->carCount; c++) {
            Car *car = &ln->cars[c];
            if (!car->active) continue;

            car->box.x += car->dir * car->baseSpeed * dt;

            // reaparecer do outro lado
            if (car->dir > 0 && car->box.x > worldW + 20.0f) {
                car->box.x = -car->box.width - randf(0.0f, w->tile * 2.0f);
            } else if (car->dir < 0 && (car->box.x + car->box.width) < -20.0f) {
                car->box.x = worldW + randf(0.0f, w->tile * 2.0f);
            }
        }
    }
}

void World_Draw(const World *w, Rectangle visibleWorldRect) {
    int screenW = (int)visibleWorldRect.width;
    float tile  = w->tile;

    // --- LISTRAS “INFINITAS” (fundo) ---
    // Calcula um intervalo de índices de faixa que cobre todo o retângulo visível.
    // Usamos uma margem +/-2 para garantir cobertura quando a divisão com números negativos trunca.
    int kStart = (int)(visibleWorldRect.y / tile) - 2;
    int kEnd   = (int)((visibleWorldRect.y + visibleWorldRect.height) / tile) + 2;

    for (int k = kStart; k <= kEnd; k++) {
        float worldY = k * tile;
        int drawY = (int)(worldY - visibleWorldRect.y);

        // Alterna as cores por paridade do índice k
        bool isRoadStripe = (k % 2 != 0);
        Color stripeColor = isRoadStripe ? (Color){ 60, 60, 60, 255 }    // rua: cinza
                                         : (Color){ 60,120, 60, 255 };  // gramado: verde

        // faixa
        DrawRectangle(0, drawY, screenW, (int)tile, stripeColor);

        // linha na fronteira superior da faixa k
        Color lineColor = (k % 2 == 0) ? GREEN : BLACK;
        DrawLine(0, drawY, screenW, drawY, lineColor);
    }

    // Linha extra na borda inferior do retângulo visível (fecha visualmente a última faixa)
    {
        float bottomWorldY = (kEnd + 1) * tile;
        int drawY = (int)(bottomWorldY - visibleWorldRect.y);
        Color lineColor = ((kEnd + 1) % 2 == 0) ? GREEN : BLACK;
        DrawLine(0, drawY, screenW, drawY, lineColor);
    }

    // --- CARROS (apenas onde há lanes reais) ---
    for (int i = 0; i < w->laneCount; i++) {
        const Lane *ln = &w->lanes[i];
        if (!ln->isRoad) continue;

        for (int c = 0; c < ln->carCount; c++) {
            const Car *car = &ln->cars[c];
            if (!car->active) continue;

            Rectangle r = car->box;
            r.x -= visibleWorldRect.x;
            r.y -= visibleWorldRect.y;

            DrawRectangleRec(r, (Color){ 200, 30, 60, 255 });

            // “faróis” para indicar direção
            float hx = r.x + (car->dir > 0 ? r.width - 6.0f : 6.0f);
            DrawCircle(hx, r.y + r.height*0.3f, 3, YELLOW);
            DrawCircle(hx, r.y + r.height*0.7f, 3, YELLOW);
        }
    }
}

bool World_CheckCollision(const World *w, Rectangle player) {
    for (int i = 0; i < w->laneCount; i++) {
        const Lane *ln = &w->lanes[i];
        if (!ln->isRoad) continue;

        for (int c = 0; c < ln->carCount; c++) {
            const Car *car = &ln->cars[c];
            if (!car->active) continue;

            if (CheckCollisionRecs(player, car->box)) return true;
        }
    }
    return false;
}

bool World_CheckCollisionInset(const World *w, Rectangle player, float inset) {
    Rectangle p = (Rectangle){
        player.x + inset,
        player.y + inset,
        player.width  - 2*inset,
        player.height - 2*inset
    };
    return World_CheckCollision(w, p);
}
