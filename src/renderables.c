#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspctrl.h>
#include <pspgu.h>
#include <pspgum.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>

typedef struct
{
    float s;
    float t;
    unsigned int color;
    float x;
    float y;
    float z;
} TextVertex;

typedef struct
{
    unsigned int color;
    float x;
    float y;
    float z;
} Vertex;

static const int font_width_table[128] = {
    10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
    10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
    10, 6, 8, 10, 10, 10, 10, 6, 10, 10, 10, 10, 6, 10, 6, 10,
    10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 6, 6, 10, 10, 10, 10,
    16, 10, 10, 10, 10, 10, 10, 10, 10, 6, 8, 10, 8, 10, 10, 10,
    10, 10, 10, 10, 10, 10, 10, 12, 10, 10, 10, 10, 10, 8, 10, 6,
    6, 8, 8, 8, 8, 8, 6, 8, 8, 6, 6, 8, 6, 10, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 12, 8, 8, 8, 10, 8, 10, 8, 12};

static int measure_text_width(const char *text)
{
    int width = 0;
    size_t len = strlen(text);

    for (size_t i = 0; i < len; ++i)
    {
        unsigned char c = (unsigned char)text[i];
        if (c < 32 || c >= 128)
        {
            c = 0;
        }
        width += font_width_table[c];
    }

    return width;
}

static void draw_text(const char *text, int x, int y, unsigned int color)
{
    int len = (int)strlen(text);
    if (len <= 0)
    {
        return;
    }

    TextVertex *vertices = (TextVertex *)sceGuGetMemory(sizeof(TextVertex) * 2 * len);

    for (int i = 0; i < len; ++i)
    {
        unsigned char c = (unsigned char)text[i];
        if (c < 32 || c >= 128)
        {
            c = 0;
        }

        int tx = (c & 0x0F) << 4;
        int ty = (c & 0xF0);
        int glyph_width = font_width_table[c];

        TextVertex *v0 = &vertices[i * 2];
        TextVertex *v1 = &vertices[i * 2 + 1];

        v0->s = (float)(tx + ((16 - glyph_width) >> 1));
        v0->t = (float)ty;
        v0->color = color;
        v0->x = (float)x;
        v0->y = (float)y;
        v0->z = 0.0f;

        v1->s = (float)(tx + 16 - ((16 - glyph_width) >> 1));
        v1->t = (float)(ty + 16);
        v1->color = color;
        v1->x = (float)(x + glyph_width);
        v1->y = (float)(y + 16);
        v1->z = 0.0f;

        x += glyph_width;
    }

    sceGumDrawArray(GU_SPRITES,
                    GU_TEXTURE_32BITF | GU_COLOR_8888 | GU_VERTEX_32BITF | GU_TRANSFORM_2D,
                    len * 2, NULL, vertices);
}

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