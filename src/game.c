#include "game.h"
#include "player.h"
#include "world.h"
#include "timer.h"

#include <stdio.h>

#define SCREEN_W  800
#define SCREEN_H  600
#define TILE      48.0f
#define TOTAL_TIME 35.0f

typedef enum { STATE_PLAYING, STATE_GAMEOVER } GameState;

static Player player;
static World world;
static GameTimer timer35;
static GameState state;
static int rowsPlayable = MAX_LANES - 1;

// 🎥 VARIÁVEIS DO SISTEMA DE CÂMERA PROFISSIONAL
static Vector2 cameraOffset = {0, 0};
static float cameraScrollThreshold = SCREEN_H * 0.3f; // 30% do topo = zona de scroll
static float cameraLerpSpeed = 8.0f; // Velocidade de suavização

static void ResetGame(void) {
    World_Init(&world, SCREEN_W, SCREEN_H, TILE);
    Player_Init(&player, (Vector2){ SCREEN_W*0.5f - TILE*0.5f, SCREEN_H - TILE }, TILE);
    Timer_Reset(&timer35, TOTAL_TIME);
    state = STATE_PLAYING;
    
    // 🎥 Reset da câmera quando reinicia o jogo
    Game_ResetCamera();
}

// 🎥 IMPLEMENTAÇÃO DAS FUNÇÕES DA CÂMERA
Vector2 Game_GetCameraOffset(void) {
    return cameraOffset;
}

void Game_ResetCamera(void) {
    cameraOffset = (Vector2){0, 0};
}

void Game_Init(void) {
    InitWindow(SCREEN_W, SCREEN_H, "CatRoad - raylib");
    SetTargetFPS(60);
    InitAudioDevice();
    SetRandomSeed(GetTime());

    World_Init(&world, SCREEN_W, SCREEN_H, TILE);
    Player_Init(&player, (Vector2){ SCREEN_W*0.5f - TILE*0.5f, SCREEN_H - TILE }, TILE);
    Timer_Start(&timer35, TOTAL_TIME);
    state = STATE_PLAYING;
}

void Game_Update(void) {
    float dt = GetFrameTime();

    if (state == STATE_PLAYING) {
        Timer_Update(&timer35, dt);
        World_Update(&world, dt, SCREEN_W);
        Player_Update(&player, dt, TILE, rowsPlayable, SCREEN_W, SCREEN_H);

        // 🎥 CORREÇÃO DA LÓGICA DA CÂMERA
        float playerScreenY = player.box.y - cameraOffset.y;
        
        // Debug: mostra os valores
        printf("PlayerY: %.1f, CameraY: %.1f, ScreenY: %.1f, Threshold: %.1f\n", 
               player.box.y, cameraOffset.y, playerScreenY, cameraScrollThreshold);
        
        // Se o player está acima do threshold, move a câmera PRA CIMA
        if (playerScreenY < cameraScrollThreshold) {
            // Quanto precisamos mover a câmera
            float moveAmount = cameraScrollThreshold - playerScreenY;
            cameraOffset.y += moveAmount * cameraLerpSpeed * dt;
            
            printf("MOVENDO CÂMERA: %.1f\n", moveAmount);
        }
        
        // Garante que a câmera não vá para baixo do nível inicial
        if (cameraOffset.y < 0) cameraOffset.y = 0;

        // checa colisão ou fim do tempo
        if (World_CheckCollision(&world, player.box) || Timer_IsOver(&timer35)) {
            state = STATE_GAMEOVER;
        }

        // restart rápido
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
    ClearBackground((Color){ 30, 30, 40, 255 });

    // 🎥 MUNDO E PLAYER DESENHAM COM OFFSET DA CÂMERA
    World_Draw(&world, cameraOffset);
    Player_Draw(&player, cameraOffset);

    // 🎥 UI (HUD) NÃO se move com a câmera - fica fixo na tela
    DrawRectangle(0, 0, SCREEN_W, 40, (Color){0, 0, 0, 140});
    char hud[128];
    snprintf(hud, sizeof(hud), "Tempo: %02d  |  Pontos: %d | CameraY: %.1f",
             (int)timer35.timeLeft, player.score, cameraOffset.y);
    DrawText(hud, 16, 10, 20, RAYWHITE);

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
    CloseAudioDevice();
    CloseWindow();
}

bool Game_ShouldClose(void) {
    return WindowShouldClose();
}