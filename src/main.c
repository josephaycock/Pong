#include <raylib.h>
#include <raymath.h>

/* 
*  Template 5.5 - Basic window 
*  ----------------------------------------------------------------------------------
*  To run example, just press F5 or go to Build and Run
*  or
*  Manually complile using the following script: 
*
*  make build_osx
*  ./bin/build_osx
*/

const int screenWidth = 1280;
const int screenHeight = 720;
const char* title = "Pong";
const float PADDLE_SPEED = 600.0f; 
const int WINNING_SCORE = 2;

typedef enum {
    GAME_START,
    GAME_SERVE,
    GAME_PLAYING,
    GAME_OVER
} GameState;

typedef struct {
    Vector2 position;
    Vector2 size;
    Color color;
} Paddle;

typedef struct {
    Vector2 position;
    float radius;
    Vector2 velocity;
    Color color;
} Ball;

int main(void) {

    InitWindow(screenWidth, screenHeight, title);

    Paddle player1 = { {0, (screenHeight / 2) - 76}, {48, 152}, DARKGREEN};
    Paddle player2 = { {screenWidth - 48, (screenHeight / 2) - 76}, {48, 152}, DARKGREEN};
    Ball ball = { { (screenWidth / 2) - 9, (screenHeight / 2) - 9}, 18, {2, 2}, DARKGREEN};

    GameState gameState = GAME_START;
    int score1 = 0, score2 = 0;
    int serveDirection = 1;

    SetTargetFPS(60);

    while (!WindowShouldClose()) {

        // --- Update ---
        float dt = GetFrameTime();
        
        switch (gameState) {
            case GAME_START:
                if (IsKeyPressed(KEY_SPACE)) {
                    gameState = GAME_SERVE;
                }
                break;
            case GAME_SERVE:
                ball.position = (Vector2){ screenWidth/2.0f, screenHeight/2.0f };
                ball.velocity = (Vector2){ 0, 0 };
                if (IsKeyPressed(KEY_SPACE)) {
                    ball.velocity = (Vector2){ 400.0f * serveDirection, 260.0f };
                    gameState = GAME_PLAYING;
                }
                break;
            case GAME_PLAYING:
                if (IsKeyDown(KEY_W)) player1.position.y -= PADDLE_SPEED * dt;
                if (IsKeyDown(KEY_S)) player1.position.y += PADDLE_SPEED * dt;

                if (IsKeyDown(KEY_UP)) player2.position.y -= PADDLE_SPEED * dt;
                if (IsKeyDown(KEY_DOWN)) player2.position.y += PADDLE_SPEED * dt;

                player1.position.y = Clamp(player1.position.y, 0, screenHeight - player1.size.y);
                player2.position.y = Clamp(player2.position.y, 0, screenHeight - player2.size.y);

                ball.position.x += ball.velocity.x * dt;
                ball.position.y += ball.velocity.y * dt;

                if (ball.position.y - ball.radius <= 0) {
                    ball.position.y = ball.radius;
                    ball.velocity.y *= -1;
                }

                if (ball.position.y + ball.radius >= screenHeight) {
                    ball.position.y = screenHeight - ball.radius;
                    ball.velocity.y *= -1;
                }

                Rectangle ballCollision = {
                    ball.position.x - ball.radius,
                    ball.position.y - ball.radius,
                    ball.radius * 2,
                    ball.radius * 2
                };

                Rectangle player1Collision = {
                    player1.position.x,
                    player1.position.y,
                    player1.size.x,
                    player1.size.y
                };

                Rectangle player2Collision = {
                    player2.position.x,
                    player2.position.y,
                    player2.size.x,
                    player2.size.y
                };

                if (CheckCollisionRecs(ballCollision, player1Collision)) {
                    ball.velocity.x *= -1;
                    ball.position.x = player1.position.x + player1.size.x + ball.radius;
                }

                if (CheckCollisionRecs(ballCollision, player2Collision)) {
                    ball.velocity.x *= -1;
                    ball.position.x = player2.position.x - ball.radius;
                }

                if (ball.position.x + ball.radius < 0) {
                    score2++;
                    if (score2 >= WINNING_SCORE) {
                        gameState = GAME_OVER;
                        break;
                    }
                    serveDirection = 1;
                    gameState = GAME_SERVE;
                }

                if (ball.position.x - ball.radius > screenWidth) {
                    score1++;
                    if (score1 >= WINNING_SCORE) {
                        gameState = GAME_OVER;
                        break;
                    }
                    serveDirection = -1;
                    gameState = GAME_SERVE;
                }
                break;
            case GAME_OVER:
                if (IsKeyPressed(KEY_SPACE)) {
                    score1 = 0;
                    score2 = 0;
                    serveDirection = (GetRandomValue(0,1) == 0) ? -1 : 1;
                    gameState = GAME_START;
                }
                break;
        }

        // --- Drawing ---
        BeginDrawing();
        ClearBackground(LIME);

        switch (gameState) {
            case GAME_START:
                DrawText("PONG", screenWidth / 2 - 44, screenHeight / 2 - 24, 48, DARKGREEN);
                DrawText("Press SPACE to Start!", screenWidth / 2 - 100, screenHeight / 2 + 32, 24, DARKGREEN);
                break;
            case GAME_SERVE:
                DrawRectangleV(player1.position, player1.size, player1.color);
                DrawRectangleV(player2.position, player2.size, player2.color);
                DrawCircleV(ball.position, ball.radius, ball.color);
                DrawText("Press SPACE to Serve!", screenWidth / 2 - 80, screenHeight / 2 - 64, 24, DARKGREEN);
                DrawText(TextFormat("%d", score1), screenWidth/4, 20, 40, DARKGREEN);
                DrawText(TextFormat("%d", score2), screenWidth*3/4, 20, 40, DARKGREEN);
                break;
            case GAME_PLAYING:
                DrawRectangleV(player1.position, player1.size, player1.color);
                DrawRectangleV(player2.position, player2.size, player2.color);
                DrawCircleV(ball.position, ball.radius, ball.color);
                DrawText(TextFormat("%d", score1), screenWidth/4, 20, 40, DARKGREEN);
                DrawText(TextFormat("%d", score2), screenWidth*3/4, 20, 40, DARKGREEN);
                break;
            case GAME_OVER:
                DrawText("GAME OVER", screenWidth / 2 - 100, screenHeight / 2 - 24, 48, DARKGREEN);
                if (score1 >= WINNING_SCORE) {
                    DrawText("Player 1 Wins!", screenWidth / 2 - 100, screenHeight / 2 + 32, 24, DARKGREEN);
                } else {
                    DrawText("Player 2 Wins!", screenWidth / 2 - 100, screenHeight / 2 + 32, 24, DARKGREEN);
                }
                DrawText("Press SPACE to Exit!", screenWidth / 2 - 100, screenHeight / 2 + 64, 24, DARKGREEN);
                break;
        }

        EndDrawing();
    }

    CloseWindow();
    
    return 0;
}