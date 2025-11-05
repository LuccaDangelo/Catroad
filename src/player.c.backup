#include "player.h"
#include "raylib.h"

void Player_Init(Player *p, Vector2 startPos, float size) {
    p->box = (Rectangle){ startPos.x, startPos.y, size, size };
    p->row = 0;
    p->maxRow = 0;
    p->score = 0;
    p->moveCd = 0.0f;
}

void Player_Update(Player *p, float dt, float tile, int maxRows, int screenW, int screenH) {
    if (p->moveCd > 0.0f) p->moveCd -= dt;

    Vector2 delta = {0};
    bool movedRow = false;

    if (p->moveCd <= 0.0f) {
        if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) {
            delta.y -= tile;
            movedRow = true;
        } else if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) {
            if (p->row > 0) {
                delta.y += tile;
                p->row -= 1;
                p->moveCd = 0.12f;
                p->box.y += delta.y;
            }
            return;
        } else if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
            delta.x -= tile;
        } else if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
            delta.x += tile;
        }

        if (delta.x != 0 || movedRow) {
            p->box.x += delta.x;
            if (movedRow) {
                p->box.y += delta.y;
                if (delta.y < 0) {
                    p->row += 1;
                    if (p->row > p->maxRow) {
                        p->maxRow = p->row;
                        p->score = p->maxRow;
                    }
                }
            }
            p->moveCd = 0.12f;
        }
    }

    if (p->box.x < 0) p->box.x = 0;
    if (p->box.x + p->box.width > screenW) p->box.x = screenW - p->box.width;
    if (p->box.y + p->box.height > screenH) p->box.y = screenH - p->box.height;

    if (p->row > maxRows) p->row = maxRows;
}

// 🎥 FUNÇÃO DRAW AGORA USA CÂMERA
void Player_Draw(const Player *p, Vector2 cameraOffset) {
    // 🎥 CRIA UMA CÓPIA DA POSIÇÃO DO PLAYER COM OFFSET APLICADO
    Rectangle playerRect = p->box;
    playerRect.y -= cameraOffset.y;
    
    // Desenha o player na posição ajustada pela câmera
    DrawRectangleRec(playerRect, (Color){240, 180, 60, 255});
    
    // 🎥 TODOS OS ELEMENTOS VISUAIS USAM A POSIÇÃO COM OFFSET
    DrawTriangle(
        (Vector2){playerRect.x + playerRect.width*0.25f, playerRect.y + 8},
        (Vector2){playerRect.x + playerRect.width*0.40f, playerRect.y - playerRect.height*0.25f},
        (Vector2){playerRect.x + playerRect.width*0.55f, playerRect.y + 8},
        (Color){240, 180, 60, 255}
    );
    DrawTriangle(
        (Vector2){playerRect.x + playerRect.width*0.60f, playerRect.y + 8},
        (Vector2){playerRect.x + playerRect.width*0.75f, playerRect.y - playerRect.height*0.25f},
        (Vector2){playerRect.x + playerRect.width*0.90f, playerRect.y + 8},
        (Color){240, 180, 60, 255}
    );
    DrawCircle(playerRect.x + playerRect.width*0.35f, playerRect.y + playerRect.height*0.45f, 3, BLACK);
    DrawCircle(playerRect.x + playerRect.width*0.65f, playerRect.y + playerRect.height*0.45f, 3, BLACK);
}