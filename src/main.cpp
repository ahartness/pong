#include "raylib.h"
#include <cmath>

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

static float ClampFloat(float value, float minValue, float maxValue) {
  if (value < minValue) {
    return minValue;
  }
  if (value > maxValue) {
    return maxValue;
  }
  return value;
}

static float PredictBallYAtX(const Ball &ball, float targetX, float minY,
                             float maxY) {
  if (ball.velocity.x <= 0.0f) {
    return ball.position.y;
  }

  const float travelTime = (targetX - ball.position.x) / ball.velocity.x;
  if (travelTime <= 0.0f) {
    return ball.position.y;
  }

  float projectedY = ball.position.y + ball.velocity.y * travelTime;
  while (projectedY < minY || projectedY > maxY) {
    if (projectedY < minY) {
      projectedY = minY + (minY - projectedY);
    } else {
      projectedY = maxY - (projectedY - maxY);
    }
  }

  return projectedY;
}

static void BounceBallOffPaddle(Ball &ball, const Paddle &paddle,
                                float horizontalDirection,
                                float maxVerticalSpeed,
                                float minHorizontalSpeed,
                                float verticalSpeedJitter) {
  const float paddleCenterY = paddle.bounds.y + paddle.bounds.height / 2.0f;
  const float hitOffset = ball.position.y - paddleCenterY;
  const float normalizedHitOffset =
      ClampFloat(hitOffset / (paddle.bounds.height / 2.0f), -1.0f, 1.0f);

  const float jitter =
      GetRandomValue(static_cast<int>(-verticalSpeedJitter),
                     static_cast<int>(verticalSpeedJitter));
  const float newVerticalSpeed = ClampFloat(
      normalizedHitOffset * maxVerticalSpeed + jitter, -maxVerticalSpeed,
      maxVerticalSpeed);
  const float currentSpeed = std::sqrt(ball.velocity.x * ball.velocity.x +
                                       ball.velocity.y * ball.velocity.y);
  const float minHorizontalSpeedSq = minHorizontalSpeed * minHorizontalSpeed;
  float horizontalSpeedSq =
      currentSpeed * currentSpeed - newVerticalSpeed * newVerticalSpeed;
  if (horizontalSpeedSq < minHorizontalSpeedSq) {
    horizontalSpeedSq = minHorizontalSpeedSq;
  }

  ball.velocity.x = std::sqrt(horizontalSpeedSq) *
                    (horizontalDirection < 0.0f ? -1.0f : 1.0f);
  ball.velocity.y = newVerticalSpeed;
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
  constexpr float paddleBounceMaxVerticalSpeed = 280.0f;
  constexpr float paddleBounceMinHorizontalSpeed = 180.0f;
  constexpr float aiDeadZone = 20.0f;
  constexpr float aiMinReactionDelay = 0.10f;
  constexpr float aiMaxReactionDelay = 0.50f;
  constexpr float aiAimError = 100.0f;
  constexpr float aiBounceVerticalJitter = 55.0f;

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
            320.0f,
            0};
  Ball ball{{screenWidth / 2.0f, screenHeight / 2.0f},
            CreateServeVelocity(ballHorizontalSpeed, ballVerticalSpeed),
            ballRadius};
  float aiThinkTimer = 0.0f;
  float aiTargetY = screenHeight / 2.0f;

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

    aiThinkTimer -= deltaTime;
    if (aiThinkTimer <= 0.0f) {
      aiThinkTimer =
          GetRandomValue(static_cast<int>(aiMinReactionDelay * 1000.0f),
                         static_cast<int>(aiMaxReactionDelay * 1000.0f)) /
          1000.0f;

      if (ball.velocity.x > 0.0f) {
        const float predictedY = PredictBallYAtX(ball, ai.bounds.x, ball.radius,
                                                 screenHeight - ball.radius);
        const float aimOffset = GetRandomValue(static_cast<int>(-aiAimError),
                                               static_cast<int>(aiAimError));
        aiTargetY = predictedY + aimOffset;
      } else {
        aiTargetY = screenHeight / 2.0f;
      }
    }

    const float aiCenterY = ai.bounds.y + ai.bounds.height / 2.0f;
    const float targetY = ClampFloat(aiTargetY, ai.bounds.height / 2.0f,
                                     screenHeight - ai.bounds.height / 2.0f);
    const float aiDeltaY = targetY - aiCenterY;
    if (aiDeltaY > aiDeadZone) {
      ai.bounds.y += ai.speed * deltaTime;
    } else if (aiDeltaY < -aiDeadZone) {
      ai.bounds.y -= ai.speed * deltaTime;
    }

    // Clamp the player paddle so it cannot leave the screen.
    if (player.bounds.y < 0.0f) {
      player.bounds.y = 0.0f;
    }
    if (player.bounds.y + player.bounds.height > screenHeight) {
      player.bounds.y = screenHeight - player.bounds.height;
    }
    if (ai.bounds.y < 0.0f) {
      ai.bounds.y = 0.0f;
    }
    if (ai.bounds.y + ai.bounds.height > screenHeight) {
      ai.bounds.y = screenHeight - ai.bounds.height;
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
      BounceBallOffPaddle(ball, player, 1.0f, paddleBounceMaxVerticalSpeed,
                          paddleBounceMinHorizontalSpeed, 0.0f);
      ball.position.x = player.bounds.x + player.bounds.width + ball.radius;
    }

    // Bounce the ball when it hits the AI paddle.
    if (ball.position.x + ball.radius >= ai.bounds.x &&
        ball.position.y >= ai.bounds.y &&
        ball.position.y <= ai.bounds.y + ai.bounds.height &&
        ball.velocity.x > 0.0f) {
      BounceBallOffPaddle(ball, ai, -1.0f, paddleBounceMaxVerticalSpeed,
                          paddleBounceMinHorizontalSpeed,
                          aiBounceVerticalJitter);
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
