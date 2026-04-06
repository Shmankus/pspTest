
#ifndef INPUTHELPER_C
#define INPUTHELPER_C

#include <stdio.h>
#include <pspctrl.h>
#include "structs.h"
#define DEFAULT_BALL_SPEED 3.0f
#define AI_ENABLED 1
int handleMenuControls(SceCtrlLatch *latchData, GameState *gameState, Ball *ball)
{

    // Selection area
    if (latchData->uiMake & PSP_CTRL_DOWN && gameState->selectedOption + 1 <= 3)
    {
        gameState->selectedOption += 1;
    }
    else if (latchData->uiMake & PSP_CTRL_UP && gameState->selectedOption - 1 > 0)
    {
        gameState->selectedOption -= 1;
    }

    // Submition area
    if (gameState->selectedOption == 1 && latchData->uiMake & PSP_CTRL_CROSS)
    {
        gameState->showMenu = 0;
    }
    else if (gameState->selectedOption == 2 && latchData->uiMake & PSP_CTRL_CROSS)
    {

        if (gameState->difficulty > gameState->difficulties[-1])
        {
            gameState->difficulty = 1;
            ball->speed = DEFAULT_BALL_SPEED;
        }
        else
        {
            gameState->difficulty++;
            ball->speed = DEFAULT_BALL_SPEED + gameState->difficulty-1;
            
        }

        snprintf(gameState->menuItems[1], sizeof(gameState->menuItems[1]), "Difficulty: %i", gameState->difficulty);
    }
    else if (gameState->selectedOption == 3 && latchData->uiMake & PSP_CTRL_CROSS)
    {
        gameState->running = 0;
    }
    return 0;
}

int handleControls(Player *leftPlayer, Player *rightPlayer, SceCtrlData *pad)
{
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
#endif