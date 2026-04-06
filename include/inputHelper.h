#ifndef INPUTHELPER_H
#define INPUTHELPER_H

#include <pspctrl.h>
#include "structs.h"

int handleMenuControls(SceCtrlLatch *latchData, GameState *gameState);
int handleControls(Player *leftPlayer, Player *rightPlayer, SceCtrlData *pad);

#endif
