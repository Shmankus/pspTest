#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspctrl.h>
#include <pspgu.h>
#include <pspgum.h>
#include <stdint.h>

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

    int horDir = 0;
    int vertDir = 0;
    // Keep the app running; press START to exit
    while (1)
    {

        SceCtrlData pad;

        unsigned int b = (sceCtrlReadBufferPositive(&pad, 1) > 0) ? pad.Buttons : 0;

        // if ((b & PSP_CTRL_RIGHT) && (b & PSP_CTRL_UP))
        // {
        //     horDir++;
        //     vertDir--;
        // }
        // else if ((b & PSP_CTRL_RIGHT) && (b & PSP_CTRL_DOWN))
        // {
        //     horDir++;
        //     vertDir++;
        // }
        // else if ((b & PSP_CTRL_LEFT) && (b & PSP_CTRL_UP))
        // {
        //     horDir--;
        //     vertDir--;
        // }
        // else if ((b & PSP_CTRL_LEFT) && (b & PSP_CTRL_DOWN))
        // {
        //     horDir--;
        //     vertDir++;
        // }
        // else if (b & PSP_CTRL_RIGHT)
        // {
        //     horDir++;
        // }
        // else if (b & PSP_CTRL_LEFT)
        // {
        //     horDir--;
        // }
        if (b & PSP_CTRL_UP)
        {
            vertDir--;
        }
        else if (b & PSP_CTRL_DOWN)
        {
            vertDir++;
        }

        // Draw every frame and swap buffers
        sceGuStart(GU_DIRECT, gu_list);
        sceGuClearColor(0xFF000000);
        sceGuClear(GU_COLOR_BUFFER_BIT);


        float playerWidth = 20.0f;
        float playerHeight = 60.0f;
        float playerSpeed = 2.0f;

        // Right side player that has movement functionality
        Vertex *controllablePlayer = (Vertex *)sceGuGetMemory(2 * sizeof(Vertex));
        controllablePlayer[0].color = 0xFFFF0000;
        controllablePlayer[0].x = SCREEN_W - (playerWidth / 2.0f);
        controllablePlayer[0].y = (SCREEN_H / 2.0f - playerHeight / 2.0f) + vertDir * playerSpeed;
        controllablePlayer[0].z = 0.0f;
        controllablePlayer[1].color = 0xFFFF0000;
        controllablePlayer[1].x = SCREEN_W;
        controllablePlayer[1].y = (SCREEN_H / 2.0f + playerHeight / 2.0f) + vertDir * playerSpeed;
        controllablePlayer[1].z = 0.0f;
        sceGuDrawArray(GU_SPRITES, GU_COLOR_8888 | GU_VERTEX_32BITF | GU_TRANSFORM_2D, 2, NULL, controllablePlayer);

        // Left side player that will be controlled by an AI
        Vertex *aiPlayer = (Vertex *)sceGuGetMemory(2 * sizeof(Vertex));
        aiPlayer[0].color = 0xFFFF0000;
        aiPlayer[0].x = 0.0f;
        aiPlayer[0].y = (SCREEN_H / 2.0f - playerHeight / 2.0f);
        aiPlayer[0].z = 0.0f;
        aiPlayer[1].color = 0xFFFF0000;
        aiPlayer[1].x = playerWidth / 2.0f;
        aiPlayer[1].y = (SCREEN_H / 2.0f + playerHeight / 2.0f);
        aiPlayer[1].z = 0.0f;
        sceGuDrawArray(GU_SPRITES, GU_COLOR_8888 | GU_VERTEX_32BITF | GU_TRANSFORM_2D, 2, NULL, aiPlayer);

        sceGuFinish();
        sceGuSync(0, 0);
        sceDisplayWaitVblankStart();
        sceGuSwapBuffers();

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
