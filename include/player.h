#ifndef PLAYER_H
#define PLAYER_H
#include "raylib.h"

typedef struct {
    Rectangle box;   // posição/tamanho do gato
    int row;         // linha atual (0 = base)
    int maxRow;      // linha máxima alcançada (para pontuação)
    int score;       // pontuação = maxRow
    float moveCd;    // cooldown de movimento (anti-spam)
} Player;

void Player_Init(Player *p, Vector2 startPos, float size);
void Player_Update(Player *p, float dt, float tile, int maxRows, int screenW, int screenH);
void Player_Draw(const Player *p);

#endif
