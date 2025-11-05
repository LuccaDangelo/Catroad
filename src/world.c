#include "world.h"
#include "raylib.h"

static float randf(float a, float b) {
    return a + (float)GetRandomValue(0, 10000) / 10000.0f * (b - a);
}

void World_Init(World *w, float worldWidth, float tile) {
    w->tile = tile;
    w->worldWidth = worldWidth;
    w->laneCount = MAX_LANES;

    // Lógica de linhas: i=0 é a BASE (gramado) em yTop = 0
    // i=1 é a faixa logo acima (rua), em yTop = -tile, e assim por diante
    for (int i = 0; i < w->laneCount; i++) {
        Lane *ln = &w->lanes[i];
        ln->yTop = -i * tile;
        ln->isRoad = (i % 2 == 1); // alterna base/gramado, depois rua, etc.
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
                float x = (float)GetRandomValue(-(int)worldWidth, (int)worldWidth);
                ln->cars[c].box = (Rectangle){ x, ln->yTop + (tile - hcar)*0.5f, wcar, hcar };
                ln->cars[c].baseSpeed = speedBase * randf(0.9f, 1.2f);
                ln->cars[c].dir = dir;
                ln->cars[c].active = true;
            }
        }
    }
}

void World_Update(World *w, float dt, float difficulty) {
    const float W = w->worldWidth;

    for (int i = 0; i < w->laneCount; i++) {
        Lane *ln = &w->lanes[i];
        if (!ln->isRoad) continue;

        for (int c = 0; c < ln->carCount; c++) {
            Car *car = &ln->cars[c];
            if (!car->active) continue;

            float speed = car->baseSpeed * difficulty;
            car->box.x += car->dir * speed * dt;

            // wrap horizontal
            if (car->dir > 0 && car->box.x > W + 40) {
                car->box.x = -car->box.width - (float)GetRandomValue(0, (int)(w->tile*2));
            } else if (car->dir < 0 && car->box.x + car->box.width < -40) {
                car->box.x = W + (float)GetRandomValue(0, (int)(w->tile*2));
            }
        }
    }
}

void World_Draw(const World *w, const Rectangle visibleWorldRect) {
    // desenha apenas faixas que estão no retângulo visível da câmera
    for (int i = 0; i < w->laneCount; i++) {
        const Lane *ln = &w->lanes[i];
        float yTop = ln->yTop;
        float yBot = ln->yTop + w->tile;

        // checagem de interseção vertical simples
        if (yBot < visibleWorldRect.y || yTop > (visibleWorldRect.y + visibleWorldRect.height)) {
            continue;
        }

        Color col = ln->isRoad ? (Color){60, 60, 60, 255} : (Color){60, 120, 60, 255};
        DrawRectangle(0, (int)ln->yTop, (int)w->worldWidth, (int)w->tile, col);

        if (ln->isRoad) {
            DrawLine(0, (int)ln->yTop, (int)w->worldWidth, (int)ln->yTop, (Color){230, 230, 230, 120});
            DrawLine(0, (int)(ln->yTop + w->tile), (int)w->worldWidth, (int)(ln->yTop + w->tile), (Color){230, 230, 230, 120});
        }
    }

    // carros
    for (int i = 0; i < w->laneCount; i++) {
        const Lane *ln = &w->lanes[i];
        if (!ln->isRoad) continue;

        for (int c = 0; c < ln->carCount; c++) {
            const Car *car = &ln->cars[c];

            // culling simples
            if (car->box.y + car->box.height < visibleWorldRect.y ||
                car->box.y > visibleWorldRect.y + visibleWorldRect.height) {
                continue;
            }

            DrawRectangleRec(car->box, (Color){200, 30, 60, 255});
            DrawCircle(car->box.x + (car->dir > 0 ? car->box.width - 6 : 6),
                       car->box.y + car->box.height*0.3f, 3, YELLOW);
            DrawCircle(car->box.x + (car->dir > 0 ? car->box.width - 6 : 6),
                       car->box.y + car->box.height*0.7f, 3, YELLOW);
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
    Rectangle p = player;
    p.x += inset; p.y += inset;
    p.width  = (p.width  > 2*inset) ? (p.width  - 2*inset) : p.width;
    p.height = (p.height > 2*inset) ? (p.height - 2*inset) : p.height;

    for (int i = 0; i < w->laneCount; i++) {
        const Lane *ln = &w->lanes[i];
        if (!ln->isRoad) continue;
        for (int c = 0; c < ln->carCount; c++) {
            const Car *car = &ln->cars[c];
            if (!car->active) continue;
            if (CheckCollisionRecs(p, car->box)) return true;
        }
    }
    return false;
}
