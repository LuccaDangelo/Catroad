#ifndef GAME_H
#define GAME_H

#include <stdbool.h>
#include "raylib.h" 

void Game_Init(void);
void Game_Update(void);
void Game_Draw(void);
void Game_Unload(void);
bool Game_ShouldClose(void);

#endif