#ifndef GAMELOGIC_H
#define GAMELOGIC_H


#include "structs.h"
float map(float x, float in_min, float in_max, float out_min, float out_max);
float verticalDistanceFromCenter(Player *player, Ball *ball);
float calculateBallAngle(Player *player, Ball *ball);
int checkPlayerCollision(Player *leftPlayer, Player *rightPlayer, Ball *ball);
int aiPlayerHandler(Player *leftPlayer, Ball *ball);
int gamePhysics(Ball *ball);

#endif