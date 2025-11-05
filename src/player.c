#include "player.h"
#include "raylib.h"

void Player_Init(Player *p, Vector2 startPos, float size) {
    p->box = (Rectangle){ startPos.x, startPos.y, size, size };
    p->row = 0;
    p->maxRow = 0;
    p->score = 0;
    p->moveCd = 0.0f;
    p->invuln = 0.35f;
}

void Player_Update(Player *p, float dt, float tile, int maxRows, int worldMinX, int worldMaxX) {
    if (p->moveCd > 0.0f) p->moveCd -= dt;
    if (p->invuln > 0.0f) p->invuln -= dt;

    Vector2 delta = {0};
    bool movedRowUp = false;
    bool movedRowDown = false;

    if (p->moveCd <= 0.0f) {
        if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) {
            delta.y -= tile;        // subir (y menor)
            movedRowUp = true;
        } else if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) {
            if (p->row > 0) {      // não desce abaixo da base
                delta.y += tile;
                movedRowDown = true;
            }
        } else if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
            delta.x -= tile;
        } else if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
            delta.x += tile;
        }

        if (delta.x != 0 || movedRowUp || movedRowDown) {
            p->box.x += delta.x;
            p->box.y += delta.y;

            if (movedRowUp) {
                p->row += 1;
                if (p->row > p->maxRow) {
                    p->maxRow = p->row;
                    p->score = p->maxRow;
                }
            } else if (movedRowDown) {
                p->row -= 1;
            }
            p->moveCd = 0.12f;
        }
    }

    // Limites horizontais do "mundo" (não sair da rua/gramado)
    if (p->box.x < worldMinX) p->box.x = worldMinX;
    if (p->box.x + p->box.width > worldMaxX) p->box.x = worldMaxX - p->box.width;

    if (p->row > maxRows) p->row = maxRows; // segurança
}

void Player_Draw(const Player *p) {
    // corpo do gato
    DrawRectangleRec(p->box, (Color){240, 180, 60, 255});
    // orelhas (triângulos)
    DrawTriangle(
        (Vector2){p->box.x + p->box.width*0.28f, p->box.y + 8},
        (Vector2){p->box.x + p->box.width*0.40f, p->box.y - p->box.height*0.25f},
        (Vector2){p->box.x + p->box.width*0.52f, p->box.y + 8},
        (Color){240, 180, 60, 255}
    );
    DrawTriangle(
        (Vector2){p->box.x + p->box.width*0.60f, p->box.y + 8},
        (Vector2){p->box.x + p->box.width*0.72f, p->box.y - p->box.height*0.25f},
        (Vector2){p->box.x + p->box.width*0.84f, p->box.y + 8},
        (Color){240, 180, 60, 255}
    );
    // olhos
    DrawCircle(p->box.x + p->box.width*0.35f, p->box.y + p->box.height*0.45f, 3, BLACK);
    DrawCircle(p->box.x + p->box.width*0.65f, p->box.y + p->box.height*0.45f, 3, BLACK);
}
