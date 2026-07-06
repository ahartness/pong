#include "raylib.h"

struct Paddle {
  Rectangle bounds;
  float speed;
};

struct Ball {
  Vector2 position;
  Vector2 velocity;
  float radius;
};

int main() {
  constexpr int screenWidth = 800;
  constexpr int screenHeight = 450;
  constexpr float paddleWidth = 20.0f;
  constexpr float paddleHeight = 100.0f;
  constexpr float paddleMargin = 30.0f;
  constexpr float ballRadius = 10.0f;

  InitWindow(screenWidth, screenHeight, "Pong");
  SetTargetFPS(60);

  Paddle player{{paddleMargin, (screenHeight - paddleHeight) / 2.0f,
                 paddleWidth, paddleHeight},
                350.0f};
  Paddle ai{{screenWidth - paddleMargin - paddleWidth,
             (screenHeight - paddleHeight) / 2.0f, paddleWidth, paddleHeight},
            0.0f};
  Ball ball{
      {screenWidth / 2.0f, screenHeight / 2.0f}, {260.0f, 180.0f}, ballRadius};

  while (!WindowShouldClose()) {
    const float deltaTime = GetFrameTime();

    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) {
      player.bounds.y -= player.speed * deltaTime;
    }
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) {
      player.bounds.y += player.speed * deltaTime;
    }

    if (player.bounds.y < 0.0f) {
      player.bounds.y = 0.0f;
    }
    if (player.bounds.y + player.bounds.height > screenHeight) {
      player.bounds.y = screenHeight - player.bounds.height;
    }

    ball.position.x += ball.velocity.x * deltaTime;
    ball.position.y += ball.velocity.y * deltaTime;

    if (ball.position.y - ball.radius <= 0.0f ||
        ball.position.y + ball.radius >= screenHeight) {
      ball.velocity.y *= -1.0f;
      if (ball.position.y - ball.radius < 0.0f) {
        ball.position.y = ball.radius;
      } else if (ball.position.y + ball.radius > screenHeight) {
        ball.position.y = screenHeight - ball.radius;
      }
    }

    if (ball.position.x - ball.radius <=
            player.bounds.x + player.bounds.width &&
        ball.position.y >= player.bounds.y &&
        ball.position.y <= player.bounds.y + player.bounds.height &&
        ball.velocity.x < 0.0f) {
      ball.velocity.x *= -1.0f;
      ball.position.x = player.bounds.x + player.bounds.width + ball.radius;
    }

    if (ball.position.x + ball.radius >= ai.bounds.x &&
        ball.position.y >= ai.bounds.y &&
        ball.position.y <= ai.bounds.y + ai.bounds.height &&
        ball.velocity.x > 0.0f) {
      ball.velocity.x *= -1.0f;
      ball.position.x = ai.bounds.x - ball.radius;
    }

    if (ball.position.x + ball.radius < 0.0f ||
        ball.position.x - ball.radius > screenWidth) {
      ball.position = {screenWidth / 2.0f, screenHeight / 2.0f};
      ball.velocity.x *= -1.0f;
    }

    BeginDrawing();
    ClearBackground(BLACK);

    DrawLine(screenWidth / 2, 0, screenWidth / 2, screenHeight, DARKGRAY);
    DrawRectangleRec(player.bounds, WHITE);
    DrawRectangleRec(ai.bounds, WHITE);
    DrawCircleV(ball.position, ball.radius, WHITE);
    DrawText("W/S or Up/Down to move", 20, 20, 20, LIGHTGRAY);

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
