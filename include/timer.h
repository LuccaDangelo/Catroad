#ifndef TIMER_H
#define TIMER_H
#include <stdbool.h>
#include "raylib.h" 

typedef struct {
    float timeLeft;
    bool running;
} GameTimer;

void Timer_Start(GameTimer *t, float seconds);
void Timer_Update(GameTimer *t, float dt);
void Timer_Reset(GameTimer *t, float seconds);
bool Timer_IsOver(const GameTimer *t);

#endif