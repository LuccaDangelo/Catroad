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

        // 🎥 CÂMERA FUNCIONAL - SIMPLES E DIRETA
        float targetY = player.box.y - (SCREEN_H * 0.4f); // Player a 40% da tela
        
        // Só move a câmera se o player estiver subindo
        if (targetY > cameraOffset.y) {
            cameraOffset.y = targetY;
        }
        
        // Nunca deixa a câmera ir para baixo do chão
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

    // 🎥 IMPORTANTE: Passar cameraOffset para ambos
    World_Draw(&world, cameraOffset);
    Player_Draw(&player, cameraOffset);

    // UI (fica fixa)
    DrawRectangle(0, 0, SCREEN_W, 40, (Color){0, 0, 0, 140});
    char hud[128];
    snprintf(hud, sizeof(hud), "Tempo: %02d | Pontos: %d | Camera: %.1f",
             (int)timer35.timeLeft, player.score, cameraOffset.y);
    DrawText(hud, 16, 10, 20, RAYWHITE);

    // ... resto do game over
    EndDrawing();
}

void Game_Unload(void) {
    CloseAudioDevice();
    CloseWindow();
}

bool Game_ShouldClose(void) {
    return WindowShouldClose();
}