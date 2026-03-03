/*******************************************************************************************
 *
 *   raylib - classic game: arkanoid
 *
 *   Sample game developed by Marc Palau and Ramon Santamaria
 *
 *   This game has been created using raylib v1.3 (www.raylib.com)
 *   raylib is licensed under an unmodified zlib/libpng license (View raylib.h
 * for details)
 *
 *   Copyright (c) 2015 Ramon Santamaria (@raysan5)
 *
 ********************************************************************************************/

#include "raylib.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

//----------------------------------------------------------------------------------
// Some Defines
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef struct Player {
  Vector2 position;
  Vector2 size;
  Color color;
  int id;
} Player;

typedef struct Ball {
  Vector2 position;
  Vector2 speed;
  int radius;
  bool active;
} Ball;

//------------------------------------------------------------------------------------
// Global Variables Declaration
//------------------------------------------------------------------------------------
static const int screenWidth = 1920;
static const int screenHeight = 1080;

static bool gameOver = false;
static bool pause = false;

static Player player1 = {0};
static Player player2 = {0};
static Ball ball = {0};

//------------------------------------------------------------------------------------
// Module Functions Declaration (local)
//------------------------------------------------------------------------------------
static void InitGame(void);        // Initialize game
static void UpdateGame(void);      // Update game (one frame)
static void DrawGame(void);        // Draw game (one frame)
static void UnloadGame(void);      // Unload game
static void UpdateDrawFrame(void); // Update and Draw (one frame)

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void) {
  // Initialization (Note windowTitle is unused on Android)
  //---------------------------------------------------------
  InitWindow(screenWidth, screenHeight, "Pong");

  InitGame();

#if defined(PLATFORM_WEB)
  emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else
  SetTargetFPS(75);
  //--------------------------------------------------------------------------------------

  // Main game loop
  while (!WindowShouldClose()) // Detect window close button or ESC key
  {
    // Update and Draw
    //----------------------------------------------------------------------------------
    UpdateDrawFrame();
    //----------------------------------------------------------------------------------
  }
#endif
  // De-Initialization
  //--------------------------------------------------------------------------------------
  UnloadGame(); // Unload loaded data (textures, sounds, models...)

  CloseWindow(); // Close window and OpenGL context
  //--------------------------------------------------------------------------------------

  return 0;
}

//------------------------------------------------------------------------------------
// Module Functions Definitions (local)
//------------------------------------------------------------------------------------

void DrawPlayer(Player p) {
  DrawRectangle(p.position.x - p.size.x / 2, p.position.y - p.size.y / 2,
                p.size.x, p.size.y, p.color);
}

void InitializeBall(Player p) {
  printf("sigma");
  ball.position =
      (Vector2){p.position.x - p.size.x / 2 - ball.radius, p.position.y};
}

// Initialize game variables
void InitGame(void) {

  // Initialize player
  player1.position = (Vector2){screenWidth * 12 / 13, screenHeight / 2};
  player1.size = (Vector2){20, screenHeight / 5};
  player1.color = WHITE;
  player1.id = 1;

  player2.position = (Vector2){screenWidth / 13, screenHeight / 2};
  player2.size = (Vector2){20, screenHeight / 5};
  player2.color = WHITE;
  player2.id = 2;

  // Initialize ball
  if (GetRandomValue(0, 1) == 0) {
    InitializeBall(player1);
  } else {
    InitializeBall(player2);
  }
  ball.speed = (Vector2){0, 0};
  ball.radius = 16;
  ball.active = false;
}

// Update game (one frame)
void UpdateGame(void) {
  if (!gameOver) {
    if (IsKeyPressed('P'))
      pause = !pause;

    if (!pause) {
      // Player movement logic
      if (IsKeyDown(KEY_UP))
        player1.position.y -= 5;
      if ((player1.position.y - player1.size.y / 2) <= 0)
        player1.position.y = player1.size.y / 2;
      if (IsKeyDown(KEY_DOWN))
        player1.position.y += 5;
      if ((player1.position.y + player1.size.y / 2) >= screenHeight)
        player1.position.y = screenHeight - player1.size.y / 2;

      if (IsKeyDown(KEY_W))
        player2.position.y -= 5;
      if ((player2.position.y - player2.size.y / 2) <= 0)
        player2.position.y = player2.size.y / 2;
      if (IsKeyDown(KEY_S))
        player2.position.y += 5;
      if ((player2.position.y + player2.size.y / 2) >= screenHeight)
        player2.position.y = screenHeight - player2.size.y / 2;

      // Ball launching logic
      if (!ball.active) {
        if (IsKeyPressed(KEY_SPACE)) {
          ball.active = true;
          ball.speed = (Vector2){-32, 0};
        }
      }

      // Ball movement logic
      if (ball.active) {
        ball.position.x += ball.speed.x;
        ball.position.y += ball.speed.y;
      } else {
        if (GetRandomValue(0, 1) == 0) {
          InitializeBall(player1);
        } else {
          InitializeBall(player2);
        }
      }

      // Collision logic: ball vs walls
      if (((ball.position.y - ball.radius) <= 0) ||
          ((ball.position.y + ball.radius) >= screenHeight)) {
        ball.speed.y *= -1;
      }
      if (((ball.position.x + ball.radius) >= screenWidth) ||
          ((ball.position.x - ball.radius) <= 0)) {
        ball.speed = (Vector2){0, 0};
        ball.active = false;
      }

      // Collision logic: ball vs player
      if (CheckCollisionCircleRec(
              ball.position, ball.radius,
              (Rectangle){player1.position.x - player1.size.x / 2,
                          player1.position.y - player1.size.y / 2,
                          player1.size.x, player1.size.y})) {
        if (ball.speed.x > 0) {
          ball.speed.x *= -1;
          ball.speed.y =
              (ball.position.y - player1.position.y) / (player1.size.y / 2) * 5;
        }
      }

      if (CheckCollisionCircleRec(
              ball.position, ball.radius,
              (Rectangle){player2.position.x - player2.size.x / 2,
                          player2.position.y - player2.size.y / 2,
                          player2.size.x, player2.size.y})) {
        if (ball.speed.x < 0) {
          ball.speed.x *= -1;
          ball.speed.y =
              (ball.position.y - player2.position.y) / (player2.size.y / 2) * 5;
        }
      }
    }
  } else {
    if (IsKeyPressed(KEY_ENTER)) {
      InitGame();
      gameOver = false;
    }
  }
}

// Draw game (one frame)
void DrawGame(void) {
  BeginDrawing();

  ClearBackground(BLACK);

  if (!gameOver) {
    // Draw player bar
    DrawPlayer(player1);
    DrawPlayer(player2);
    // Draw ball
    DrawCircleV(ball.position, ball.radius, MAROON);

    if (pause)
      DrawText("GAME PAUSED",
               screenWidth / 2 - MeasureText("GAME PAUSED", 40) / 2,
               screenHeight / 2 - 40, 40, GRAY);
  } else
    DrawText("PRESS [ENTER] TO PLAY AGAIN",
             GetScreenWidth() / 2 -
                 MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20) / 2,
             GetScreenHeight() / 2 - 50, 20, GRAY);

  EndDrawing();
}

// Unload game variables
void UnloadGame(void) {
  // TODO: Unload all dynamic loaded data (textures, sounds, models...)
}

// Update and Draw (one frame)
void UpdateDrawFrame(void) {
  UpdateGame();
  DrawGame();
}
