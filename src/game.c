#include "game.h"
#include "player.h"
#include "world.h"
#include "timer.h"

#include "raylib.h"
#include <stdio.h>

#define SCREEN_W    800
#define SCREEN_H    600
#define TILE        48.0f
#define TOTAL_TIME 35.0f
#define WORLD_W     800.0f  // largura lógica do mundo (coincide com tela)

typedef enum { STATE_HOME, STATE_PLAYING, STATE_GAMEOVER } GameState;

static Player player;
static World world;
static GameTimer timer35;
static GameState state;

static int rowsPlayable = MAX_LANES - 1; // topo lógico pré-gerado
static bool debugHitboxes = false;

// Câmera que segue o gato
static Camera2D cam;

// util
static void ResetGame(void) {
    // World_Init agora também carrega as texturas
    World_Init(&world, WORLD_W, TILE);
    Player_Init(&player, (Vector2){ WORLD_W*0.5f - TILE*0.5f, 0.0f }, TILE);
    Timer_Reset(&timer35, TOTAL_TIME);
    state = STATE_PLAYING;

    cam.target = (Vector2){ player.box.x + player.box.width*0.5f, player.box.y + player.box.height*0.5f };
    cam.offset = (Vector2){ SCREEN_W*0.5f, SCREEN_H*0.65f }; // mantém o player mais para a parte de baixo da tela
    cam.zoom = 1.0f;
    cam.rotation = 0.0f;
}

void Game_Init(void) {
    InitWindow(SCREEN_W, SCREEN_H, "CatRoad - raylib");
    SetTargetFPS(60);
    InitAudioDevice();

    // World_Init agora também carrega as texturas
    World_Init(&world, WORLD_W, TILE);
    
    Player_Init(&player, (Vector2){ WORLD_W*0.5f - TILE*0.5f, 0.0f }, TILE);
    Timer_Start(&timer35, TOTAL_TIME);
    state = STATE_HOME;

    cam.target = (Vector2){ player.box.x + player.box.width*0.5f, player.box.y + player.box.height*0.5f };
    cam.offset = (Vector2){ SCREEN_W*0.5f, SCREEN_H*0.65f };
    cam.zoom = 1.0f;
    cam.rotation = 0.0f;
}

void Game_Update(void) {
    float dt = GetFrameTime();

    if (IsKeyPressed(KEY_F1)) debugHitboxes = !debugHitboxes;

    if (state == STATE_HOME) {
        // Pequena animação de "piscar" poderia ser adicionada, por hora só entrada
        if (IsKeyPressed(KEY_ENTER)) {
            ResetGame();
        }
        return;
    }

    if (state == STATE_PLAYING) {
        // Dificuldade aumenta com a altura alcançada
        float difficulty = 1.0f + 0.07f * (float)player.maxRow;

        Timer_Update(&timer35, dt);
        World_Update(&world, dt, difficulty);
        Player_Update(&player, dt, TILE, rowsPlayable, 0, (int)WORLD_W);

        // atualiza câmera para seguir o jogador (eixo Y subindo)
        cam.target = (Vector2){ player.box.x + player.box.width*0.5f, player.box.y + player.box.height*0.5f };

        // colisão com hitbox justa + timer
        bool hit = World_CheckCollisionInset(&world, player.box, 4.0f);
        if ((hit && player.invuln <= 0.0f) || Timer_IsOver(&timer35)) {
            state = STATE_GAMEOVER;
        }

        // restart rápido opcional
        if (IsKeyPressed(KEY_R)) {
            ResetGame();
        }
    } else if (state == STATE_GAMEOVER) {
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_R)) {
            ResetGame();
        }
    }
}

void Game_Draw(void) {
    BeginDrawing();
    ClearBackground((Color){ 25, 25, 35, 255 });

    if (state == STATE_HOME) {
        const char *title = "CATROAD";
        int tw = MeasureText(title, 72);
        DrawText(title, SCREEN_W/2 - tw/2, SCREEN_H/2 - 180, 72, (Color){240, 180, 60, 255});

        const char *subtitle = "Vá o mais longe possível em 35s";
        int sw = MeasureText(subtitle, 24);
        DrawText(subtitle, SCREEN_W/2 - sw/2, SCREEN_H/2 - 80, 24, RAYWHITE);

        const char *hint = "[ENTER] para jogar | WASD/Setas para mover | [F1] hitboxes";
        int hw = MeasureText(hint, 20);
        DrawText(hint, SCREEN_W/2 - hw/2, SCREEN_H/2 + 10, 20, GRAY);
        EndDrawing();
        return;
    }

    // Define retângulo visível no mundo para culling de desenho
    Rectangle visibleWorldRect = {
        cam.target.x - cam.offset.x / cam.zoom,
        cam.target.y - cam.offset.y / cam.zoom,
        SCREEN_W / cam.zoom,
        SCREEN_H / cam.zoom
    };

    BeginMode2D(cam);
        // World_Draw agora desenha as texturas
        World_Draw(&world, visibleWorldRect);
        Player_Draw(&player);

        if (debugHitboxes) {
            Rectangle p = player.box; p.x+=4; p.y+=4; p.width-=8; p.height-=8;
            DrawRectangleLinesEx(p, 1, GREEN);

            for (int i = 0; i < world.laneCount; i++) {
                const Lane *ln = &world.lanes[i];
                if (!ln->isRoad) continue;
                for (int c = 0; c < ln->carCount; c++) {
                    const Car *car = &ln->cars[c];
                    if (!car->active) continue;
                    DrawRectangleLinesEx(car->box, 1, RED);
                }
            }
        }
    EndMode2D();

    // HUD
    DrawRectangle(0, 0, SCREEN_W, 44, (Color){0, 0, 0, 150});
    char hud[128];
    snprintf(hud, sizeof(hud), "Tempo: %02d  |  Pontos: %d",
             (int)timer35.timeLeft, player.score);
    DrawText(hud, 16, 12, 20, RAYWHITE);

    if (state == STATE_GAMEOVER) {
        const char *msg = "Game Over!";
        int fw = MeasureText(msg, 40);
        DrawText(msg, SCREEN_W/2 - fw/2, SCREEN_H/2 - 60, 40, RED);

        char sc[128];
        snprintf(sc, sizeof(sc), "Distancia (linhas): %d", player.score);
        int sw = MeasureText(sc, 24);
        DrawText(sc, SCREEN_W/2 - sw/2, SCREEN_H/2 - 16, 24, RAYWHITE);

        const char *hint = "[ENTER] ou [R] para reiniciar";
        int hw = MeasureText(hint, 20);
        DrawText(hint, SCREEN_W/2 - hw/2, SCREEN_H/2 + 20, 20, GRAY);
    }

    EndDrawing();
}

void Game_Unload(void) {
    // --- NOVO: Descarregar nossas texturas ---
    UnloadTexture(world.texPavement);
    UnloadTexture(world.texRoad);
    for (int i = 0; i < NUM_CAR_TEXTURES; i++) {
        UnloadTexture(world.texCar[i]);
    }
    // ----------------------------------------

    CloseAudioDevice();
    CloseWindow();
}

bool Game_ShouldClose(void) {
    return WindowShouldClose();
}