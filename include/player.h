#ifndef PLAYER_H
#define PLAYER_H
#include "raylib.h"

typedef struct {
    Rectangle box;   // posição/tamanho (mundo)
    int row;         // linha atual (0 = base; negativo para baixo não usamos)
    int maxRow;      // linha máxima alcançada (para score)
    int score;       // = maxRow
    float moveCd;    // cooldown para movimentação em grade (segundos)
    float invuln;    // invulnerável por alguns ms ao iniciar/reiniciar
} Player;

void Player_Init(Player *p, Vector2 startPos, float size);
void Player_Update(Player *p, float dt, float tile, int maxRows, int worldMinX, int worldMaxX);
void Player_Draw(const Player *p);

#endif
