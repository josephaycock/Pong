#include <raylib.h>
#include <raymath.h>
#include <stdbool.h>

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
const int WINNING_SCORE = 3;
const float BALL_SPEED_INCREMENT = 1.03f;
const float BALL_MAX_SPEED = 1500.0f;
const float MAX_DEFLECTION_ANGLE = 5 * (PI / 12); // 75 degrees in radians
const float SIDE_PADDING = 32.0f;   // Inset from left/right edges

typedef enum {
    GAME_START,
    GAME_SERVE,
    GAME_PLAYING,
    GAME_PAUSE,
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

// Draw a dashed center line
static void DrawCenterLine(int w, int h, Color color) {
    int segmentHeight = 20;   // height of each dash
    int gap = 20;             // space between dashes
    int lineX = w / 2 - 2;    // 4px wide line centered

    for (int y = 0; y < h; y += segmentHeight + gap) {
        DrawRectangle(lineX, y, 4, segmentHeight, color);
    }
}

int main(void) {
    // --- Initialization ---
    // Enable V-Sync
    SetConfigFlags(FLAG_VSYNC_HINT);

    InitWindow(screenWidth, screenHeight, title);

    // --- Game variables ---
    Paddle player1 = { {SIDE_PADDING, (screenHeight - 120) * 0.5f}, {16, 120}, DARKGREEN };
    Paddle player2 = { {screenWidth - SIDE_PADDING - 16, (screenHeight - 120) * 0.5f}, {16, 120}, DARKGREEN };
    Ball ball = { { (screenWidth / 2) - 9, (screenHeight / 2) - 9}, 8, {2, 2}, DARKGREEN};

    // --- Game state variables ---
    GameState gameState = GAME_START;
    int score1 = 0, score2 = 0;
    int serveDirection = 1;
    bool serveJustHappened = true;

    // Main game loop
    while (!WindowShouldClose()) {

        // --- Update ---
        float dt = GetFrameTime();
        
        switch (gameState) {
            case GAME_START:
                if (IsKeyPressed(KEY_SPACE)) {
                    serveJustHappened = true;
                    gameState = GAME_SERVE;
                }
                break;
            case GAME_SERVE:
                // Reset ball position if a serve just happened
                if (serveJustHappened) {
                    ball.position = (Vector2){ screenWidth/2.0f, screenHeight/2.0f };
                    ball.velocity = (Vector2){ 0, 0 };
                    // Does not recenter
                    serveJustHappened = false;
                }

                // Allow paddle movement during serve
                if (IsKeyDown(KEY_W))      player1.position.y -= PADDLE_SPEED * dt;
                if (IsKeyDown(KEY_S))      player1.position.y += PADDLE_SPEED * dt;
                if (IsKeyDown(KEY_UP))     player2.position.y -= PADDLE_SPEED * dt;
                if (IsKeyDown(KEY_DOWN))   player2.position.y += PADDLE_SPEED * dt;

                player1.position.y = Clamp(player1.position.y, 0, screenHeight - player1.size.y);
                player2.position.y = Clamp(player2.position.y, 0, screenHeight - player2.size.y);

                // Serve the ball
                if (IsKeyPressed(KEY_SPACE)) {
                    // Small random angle so serves aren’t identical
                    float ang = DEG2RAD * (float)GetRandomValue(-20, 20);
                    Vector2 dir = Vector2Normalize((Vector2){ serveDirection * cosf(ang), sinf(ang) });
                    ball.velocity = Vector2Scale(dir, 480.0f);  // ~same speed as before
                    gameState = GAME_PLAYING;
                }
                break;
            case GAME_PLAYING:
                if (IsKeyPressed(KEY_P)) {
                    gameState = GAME_PAUSE;
                    break;
                }

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
                    ball.radius * 2.0f,
                    ball.radius * 2.0f
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

                // Player 1 collision with angle calculation
                if (CheckCollisionRecs(ballCollision, player1Collision) && ball.velocity.x < 0) {
                    // Calculate hit position relative to paddle center
                    float paddleCenterY = player1.position.y + (player1.size.y / 2);
                    float t = (ball.position.y - paddleCenterY) / (player1.size.y / 2);
                    t = Clamp(t, -1.0f, 1.0f); // Ensure t is within [-1, 1]

                    // Calculate deflection angle
                    float deflectionAngle = t * MAX_DEFLECTION_ANGLE;

                    // Calculate speed
                    float currentSpeed = Vector2Length(ball.velocity);
                    float newSpeed = fminf(currentSpeed * BALL_SPEED_INCREMENT, BALL_MAX_SPEED);

                    // Update ball velocity based on deflection angle
                    Vector2 direction = Vector2Normalize((Vector2){ cosf(deflectionAngle), sinf(deflectionAngle) });
                    ball.velocity = Vector2Scale(direction, newSpeed); 

                    // Nudge ball out of paddle
                    ball.position.x = player1.position.x + player1.size.x + ball.radius;
                }

                // Player 2 collision with angle calculation
                if (CheckCollisionRecs(ballCollision, player2Collision) && ball.velocity.x > 0) {
                    // Calculate hit position relative to paddle center
                    float paddleCenterY = player2.position.y + (player2.size.y / 2);
                    float t = (ball.position.y - paddleCenterY) / (player2.size.y / 2);
                    t = Clamp(t, -1.0f, 1.0f); // Ensure t is within [-1, 1]

                    // Calculate deflection angle
                    float deflectionAngle = t * MAX_DEFLECTION_ANGLE;

                    // Calculate speed
                    float currentSpeed = Vector2Length(ball.velocity);
                    float newSpeed = fminf(currentSpeed * BALL_SPEED_INCREMENT, BALL_MAX_SPEED);

                    // Update ball velocity based on deflection angle
                    Vector2 direction = Vector2Normalize((Vector2){ -cosf(deflectionAngle), sinf(deflectionAngle) });
                    ball.velocity = Vector2Scale(direction, newSpeed); 

                    // Nudge ball out of paddle
                    ball.position.x = player2.position.x - ball.radius;
                }

                if (ball.position.x + ball.radius < 0) {
                    score2++;
                    if (score2 >= WINNING_SCORE) {
                        gameState = GAME_OVER;
                        break;
                    }
                    serveDirection = 1;
                    serveJustHappened = true;
                    gameState = GAME_SERVE;
                }

                if (ball.position.x - ball.radius > screenWidth) {
                    score1++;
                    if (score1 >= WINNING_SCORE) {
                        gameState = GAME_OVER;
                        break;
                    }
                    serveDirection = -1;
                    serveJustHappened = true;
                    gameState = GAME_SERVE;
                }
                break;
            case GAME_PAUSE:
                if (IsKeyPressed(KEY_P)) {
                    gameState = GAME_PLAYING; // Resume game
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
        ClearBackground(GREEN);

        switch (gameState) {
            case GAME_START:
                // Bar under the title (longer + thicker)
                DrawRectangle(screenWidth / 2 - 150, screenHeight / 2 - 60, 300, 6, DARKGREEN);

                // Title
                DrawText("PONG", screenWidth / 2 - MeasureText("PONG", 64) / 2, screenHeight / 2 - 120, 64, DARKGREEN);

                // Prompt
                DrawText("Press SPACE to Start!", screenWidth / 2 - MeasureText("Press SPACE to Start!", 24) / 2, screenHeight / 2 + 10, 24, DARKGREEN);

                // Instructions
                DrawText("How to Play:", screenWidth / 2 - MeasureText("How to Play:", 24) / 2, screenHeight / 2 + 50, 24, DARKGREEN);
                DrawText("First to 3 Points Wins!", screenWidth / 2 - MeasureText("First to 3 Points Wins!", 24) / 2, screenHeight / 2 + 80, 24, DARKGREEN);

                // Bottom hints
                DrawText("Player 1: W/S keys", screenWidth / 4 - MeasureText("Player 1: W/S keys", 20) / 2, screenHeight - 40, 20, DARKGREEN);
                DrawText("Player 2: Up/Down keys", screenWidth * 3 / 4 - MeasureText("Player 2: Up/Down keys", 20) / 2, screenHeight - 40, 20, DARKGREEN);

                // Pause hint (top center)
                DrawText("Press P to Pause during play", screenWidth / 2 - MeasureText("Press P to Pause during play", 20) / 2, 20, 20, DARKGREEN);
                break;
            case GAME_SERVE:
                // Draw center line
                // DrawCenterLine(screenWidth, screenHeight, DARKGREEN);

                // Draw paddles and ball
                DrawRectangleV(player1.position, player1.size, player1.color);
                DrawRectangleV(player2.position, player2.size, player2.color);
                DrawCircleV(ball.position, ball.radius, ball.color);

                // Text prompt centered
                int serveFontSize = 32;
                const char *serveText = "Press SPACE to Serve!";
                DrawText(serveText,
                        screenWidth / 2 - MeasureText(serveText, serveFontSize) / 2,
                        screenHeight / 2 - 120,
                        serveFontSize,
                        DARKGREEN);

                // Scores centered in their quarters
                int scoreFontSize = 56;
                const char *s1 = TextFormat("%d", score1);
                const char *s2 = TextFormat("%d", score2);

                DrawText(s1,
                        screenWidth / 4 - MeasureText(s1, scoreFontSize) / 2,
                        20,
                        scoreFontSize,
                        DARKGREEN);

                DrawText(s2,
                        screenWidth * 3 / 4 - MeasureText(s2, scoreFontSize) / 2,
                        20,
                        scoreFontSize,
                        DARKGREEN);
                break;
            case GAME_PLAYING:
                // Draw center line
                DrawCenterLine(screenWidth, screenHeight, DARKGREEN);

                // Draw paddles and ball
                DrawRectangleV(player1.position, player1.size, player1.color);
                DrawRectangleV(player2.position, player2.size, player2.color);
                DrawCircleV(ball.position, ball.radius, ball.color);

                // Scores centered
                {
                    int scoreFont = 56;
                    const char *s1 = TextFormat("%d", score1);
                    const char *s2 = TextFormat("%d", score2);

                    DrawText(s1,
                            screenWidth/4 - MeasureText(s1, scoreFont)/2,
                            20,
                            scoreFont,
                            DARKGREEN);

                    DrawText(s2,
                            (screenWidth*3)/4 - MeasureText(s2, scoreFont)/2,
                            20,
                            scoreFont,
                            DARKGREEN);
                }
                break;
            case GAME_PAUSE:
                // Draw center line
                // DrawCenterLine(screenWidth, screenHeight, DARKGREEN);

                // Draw paddles and ball
                DrawRectangleV(player1.position, player1.size, player1.color);
                DrawRectangleV(player2.position, player2.size, player2.color);
                DrawCircleV(ball.position, ball.radius, ball.color);

                // Scores centered
                {
                    int scoreFont = 56;
                    const char *s1 = TextFormat("%d", score1);
                    const char *s2 = TextFormat("%d", score2);

                    DrawText(s1,
                            screenWidth/4 - MeasureText(s1, scoreFont)/2,
                            20,
                            scoreFont,
                            DARKGREEN);

                    DrawText(s2,
                            (screenWidth*3)/4 - MeasureText(s2, scoreFont)/2,
                            20,
                            scoreFont,
                            DARKGREEN);
                }

                // Pause texts centered
                {
                    int titleFont = 48;
                    int hintFont  = 24;
                    const char *t  = "PAUSED";
                    const char *h  = "Press P to Resume";

                    DrawText(t,
                            screenWidth/2 - MeasureText(t, titleFont)/2,
                            screenHeight/2 - 40,
                            titleFont,
                            DARKGREEN);

                    DrawText(h,
                            screenWidth/2 - MeasureText(h, hintFont)/2,
                            screenHeight/2 + 10,
                            hintFont,
                            DARKGREEN);
                }
                break;
            case GAME_OVER:
            {
                // Scores (top, centered in quarters)
                int scoreFont = 56;
                const char *s1 = TextFormat("%d", score1);
                const char *s2 = TextFormat("%d", score2);

                int s1W = MeasureText(s1, scoreFont);
                int s2W = MeasureText(s2, scoreFont);

                int sY = 40;                 // score Y
                int barH = 5;                // underline thickness
                int barPad = 20;             // extra width beyond text
                int barGap = 6;              // gap between text baseline and bar

                // Left score
                int s1X = screenWidth/4 - s1W/2;
                DrawText(s1, s1X, sY, scoreFont, DARKGREEN);

                // Right score
                int s2X = (screenWidth*3)/4 - s2W/2;
                DrawText(s2, s2X, sY, scoreFont, DARKGREEN);

                // Only underline the winner
                if (score1 >= WINNING_SCORE) {
                    DrawRectangle(s1X - barPad/2, sY + scoreFont + barGap, s1W + barPad, barH, DARKGREEN);
                } 
                else if (score2 >= WINNING_SCORE) {
                    DrawRectangle(s2X - barPad/2, sY + scoreFont + barGap, s2W + barPad, barH, DARKGREEN);
                }

                // Main title
                int titleFont = 48;
                const char *title = "GAME OVER";
                DrawText(title,
                        screenWidth/2 - MeasureText(title, titleFont)/2,
                        screenHeight/2 - 100,
                        titleFont,
                        DARKGREEN);

                // Winner line
                int winFont = 28;
                const char *winner = (score1 >= WINNING_SCORE) ? "Player 1 Wins!" : "Player 2 Wins!";
                DrawText(winner,
                        screenWidth/2 - MeasureText(winner, winFont)/2,
                        screenHeight/2 - 40,
                        winFont,
                        DARKGREEN);

                // Replay hint
                int hintFont = 32;
                const char *hint = "Press SPACE to Replay!";
                DrawText(hint,
                        screenWidth/2 - MeasureText(hint, hintFont)/2,
                        screenHeight/2 + 20,
                        hintFont,
                        DARKGREEN);
            }
                break;
        }

        EndDrawing();
    }

    CloseWindow();
    
    return 0;
}