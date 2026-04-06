#ifndef VISUALS_H
#define VISUALS_H

int measure_text_width(const char *text);
void draw_text(const char *text, int x, int y, unsigned int color);
void circle(float cx, float cy, float r, unsigned int color, int segments);

#endif
