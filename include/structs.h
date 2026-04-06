#ifndef STRUCTS_H
#define STRUCTS_H

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
typedef struct
{
    unsigned int running;
    unsigned int showMenu;
    unsigned int selectedOption;
    unsigned int difficulties[3];
    unsigned int difficulty;
    char menuItems[3][20];

} GameState;

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

enum
{
    SCREEN_W = 480,
    SCREEN_H = 272,
    VRAM_STRIDE = 512
};
#endif