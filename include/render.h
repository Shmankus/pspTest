#ifndef RENDER_H
#define RENDER_H

#include <pspctrl.h>
#include "structs.h"

int renderGame(Player *leftPlayer, Player *rightPlayer, Ball *ball);
void renderMenu(SceCtrlData *pad, unsigned int *selectedOption, char menuItems[3][20]);

#endif
