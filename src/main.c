#include "game.h"

int main(void) {
    Game_Init();

    while (!Game_ShouldClose()) {
        Game_Update();
        Game_Draw();
    }

    Game_Unload();
    return 0;
}