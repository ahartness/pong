#include "raylib.h"

struct Paddle {
  Rectangle bounds;
  float speed;
  int score;
};

struct Ball {
  Vector2 position;
  Vector2 velocity;
  float radius;
};

static Vector2 CreateServeVelocity(float horizontalSpeed, float verticalSpeed) {
  return {GetRandomValue(0, 1) == 0 ? -horizontalSpeed : horizontalSpeed,
          GetRandomValue(0, 1) == 0 ? -verticalSpeed : verticalSpeed};
}

int main() {
  // Define the fixed dimensions and gameplay measurements used across the
  // scene.
  constexpr int screenWidth = 800;
  constexpr int screenHeight = 450;
  constexpr int scoreFontSize = 20;
  constexpr int scoreMargin = 20;
  constexpr float paddleWidth = 20.0f;
  constexpr float paddleHeight = 100.0f;
  constexpr float paddleMargin = 30.0f;
  constexpr float ballRadius = 10.0f;
  constexpr float ballHorizontalSpeed = 260.0f;
  constexpr float ballVerticalSpeed = 180.0f;

  // Create the game window and lock the loop to a stable frame rate.
  InitWindow(screenWidth, screenHeight, "Pong");
  SetTargetFPS(60);

  // Set up the initial positions and movement speeds for both paddles and the
  // ball.
  Paddle player{{paddleMargin, (screenHeight - paddleHeight) / 2.0f,
                 paddleWidth, paddleHeight},
                350.0f,
                0};
  Paddle ai{{screenWidth - paddleMargin - paddleWidth,
             (screenHeight - paddleHeight) / 2.0f, paddleWidth, paddleHeight},
            0.0f,
            0};
  Ball ball{{screenWidth / 2.0f, screenHeight / 2.0f},
            CreateServeVelocity(ballHorizontalSpeed, ballVerticalSpeed),
            ballRadius};

  while (!WindowShouldClose()) {
    // Use frame time so movement stays consistent across different machines.
    const float deltaTime = GetFrameTime();

    // Move the player paddle based on keyboard input.
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) {
      player.bounds.y -= player.speed * deltaTime;
    }
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) {
      player.bounds.y += player.speed * deltaTime;
    }

    // Clamp the player paddle so it cannot leave the screen.
    if (player.bounds.y < 0.0f) {
      player.bounds.y = 0.0f;
    }
    if (player.bounds.y + player.bounds.height > screenHeight) {
      player.bounds.y = screenHeight - player.bounds.height;
    }

    // Advance the ball using its current velocity.
    ball.position.x += ball.velocity.x * deltaTime;
    ball.position.y += ball.velocity.y * deltaTime;

    // Bounce the ball off the top and bottom edges and keep it in bounds.
    if (ball.position.y - ball.radius <= 0.0f ||
        ball.position.y + ball.radius >= screenHeight) {
      ball.velocity.y *= -1.0f;
      if (ball.position.y - ball.radius < 0.0f) {
        ball.position.y = ball.radius;
      } else if (ball.position.y + ball.radius > screenHeight) {
        ball.position.y = screenHeight - ball.radius;
      }
    }

    // Bounce the ball when it hits the player's paddle.
    if (ball.position.x - ball.radius <=
            player.bounds.x + player.bounds.width &&
        ball.position.y >= player.bounds.y &&
        ball.position.y <= player.bounds.y + player.bounds.height &&
        ball.velocity.x < 0.0f) {
      ball.velocity.x *= -1.0f;
      ball.position.x = player.bounds.x + player.bounds.width + ball.radius;
    }

    // Bounce the ball when it hits the AI paddle.
    if (ball.position.x + ball.radius >= ai.bounds.x &&
        ball.position.y >= ai.bounds.y &&
        ball.position.y <= ai.bounds.y + ai.bounds.height &&
        ball.velocity.x > 0.0f) {
      ball.velocity.x *= -1.0f;
      ball.position.x = ai.bounds.x - ball.radius;
    }

    // Reset the ball to center after it travels past the player.
    if (ball.position.x + ball.radius < 0.0f) {
      ai.score++;
      ball.position = {screenWidth / 2.0f, screenHeight / 2.0f};
      ball.velocity =
          CreateServeVelocity(ballHorizontalSpeed, ballVerticalSpeed);
    }

    // Reset the ball to center after it travels past the ai.
    if (ball.position.x - ball.radius > screenWidth) {
      player.score++;
      ball.position = {screenWidth / 2.0f, screenHeight / 2.0f};
      ball.velocity =
          CreateServeVelocity(ballHorizontalSpeed, ballVerticalSpeed);
    }

    // Draw the current frame: background, center line, game objects, and
    // controls.
    BeginDrawing();
    ClearBackground(BLACK);

    DrawLine(screenWidth / 2, 0, screenWidth / 2, screenHeight, DARKGRAY);
    DrawRectangleRec(player.bounds, WHITE);
    DrawRectangleRec(ai.bounds, WHITE);
    DrawCircleV(ball.position, ball.radius, WHITE);
    const char *playerScoreText = TextFormat("Player: %i", player.score);
    const char *aiScoreText = TextFormat("AI: %i", ai.score);
    DrawText(playerScoreText, scoreMargin, scoreMargin, scoreFontSize,
             LIGHTGRAY);
    DrawText(aiScoreText,
             screenWidth - scoreMargin -
                 MeasureText(aiScoreText, scoreFontSize),
             scoreMargin, scoreFontSize, LIGHTGRAY);

    EndDrawing();
  }

  // Release the window resources before exiting.
  CloseWindow();
  return 0;
}
