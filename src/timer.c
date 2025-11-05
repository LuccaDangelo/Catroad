#include "timer.h"

void Timer_Start(GameTimer *t, float seconds) {
    t->timeLeft = seconds;
    t->running = true;
}

void Timer_Update(GameTimer *t, float dt) {
    if (!t->running) return;
    t->timeLeft -= dt;
    if (t->timeLeft < 0.0f) {
        t->timeLeft = 0.0f;
        t->running = false;
    }
}

void Timer_Reset(GameTimer *t, float seconds) {
    t->timeLeft = seconds;
    t->running = true;
}

bool Timer_IsOver(const GameTimer *t) {
    return !t->running || t->timeLeft <= 0.0f;
}