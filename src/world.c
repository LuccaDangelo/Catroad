#include "world.h"
#include "raylib.h"

static float randf(float a, float b) {
    return a + (float)GetRandomValue(0, 10000) / 10000.0f * (b - a);
}

void World_Init(World *w, int screenW, int screenH, float tile) {
    w->tile = tile;
    w->laneCount = MAX_LANES;

    // distribui faixas do bottom para cima
    for (int i = 0; i < w->laneCount; i++) {
        Lane *ln = &w->lanes[i];
        ln->y = screenH - (int)((i+1) * tile);
        ln->isRoad = (i % 2 == 1); // alterna: base gramado (i=0), depois rua, etc.
        ln->carCount = 0;

        if (ln->isRoad) {
            int cars = 2 + GetRandomValue(0, 2); // 2–4 carros
            if (cars > MAX_CARS_PER_LANE) cars = MAX_CARS_PER_LANE;
            ln->carCount = cars;

            int dir = (GetRandomValue(0, 1) == 0) ? -1 : +1;
            float speedBase = randf(90.0f, 160.0f);
            for (int c = 0; c < cars; c++) {
                float wcar = tile * randf(1.2f, 1.8f);
                float hcar = tile * 0.8f;
                float x = (float)GetRandomValue(-screenW, screenW);
                ln->cars[c].box = (Rectangle){ x, (float)ln->y + (tile - hcar)*0.5f, wcar, hcar };
                ln->cars[c].speed = speedBase * randf(0.9f, 1.2f);
                ln->cars[c].dir = dir;
                ln->cars[c].active = true;
            }
        }
    }
}

void World_Update(World *w, float dt, int screenW) {
    // move os carros com wrap
    for (int i = 0; i < w->laneCount; i++) {
        Lane *ln = &w->lanes[i];
        if (!ln->isRoad) continue;
        for (int c = 0; c < ln->carCount; c++) {
            Car *car = &ln->cars[c];
            if (!car->active) continue;
            car->box.x += car->dir * car->speed * dt;

            if (car->dir > 0 && car->box.x > screenW + 20) {
                car->box.x = -car->box.width - (float)GetRandomValue(0, (int)(w->tile*2));
            } else if (car->dir < 0 && car->box.x + car->box.width < -20) {
                car->box.x = screenW + (float)GetRandomValue(0, (int)(w->tile*2));
            }
        }
    }
}

void World_Draw(const World *w) {
    // gramado/base
    for (int i = 0; i < w->laneCount; i++) {
        const Lane *ln = &w->lanes[i];
        Color col = ln->isRoad ? (Color){ 60, 60, 60, 255 } : (Color){ 60, 120, 60, 255 };
        DrawRectangle(0, ln->y, GetScreenWidth(), (int)w->tile, col);

        // linhas de faixa (rua)
        if (ln->isRoad) {
            DrawLine(0, ln->y, GetScreenWidth(), ln->y, (Color){ 230, 230, 230, 120 });
            DrawLine(0, ln->y + (int)w->tile, GetScreenWidth(), ln->y + (int)w->tile, (Color){ 230, 230, 230, 120 });
        }
    }

    // carros
    for (int i = 0; i < w->laneCount; i++) {
        const Lane *ln = &w->lanes[i];
        if (!ln->isRoad) continue;
        for (int c = 0; c < ln->carCount; c++) {
            const Car *car = &ln->cars[c];
            if (!car->active) continue;
            DrawRectangleRec(car->box, (Color){ 200, 30, 60, 255 });
            // “faróis”
            DrawCircle(car->box.x + (car->dir > 0 ? car->box.width - 6 : 6),
                       car->box.y + car->box.height*0.3f, 3, YELLOW);
            DrawCircle(car->box.x + (car->dir > 0 ? car->box.width - 6 : 6),
                       car->box.y + car->box.height*0.7f, 3, YELLOW);
        }
    }
}

bool World_CheckCollision(const World *w, Rectangle player) {
    // colisão com carros somente nas ruas
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
