#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspctrl.h>
#include <pspgu.h>
#include <pspgum.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>

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
    unsigned int color;
    float x;
    float y;
    float z;
} Vertex;

typedef struct
{
    float x;
    float y;
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
    float height;
    float width;
    float speed;
} Player;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static void circle(float cx, float cy, float r, unsigned int color, int segments)
{
    Vertex *v = (Vertex *)sceGuGetMemory((segments + 2) * sizeof(Vertex));

    v[0].color = color;
    v[0].x = cx;
    v[0].y = cy;
    v[0].z = 0.0f;

    for (int i = 0; i <= segments; i++)
    {
        float t = (2.0f * (float)M_PI * i) / segments;
        v[i + 1].color = color;
        v[i + 1].x = cx + cosf(t) * r;
        v[i + 1].y = cy + sinf(t) * r;
        v[i + 1].z = 0.0f;
    }

    sceGuDrawArray(GU_TRIANGLE_FAN,
                   GU_COLOR_8888 | GU_VERTEX_32BITF | GU_TRANSFORM_2D,
                   segments + 2, NULL, v);
}

int checkPlayerCollision(Player leftPlayer, Player rightPlayer, Ball ball)
{


    return 0;
}

int render(Player player, Ball ball)
{
    // Draw every frame and swap buffers
    sceGuStart(GU_DIRECT, gu_list);
    sceGuClearColor(0xFF000000);
    sceGuClear(GU_COLOR_BUFFER_BIT);

    // Right side player that has movement functionality

    Vertex *controllablePlayer = (Vertex *)sceGuGetMemory(2 * sizeof(Vertex));
    controllablePlayer[0].color = 0xFFFF0000;
    controllablePlayer[0].x = SCREEN_W - (player.width / 2.0f);
    controllablePlayer[0].y = player.y - (player.height / 2.0f);
    controllablePlayer[0].z = 0.0f;
    controllablePlayer[1].color = 0xFFFF0000;
    controllablePlayer[1].x = SCREEN_W;
    controllablePlayer[1].y = player.y + player.height / 2.0f;
    controllablePlayer[1].z = 0.0f;
    sceGuDrawArray(GU_SPRITES, GU_COLOR_8888 | GU_VERTEX_32BITF | GU_TRANSFORM_2D, 2, NULL, controllablePlayer);

    // Left side player that will be controlled by an AI
    Vertex *aiPlayer = (Vertex *)sceGuGetMemory(2 * sizeof(Vertex));
    aiPlayer[0].color = 0xFFFF0000;
    aiPlayer[0].x = 0.0f;
    aiPlayer[0].y = (SCREEN_H / 2.0f - player.height / 2.0f);
    aiPlayer[0].z = 0.0f;
    aiPlayer[1].color = 0xFFFF0000;
    aiPlayer[1].x = player.width / 2.0f;
    aiPlayer[1].y = (SCREEN_H / 2.0f + player.height / 2.0f);
    aiPlayer[1].z = 0.0f;
    sceGuDrawArray(GU_SPRITES, GU_COLOR_8888 | GU_VERTEX_32BITF | GU_TRANSFORM_2D, 2, NULL, aiPlayer);

    // Draw the ball in the center of the screen
    circle(ball.x, ball.y, ball.radius, ball.color, ball.segments);

    sceGuFinish();
    sceGuSync(0, 0);
    sceDisplayWaitVblankStart();
    sceGuSwapBuffers();

    return 0;
}

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

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
    sceGuDisable(GU_TEXTURE_2D);
    sceGuFinish();
    sceGuSync(0, 0);
    sceGuDisplay(GU_TRUE);

    sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_DIGITAL);

    float moveSpeed[] = {2.0f, 2.5f, 3.0f};
    float ballSpeed[] = {3.0f, 3.5f, 4.0f};

    int difficulty = 0; // 0: Easy, 1: Medium, 2: Hard

    SceCtrlData pad;

    // Initialize the player in the center of the screen
    Player player;
    player.y = SCREEN_H / 2.0f;
    player.height = 60.0f;
    player.width = 10.0f;
    player.speed = moveSpeed[difficulty];

    // Initialize the ball in the center of the screen
    Ball ball;
    ball.x = SCREEN_W / 2.0f;
    ball.y = SCREEN_H / 2.0f;
    ball.radius = 10.0f;
    ball.speed = ballSpeed[difficulty];
    ball.dirX = ball.speed * (rand() % 2 == 0) ? -1 : 1;
    ball.dirY = ball.speed * (rand() % 2 == 0) ? -1 : 1;
    ball.color = 0xFFFFFF00;
    ball.segments = 64;

    // Main game loop
    while (1)
    {

        // Read the current state of the controls
        unsigned int b = (sceCtrlReadBufferPositive(&pad, 1) > 0) ? pad.Buttons : 0;
        if (b & PSP_CTRL_UP)
        {
            player.y -= player.speed;
            if (player.y - player.height / 2.0f < 0.0f)
            {
                player.y = player.height / 2.0f;
            }
        }
        else if (b & PSP_CTRL_DOWN)
        {
            player.y += player.speed;
            if (player.y + player.height / 2.0f > SCREEN_H)
            {
                player.y = SCREEN_H - player.height / 2.0f;
            }
        }

        ball.y += ball.dirY;
        ball.x += ball.dirX;

        if (ball.y + ball.radius > SCREEN_H)
        {

            ball.dirY = -ball.dirY;
        }
        else if (ball.y - ball.radius < 0)
        {

            ball.dirY = -ball.dirY;
        }

        render(player, ball); // Render the game state

        sceCtrlReadBufferPositive(&pad, 1);
        if (pad.Buttons & PSP_CTRL_START)
        {
            break;
        }
        sceKernelDelayThread(16000); // ~16ms
    }

    sceGuTerm();
    return 0;
}
