#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspctrl.h>
#include <stdint.h>
#include <string.h>

PSP_MODULE_INFO("HelloWorldCentered", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

enum { SCREEN_W = 480, SCREEN_H = 272, CHAR_W = 8, CHAR_H = 8, VRAM_STRIDE = 512 };

static uint32_t *g_vram = (uint32_t*)0x44000000;

// Minimal 8x8 glyphs for the letters we need
static const uint8_t glyph_space[8] = {0,0,0,0,0,0,0,0};
static const uint8_t glyph_H[8]    = {0x66,0x66,0x66,0x7E,0x66,0x66,0x66,0x00};
static const uint8_t glyph_e[8]    = {0x00,0x00,0x3C,0x66,0x7E,0x60,0x3C,0x00};
static const uint8_t glyph_l[8]    = {0x18,0x18,0x18,0x18,0x18,0x18,0x0C,0x00};
static const uint8_t glyph_o[8]    = {0x00,0x00,0x3C,0x66,0x66,0x66,0x3C,0x00};
static const uint8_t glyph_W[8]    = {0x63,0x63,0x63,0x6B,0x7F,0x77,0x63,0x00};
static const uint8_t glyph_r[8]    = {0x00,0x00,0x36,0x3A,0x30,0x30,0x30,0x00};
static const uint8_t glyph_d[8]    = {0x06,0x06,0x3E,0x66,0x66,0x66,0x3E,0x00};

static const uint8_t *get_glyph(char c) {
    switch (c) {
        case 'H': return glyph_H;
        case 'e': return glyph_e;
        case 'l': return glyph_l;
        case 'o': return glyph_o;
        case 'W': return glyph_W;
        case 'r': return glyph_r;
        case 'd': return glyph_d;
        case ' ': return glyph_space;
        default:  return glyph_space;
    }
}

static void clear_screen(uint32_t color) {
    for (int y = 0; y < SCREEN_H; y++) {
        uint32_t *row = g_vram + y * VRAM_STRIDE;
        for (int x = 0; x < SCREEN_W; x++) {
            row[x] = color;
        }
    }
}

static void fill_rect(int x, int y, int w, int h, uint32_t color) {
    if (x < 0) { w += x; x = 0; }
    if (y < 0) { h += y; y = 0; }
    if (x + w > SCREEN_W) w = SCREEN_W - x;
    if (y + h > SCREEN_H) h = SCREEN_H - y;
    if (w <= 0 || h <= 0) return;
    for (int yy = 0; yy < h; yy++) {
        uint32_t *row = g_vram + (y + yy) * VRAM_STRIDE + x;
        for (int xx = 0; xx < w; xx++) {
            row[xx] = color;
        }
    }
}

static void draw_char(int x, int y, char c, uint32_t color) {
    const uint8_t *g = get_glyph(c);
    for (int row = 0; row < 8; row++) {
        uint8_t bits = g[row];
        uint32_t *dst = g_vram + (y + row) * VRAM_STRIDE + x;
        for (int col = 0; col < 8; col++) {
            if (bits & (1 << (7 - col))) {
                dst[col] = color;
            }
        }
    }
}

static void draw_text(int x, int y, const char *s, uint32_t color) {
    for (int i = 0; s[i]; i++) {
        draw_char(x + i * CHAR_W, y, s[i], color);
    }
}

int main(int argc, char *argv[]) {
    (void)argc; (void)argv;

    const char *msg = "Hello World";

    sceDisplaySetMode(0, SCREEN_W, SCREEN_H);
    sceDisplaySetFrameBuf((void*)0x44000000, VRAM_STRIDE, PSP_DISPLAY_PIXEL_FORMAT_8888, PSP_DISPLAY_SETBUF_IMMEDIATE);

    clear_screen(0xFF000000); // black
    fill_rect(40, 40, 120, 60, 0xFF00AAFF); // light blue rectangle

    const int msg_len = (int)strlen(msg);
    const int text_w = msg_len * CHAR_W;
    const int x = (SCREEN_W - text_w) / 2;
    const int y = (SCREEN_H - CHAR_H) / 2;
    draw_text(x, y, msg, 0xFFFFFFFF);

    sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_DIGITAL);

    // Keep the app running; press START to exit
    while (1) {
        SceCtrlData pad;
        sceCtrlReadBufferPositive(&pad, 1);
        if (pad.Buttons & PSP_CTRL_START) {
            break;
        }
        sceDisplayWaitVblankStart();
    }

    return 0;
}
