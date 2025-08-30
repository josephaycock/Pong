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

typedef struct {
    Vector2 position;
    Vector2 size;
    float speed;
    Color color;
} PaddleLeft;

typedef struct {
    Vector2 position;
    Vector2 size;
    float speed;
    Color color;
} PaddleRight;

typedef struct {
    Vector2 position;
    float radius;
    Vector2 velocity;
    Color color;
} Ball;

int main(void) {

    InitWindow(screenWidth, screenHeight, title);

    PaddleLeft leftPaddle = { {0, (screenHeight / 2) - 76}, {48, 152}, 1.0, DARKGREEN};
    PaddleRight rightPaddle = { {screenWidth - 48, (screenHeight / 2) - 76}, {48, 152}, 1.0, DARKGREEN};
    Ball ball = { { (screenWidth / 2) - 9, (screenHeight / 2) - 9}, 18, {2, 2}, DARKGREEN};

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();

        ClearBackground(LIME);

        DrawRectangleV(leftPaddle.position, leftPaddle.size, leftPaddle.color);
        DrawRectangleV(rightPaddle.position, rightPaddle.size, rightPaddle.color);
        DrawCircleV(ball.position, ball.radius, ball.color);

        EndDrawing();
    }

    CloseWindow();
    
    return 0;
}