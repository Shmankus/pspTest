#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspctrl.h>
#include <pspgu.h>
#include <pspgum.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "font.c"
#include "renderables.c"

// TO-FIX
// - Ball can get stuck inside of the rectangle, create a top and bottom collision check

// TO-DO
// - add an angle on how the ball gets bounces based on how far along the paddle it is

const int AI_ENABLED = 1;
unsigned int gameRunning = 1;

PSP_MODULE_INFO("HelloWorldCentered", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

enum
{
    SCREEN_W = 480,
    SCREEN_H = 272,
    VRAM_STRIDE = 512
};

static unsigned int __attribute__((aligned(16))) gu_list[262144];

typedef struct
{
    float y;
    float x;
    float radius;
    float speed;
    float dirX;
    float dirY;
    unsigned int color;
    int segments;
} Ball;

typedef struct
{
    float y;
    float x;
    float height;
    float width;
    float speed;
    int score
} Player;

int checkPlayerCollision(Player *leftPlayer, Player *rightPlayer, Ball *ball)
{

    // left player collision check
    if (ball->x - ball->radius < 0.0f)
    {
        ball->x = SCREEN_W / 2.0f;
        ball->y = SCREEN_H / 2.0f;
        rightPlayer->score++;
    }
    else if ((ball->y + ball->radius > leftPlayer->y - leftPlayer->height / 2.0f) && (ball->y - ball->radius < leftPlayer->y + leftPlayer->height / 2.0f))
    {
        if ((ball->x - ball->radius < leftPlayer->x + leftPlayer->width))
        {
            ball->dirX = -ball->dirX;
        }
    }

    // right player collision check
    if (ball->x + ball->radius > SCREEN_W)
    {
        ball->x = SCREEN_W / 2.0f;
        ball->y = SCREEN_H / 2.0f;
        leftPlayer->score++;
    }
    else if ((ball->y + ball->radius > rightPlayer->y - rightPlayer->height / 2.0f) && (ball->y - ball->radius < rightPlayer->y + rightPlayer->height / 2.0f))
    {
        if ((ball->x + ball->radius > rightPlayer->x))
        {
            ball->dirX = -ball->dirX;
        }
    }

    return 0;
}

int render(Player *leftPlayer, Player *rightPlayer, Ball *ball)
{
    // Draw every frame and swap buffers
    sceGuStart(GU_DIRECT, gu_list);
    sceGuClearColor(0xFF330033);
    sceGuClear(GU_COLOR_BUFFER_BIT);
    sceGuDisable(GU_TEXTURE_2D);

    // Right side player that has movement functionality
    Vertex *controllablePlayerVertices = (Vertex *)sceGuGetMemory(2 * sizeof(Vertex));
    controllablePlayerVertices[0].color = 0xFFFF0000;
    controllablePlayerVertices[0].x = rightPlayer->x;
    controllablePlayerVertices[0].y = rightPlayer->y - (rightPlayer->height / 2.0f);
    controllablePlayerVertices[0].z = 0.0f;
    controllablePlayerVertices[1].color = 0xFFFF0000;
    controllablePlayerVertices[1].x = rightPlayer->x + rightPlayer->width;
    controllablePlayerVertices[1].y = rightPlayer->y + rightPlayer->height / 2.0f;
    controllablePlayerVertices[1].z = 0.0f;
    sceGuDrawArray(GU_SPRITES, GU_COLOR_8888 | GU_VERTEX_32BITF | GU_TRANSFORM_2D, 2, NULL, controllablePlayerVertices);

    // Left side player that will be controlled by an AI
    Vertex *aiPlayerVertices = (Vertex *)sceGuGetMemory(2 * sizeof(Vertex));
    aiPlayerVertices[0].color = 0xFFFF0000;
    aiPlayerVertices[0].x = leftPlayer->x;
    aiPlayerVertices[0].y = leftPlayer->y - (leftPlayer->height / 2.0f);
    aiPlayerVertices[0].z = 0.0f;
    aiPlayerVertices[1].color = 0xFFFF0000;
    aiPlayerVertices[1].x = leftPlayer->x + leftPlayer->width;
    aiPlayerVertices[1].y = leftPlayer->y + leftPlayer->height / 2.0f;
    aiPlayerVertices[1].z = 0.0f;
    sceGuDrawArray(GU_SPRITES, GU_COLOR_8888 | GU_VERTEX_32BITF | GU_TRANSFORM_2D, 2, NULL, aiPlayerVertices);

    // Draw the ball in the center of the screen
    circle(ball->x, ball->y, ball->radius, ball->color, ball->segments);

    sceGuEnable(GU_TEXTURE_2D); // starts for text
    // Write score to screen
    char leftScore[3];
    snprintf(leftScore, sizeof(leftScore), "%i", leftPlayer->score);
    draw_text(leftScore, (SCREEN_W - measure_text_width(leftScore)) / 4.0f, (SCREEN_H - 16) / 2, 0xFFFFFFFF);
    char rightScore[3];
    snprintf(rightScore, sizeof(rightScore), "%i", rightPlayer->score);
    draw_text(rightScore, ((SCREEN_W - (measure_text_width(rightScore)) / 2.0f)) * .75f, (SCREEN_H - 16) / 2, 0xFFFFFFFF);
    sceGuDisable(GU_TEXTURE_2D); // ends for text

    sceGuFinish();
    sceGuSync(0, 0);
    sceDisplayWaitVblankStart();
    sceGuSwapBuffers();

    return 0;
}

int aiPlayerHandler(Player *leftPlayer, Ball *ball)
{

    if (leftPlayer->y < ball->y)
    {
        leftPlayer->y += leftPlayer->speed;
    }
    else if (leftPlayer->y > ball->y)
    {
        leftPlayer->y -= leftPlayer->speed;
    }
}

int handleControls(Player *leftPlayer, Player *rightPlayer, SceCtrlData *pad)
{

    sceCtrlReadBufferPositive(pad, 1);

    // Exit game using home
    if (pad->Buttons & PSP_CTRL_HOME)
    {
        gameRunning = 0;
    }

    if (pad->Buttons & PSP_CTRL_TRIANGLE)
    {
        rightPlayer->y -= rightPlayer->speed;
        if (rightPlayer->y - rightPlayer->height / 2.0f < 0.0f)
        {
            rightPlayer->y = rightPlayer->height / 2.0f;
        }
    }
    else if (pad->Buttons & PSP_CTRL_CROSS)
    {
        rightPlayer->y += rightPlayer->speed;
        if (rightPlayer->y + rightPlayer->height / 2.0f > SCREEN_H)
        {
            rightPlayer->y = SCREEN_H - rightPlayer->height / 2.0f;
        }
    }

    if (AI_ENABLED == 0)
    {
        if (pad->Buttons & PSP_CTRL_UP)
        {
            leftPlayer->y -= leftPlayer->speed;
            if (leftPlayer->y - leftPlayer->height / 2.0f < 0.0f)
            {
                leftPlayer->y = leftPlayer->height / 2.0f;
            }
        }
        else if (pad->Buttons & PSP_CTRL_DOWN)
        {
            leftPlayer->y += leftPlayer->speed;
            if (leftPlayer->y + leftPlayer->height / 2.0f > SCREEN_H)
            {
                leftPlayer->y = SCREEN_H - leftPlayer->height / 2.0f;
            }
        }
    }

    return 0;
}


#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

int gamePhysics(Ball *ball)
{

    ball->y += ball->dirY * ball->speed;
    ball->x += ball->dirX * ball->speed;

    if (ball->y + ball->radius > SCREEN_H)
    {

        ball->dirY = -ball->dirY;
    }
    else if (ball->y - ball->radius < 0)
    {

        ball->dirY = -ball->dirY;
    }
}

int main(int argc, char *argv[])
{

    (void)argc;
    (void)argv;
    (void)size_font;

    // PSP setup
    sceDisplaySetMode(0, SCREEN_W, SCREEN_H);
    sceGuInit();
    sceGuStart(GU_DIRECT, gu_list);
    sceGuDrawBuffer(GU_PSM_8888, (void *)0, VRAM_STRIDE);
    sceGuDispBuffer(SCREEN_W, SCREEN_H, (void *)0x88000, VRAM_STRIDE);
    sceGuOffset(2048 - (SCREEN_W / 2), 2048 - (SCREEN_H / 2));
    sceGuViewport(2048, 2048, SCREEN_W, SCREEN_H);
    sceGuScissor(0, 0, SCREEN_W, SCREEN_H);
    sceGuEnable(GU_SCISSOR_TEST);
    sceGuDisable(GU_DEPTH_TEST);
    sceGuShadeModel(GU_SMOOTH);
    sceGuEnable(GU_TEXTURE_2D);
    sceGuEnable(GU_BLEND);
    sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);
    sceGuTexMode(GU_PSM_8888, 0, 0, 0);
    sceGuTexImage(0, 256, 128, 256, font);
    sceGuTexFunc(GU_TFX_MODULATE, GU_TCC_RGBA);
    sceGuTexEnvColor(0);
    sceGuTexOffset(0.0f, 0.0f);
    sceGuTexScale(1.0f / 256.0f, 1.0f / 128.0f);
    sceGuTexWrap(GU_REPEAT, GU_REPEAT);
    sceGuTexFilter(GU_NEAREST, GU_NEAREST);
    sceGuFinish();
    sceGuSync(0, 0);
    sceGuDisplay(GU_TRUE);
    sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_DIGITAL);

    // difficulty handling
    float moveSpeed[] = {2.0f, 2.5f, 3.0f};
    float ballSpeed[] = {3.0f, 3.5f, 4.0f};
    int difficulty = 0; // 0: Easy, 1: Medium, 2: Hard

    // Initialize controllable player on the right side of the screen
    Player rightPlayer;
    rightPlayer.height = 60.0f;
    rightPlayer.width = 10.0f;
    rightPlayer.y = SCREEN_H / 2.0f;
    rightPlayer.x = SCREEN_W - rightPlayer.width;
    rightPlayer.speed = 2.0f;
    rightPlayer.score = 0;

    // Initialize AI player on the left side of the screen
    Player leftPlayer;
    leftPlayer.height = 60.0f;
    leftPlayer.width = 10.0f;
    leftPlayer.y = SCREEN_H / 2.0f;
    leftPlayer.x = 0.0f;
    leftPlayer.speed = 2.0f;
    leftPlayer.score = 0;

    // Initialize the ball in the center of the screen
    Ball ball;
    ball.x = SCREEN_W / 2.0f;
    ball.y = SCREEN_H / 2.0f;
    ball.radius = 10.0f;
    // ball.speed = ballSpeed[difficulty];
    ball.speed = 3.0f;
    ball.dirX = (rand() % 2 == 0) ? -1.0f : 1.0f;
    ball.dirY = (rand() % 2 == 0) ? -1.0f : 1.0f;
    ball.color = 0xFFFFFF00;
    ball.segments = 64;

    // input initialization
    SceCtrlData pad;

    // Main game loop
    while (gameRunning)
    {

        // ----- Input reading -----
        handleControls(&leftPlayer, &rightPlayer, &pad);

        // ----- AI Player -----
        if (AI_ENABLED == 1)
        {
            aiPlayerHandler(&leftPlayer, &ball);
        }

        // ----- Physics -----
        gamePhysics(&ball);

        checkPlayerCollision(&leftPlayer, &rightPlayer, &ball);

        // ----- Render -----
        render(&leftPlayer, &rightPlayer, &ball); // Render the game state

        // sceKernelDelayThread(16000); // ~16ms per tick
    }

    sceGuTerm();
    return 0;
}
