#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspgu.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "structs.h"
#include "font.c"
#include "render.h"
#include "gameLogic.h"
#include "mathHelpers.h"
#include "inputHelper.h"

// TO-FIX
// - Ball can get stuck inside of the rectangle, create a top and bottom collision check

// TO-DO
// - add speed multiplier per ball bounce
// - add actual difficulty settings
// - add main menu

#define AI_ENABLED 1

PSP_MODULE_INFO("Pong", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

static unsigned int __attribute__((aligned(16))) gu_list[262144];

void pspSetup()
{
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
}

int main(int argc, char *argv[])
{

    (void)argc;
    (void)argv;
    (void)size_font;

    pspSetup();

    GameState gameState;
    gameState.running = 1;
    gameState.showMenu = 1;
    gameState.selectedOption = 1;
    gameState.difficulties[0] = 1;
    gameState.difficulties[1] = 2;
    gameState.difficulties[2] = 3;
    gameState.difficulty = gameState.difficulties[0];
    strcpy(gameState.menuItems[0], "Unpause");
    snprintf(gameState.menuItems[1], sizeof(gameState.menuItems[1]), "Difficulty: %i", gameState.difficulty);
    strcpy(gameState.menuItems[2], "Exit");

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
    ball.speed = 3.0f;
    ball.angle = (45.0f * (2 * (rand() % 4) + 1)) * (M_PI / 180.0);
    ball.color = 0xFFFFFF00;
    ball.segments = 64;

    // input initialization
    SceCtrlData pad;
    SceCtrlLatch latchData;

    // Main game loop
    while (gameState.running)
    {
        sceCtrlReadBufferPositive(&pad, 1);
        sceCtrlReadLatch(&latchData);
        // Exit game using home
        if (pad.Buttons & PSP_CTRL_HOME)
        {
            gameState.running = 0;
        }
        else if (latchData.uiMake & PSP_CTRL_START)
        {
            if (gameState.showMenu == 1)
            {
                gameState.showMenu = 0;
            }
            else
            {
                gameState.showMenu = 1;
            }
        }
        if (gameState.showMenu == 0)
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
            renderGame(&leftPlayer, &rightPlayer, &ball); // Render the game state
        }
        else
        {
            handleMenuControls(&latchData, &gameState, &ball);

            renderMenu(&pad, &gameState.selectedOption, gameState.menuItems);
        }
    }
    sceGuTerm();
    return 0;
}
