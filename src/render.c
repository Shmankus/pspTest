#include <pspdisplay.h>
#include <pspctrl.h>
#include <pspgu.h>
#include <stdio.h>
#include <math.h>
#include "structs.h"
#include "visuals.h"

#ifndef RENDER_C
#define RENDER_C

static unsigned int __attribute__((aligned(16))) gu_list[262144];

int renderGame(Player *leftPlayer, Player *rightPlayer, Ball *ball)
{
    // Draw every frame and swap buffers
    sceGuStart(GU_DIRECT, gu_list);

    if (1)
    {

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

        char ballAngle[12];

        if (sin(ball->angle) <= 0.0f)
        {
            snprintf(ballAngle, sizeof(ballAngle), "up %.2f", ball->angle);
        }
        else
        {
            snprintf(ballAngle, sizeof(ballAngle), "down %.2f", ball->angle);
        }
        // snprintf(ballAngle, sizeof(ballAngle), "%.5f", ball->angle);
        draw_text(ballAngle, (SCREEN_W - measure_text_width(ballAngle)) / 2.0f, (SCREEN_H - 32) / 2, 0xFFFFFFFF);

        char ballSpeed[6];
        snprintf(ballSpeed, sizeof(ballSpeed), "%.5f", ball->speed);
        draw_text(ballSpeed, (SCREEN_W - measure_text_width(ballSpeed)) / 2.0f, (SCREEN_H) / 2, 0xFFFFFFFF);
        sceGuDisable(GU_TEXTURE_2D); // ends for text
    }

    sceGuFinish();
    sceGuSync(0, 0);
    sceDisplayWaitVblankStart();
    sceGuSwapBuffers();

    return 0;
}

void renderMenu(SceCtrlData *pad, unsigned int *selectedOption, char menuItems[3][20])
{
    sceGuStart(GU_DIRECT, gu_list);
    sceGuClearColor(0xFF000000);
    sceGuClear(GU_COLOR_BUFFER_BIT);
    sceGuEnable(GU_TEXTURE_2D);

    char pauseText[] = "PAUSED";
    draw_text(pauseText, (SCREEN_W - measure_text_width(pauseText)) / 2.0f, (SCREEN_H - 16) / 3, 0xFFFFFFFF);

    for (int i = 0; i < 3; i++)
    {
        draw_text(menuItems[i],
                  (SCREEN_W - measure_text_width(menuItems[i])) / 2.0f,
                  (SCREEN_H - 16) / 3 + (32 * (i + 1)),
                  (*selectedOption == i + 1) ? 0xFFFFFF00 : 0xFFFFFFFF);
    }

    sceGuDisable(GU_TEXTURE_2D);
    sceGuFinish();
    sceGuSync(0, 0);
    sceDisplayWaitVblankStart();
    sceGuSwapBuffers();
}
#endif