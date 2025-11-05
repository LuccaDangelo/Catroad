#ifndef PLAYER_H
#define PLAYER_H
#include "raylib.h"

typedef struct {
    Rectangle box;
    int row;
    int maxRow;
    int score;
    float moveCd;
} Player;

void Player_Init(Player *p, Vector2 startPos, float size);
void Player_Update(Player *p, float dt, float tile, int maxRows, int screenW, int screenH);
void Player_Draw(const Player *p, Vector2 cameraOffset);

#endif