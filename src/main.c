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

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// TO-FIX
// - Ball can get stuck inside of the rectangle, create a top and bottom collision check

// TO-DO
// - add speed multiplier per ball bounce
// - add actual difficulty settings
// - add main menu 

const int AI_ENABLED = 1;
unsigned int gameRunning = 1;


const int MAX_BOUNCE_ANGLE = 30;
const float BALL_SPEED_MULTIPLIER = 1.05;
const float DEFAULT_BALL_SPEED = 3.0f;

PSP_MODULE_INFO("Pong", 0, 1, 0);
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
    float angle;
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
    int score;
} Player;

float map(float x, float in_min, float in_max, float out_min, float out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// TO FIX - RIGHT NOW THE CENTER IS DOING THE HIGH ANGLE CHANGES INSTEAD OF THE EDGES, IT SHOULD BE THE OPPISITE
float verticalDistanceFromCenter(Player *player, Ball *ball)
{

    return ball->y - player->y;
}

// I think this works the way i want it to, edges narrow / widen the angle based on what side the ball is coming from and center will keep ball same angle
float calculateBallAngle(Player *player, Ball *ball)
{

    if (ball->angle < 0.0f)
    {
        // return (M_PI - ball->angle);
        if (verticalDistanceFromCenter(player, ball) > 0.0f)
        {

            return fmod((M_PI - ball->angle) - map(abs(verticalDistanceFromCenter(player, ball)), 0.0f, player->height / 2.0f, 0.0f, (MAX_BOUNCE_ANGLE * M_PI / 180.0f)), 2 * M_PI);
        }
        else
        {
            return fmod((M_PI - ball->angle) + map(abs(verticalDistanceFromCenter(player, ball)), 0.0f, player->height / 2.0f, 0.0f, (MAX_BOUNCE_ANGLE * M_PI / 180.0f)), 2 * M_PI);
        }
    }
    else if (ball->angle > 0.0f)
    {
        // return (M_PI - ball->angle);
        if (verticalDistanceFromCenter(player, ball) < 0.0f)
        {

            return fmod((M_PI - ball->angle) + map(abs(verticalDistanceFromCenter(player, ball)), 0.0f, player->height / 2.0f, 0.0f, (MAX_BOUNCE_ANGLE * M_PI / 180.0f)), 2 * M_PI);
        }
        else
        {
            return fmod((M_PI - ball->angle) - map(abs(verticalDistanceFromCenter(player, ball)), 0.0f, player->height / 2.0f, 0.0f, (MAX_BOUNCE_ANGLE * M_PI / 180.0f)), 2 * M_PI);
        }
    }
    else if (ball->angle == 0.0f)
    {
        return (M_PI - ball->angle);
    }
}

int checkPlayerCollision(Player *leftPlayer, Player *rightPlayer, Ball *ball)
{

    // left player collision check
    if (ball->x - ball->radius < 0.0f)
    {
        ball->angle = (-45.0f * (2 * (rand() % 4) + 1)) * (M_PI / 180.0);
        
        ball->x = SCREEN_W / 2.0f;
        ball->y = SCREEN_H / 2.0f;
        ball->speed = DEFAULT_BALL_SPEED;
        rightPlayer->score++;
    }
    else if ((ball->y + ball->radius > leftPlayer->y - leftPlayer->height / 2.0f) && (ball->y - ball->radius < leftPlayer->y + leftPlayer->height / 2.0f))
    {
        if ((ball->x - ball->radius < leftPlayer->x + leftPlayer->width))
        {
            ball->x = leftPlayer->x + leftPlayer->width + ball->radius;
            ball->angle = calculateBallAngle(leftPlayer, ball);
            ball->speed *= BALL_SPEED_MULTIPLIER;
        }
    }

    // right player collision check
    if (ball->x + ball->radius > SCREEN_W)
    {
        ball->angle = (-45.0f * (2 * (rand() % 4) + 1)) * (M_PI / 180.0);

        ball->x = SCREEN_W / 2.0f;
        ball->y = SCREEN_H / 2.0f;
        ball->speed = DEFAULT_BALL_SPEED;
        leftPlayer->score++;
    }
    else if ((ball->y + ball->radius > rightPlayer->y - rightPlayer->height / 2.0f) && (ball->y - ball->radius < rightPlayer->y + rightPlayer->height / 2.0f))
    {
        if ((ball->x + ball->radius > rightPlayer->x))
        {
            ball->x = rightPlayer->x - ball->radius;
            ball->angle = calculateBallAngle(rightPlayer, ball);
            ball->speed *= BALL_SPEED_MULTIPLIER;
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

    char ballAngle[6];
    snprintf(ballAngle, sizeof(ballAngle), "%.5f", ball->angle);
    draw_text(ballAngle, (SCREEN_W - measure_text_width(ballAngle)) / 2.0f, (SCREEN_H - 32) / 2, 0xFFFFFFFF);

    char ballSpeed[6];
    snprintf(ballSpeed, sizeof(ballSpeed), "%.5f", ball->speed);
    draw_text(ballSpeed, (SCREEN_W - measure_text_width(ballSpeed)) / 2.0f, (SCREEN_H) / 2, 0xFFFFFFFF);
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
    return 0;
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

int gamePhysics(Ball *ball)
{

    ball->y += sin(ball->angle) * ball->speed;
    ball->x += cos(ball->angle) * ball->speed;
    if (ball->y + ball->radius > SCREEN_H)
    {
        ball->y = SCREEN_H - ball->radius;
        ball->angle = fmod(-ball->angle, 2 * M_PI);
    }
    else if (ball->y - ball->radius < 0)
    {
        ball->y = ball->radius;
        ball->angle = fmod(-ball->angle, 2 * M_PI);
    }
    return 0;
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
    leftPlayer.speed = 1.0f;
    leftPlayer.score = 0;

    // Initialize the ball in the center of the screen
    Ball ball;
    ball.x = SCREEN_W / 2.0f;
    ball.y = SCREEN_H / 2.0f;
    ball.radius = 10.0f;
    // ball.speed = ballSpeed[difficulty];
    ball.speed = 3.0f;
    // ball.dirX = (rand() % 2 == 0) ? -1.0f : 1.0f;
    // ball.dirY = (rand() % 2 == 0) ? -1.0f : 1.0f;
    ball.angle = (45.0f * (2 * (rand() % 4) + 1)) * (M_PI / 180.0);

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
