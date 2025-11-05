#include "world.h"
#include "raylib.h"

// Random float in [a, b]
static float randf(float a, float b) {
    return a + (float)GetRandomValue(0, 10000) / 10000.0f * (b - a);
}

// Compatível com versões de raylib sem DrawTextureTiled.
// Repete a textura 'tex' (região 'src') para preencher 'dest' com scale/rotation/origin.
static void DrawTextureTiledCompat(Texture2D tex,
                                   Rectangle src,
                                   Rectangle dest,
                                   Vector2 origin,
                                   float rotation,
                                   float scale,
                                   Color tint)
{
    if (scale <= 0.0f) scale = 1.0f;

    const float tileW = src.width  * scale;
    const float tileH = src.height * scale;
    if (tileW <= 0 || tileH <= 0) return;

    const float destRight  = dest.x + dest.width;
    const float destBottom = dest.y + dest.height;

    for (float y = dest.y; y < destBottom; y += tileH) {
        float drawH = tileH;
        float srcH  = src.height;
        if (y + drawH > destBottom) {
            drawH = destBottom - y;
            srcH  = drawH / scale;
        }

        for (float x = dest.x; x < destRight; x += tileW) {
            float drawW = tileW;
            float srcW  = src.width;
            if (x + drawW > destRight) {
                drawW = destRight - x;
                srcW  = drawW / scale;
            }

            Rectangle srcPart = (Rectangle){ src.x, src.y, srcW, srcH };
            Rectangle dstPart = (Rectangle){ x, y, drawW, drawH };

            DrawTexturePro(tex, srcPart, dstPart, origin, rotation, tint);
        }
    }
}

void World_Init(World *w, float worldWidth, float tile) {
    w->tile = tile;
    w->worldWidth = worldWidth;
    w->laneCount = MAX_LANES;

    // --- CARREGAR TEXTURAS ---
    w->texPavement = LoadTexture("resources/pavement.png");
    w->texRoad     = LoadTexture("resources/road.png");

    w->texCar[0] = LoadTexture("resources/car1.png");
    w->texCar[1] = LoadTexture("resources/car2.png");
    w->texCar[2] = LoadTexture("resources/car3.png");
    // --------------------------

    // i=0 é a base (gramado) em yTop = 0; i=1 é a rua em yTop = -tile; etc.
    for (int i = 0; i < w->laneCount; i++) {
        Lane *ln = &w->lanes[i];
        ln->yTop = -i * tile;
        ln->isRoad = (i % 2 == 1); // alterna: gramado/rua/gramado/rua...
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
                ln->cars[c].textureIndex = GetRandomValue(0, NUM_CAR_TEXTURES - 1);
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
    // --- 1) DESENHAR FAIXAS (TEXTURAS) ---
    for (int i = 0; i < w->laneCount; i++) {
        const Lane *ln = &w->lanes[i];
        float yTop = ln->yTop;
        float yBot = ln->yTop + w->tile;

        // culling vertical simples
        if (yBot < visibleWorldRect.y || yTop > (visibleWorldRect.y + visibleWorldRect.height)) {
            continue;
        }

        Texture2D tex = ln->isRoad ? w->texRoad : w->texPavement;
        Rectangle texSource = (Rectangle){ 0, 0, (float)tex.width, (float)tex.height };
        Rectangle texDest   = (Rectangle){ 0, ln->yTop, w->worldWidth, w->tile };
        Vector2 origin = (Vector2){ 0, 0 };

        // Compatível com versões sem DrawTextureTiled
        DrawTextureTiledCompat(tex, texSource, texDest, origin, 0.0f, 1.0f, WHITE);

        if (ln->isRoad) {
            DrawLine(0, (int)ln->yTop, (int)w->worldWidth, (int)ln->yTop, (Color){230, 230, 230, 120});
            DrawLine(0, (int)(ln->yTop + w->tile), (int)w->worldWidth, (int)(ln->yTop + w->tile), (Color){230, 230, 230, 120});
        }
    }

    // --- 2) DESENHAR CARROS (TEXTURAS) ---
    for (int i = 0; i < w->laneCount; i++) {
        const Lane *ln = &w->lanes[i];
        if (!ln->isRoad) continue;

        for (int c = 0; c < ln->carCount; c++) {
            const Car *car = &ln->cars[c];
            if (!car->active) continue;

            // culling vertical
            if (car->box.y + car->box.height < visibleWorldRect.y ||
                car->box.y > visibleWorldRect.y + visibleWorldRect.height) {
                continue;
            }

            Texture2D carTex = w->texCar[car->textureIndex];
            Rectangle source = (Rectangle){ 0, 0, (float)carTex.width, (float)carTex.height };
            if (car->dir < 0) {
                source.width = -source.width; // flip horizontal
            }

            Rectangle dest = car->box;
            DrawTexturePro(carTex, source, dest, (Vector2){0,0}, 0.0f, WHITE);

            // Faróis (opcional)
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
