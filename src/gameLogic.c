#ifndef GAMELOGIC_C
#define GAMELOGIC_C

#include <math.h>
#include <stdlib.h>
#include "structs.h"
#include "mathHelpers.h"

#define MAX_BOUNCE_ANGLE 15
#define BALL_SPEED_MULTIPLIER 1.05f
#define DEFAULT_BALL_SPEED 3.0f

// TO FIX - RIGHT NOW THE CENTER IS DOING THE HIGH ANGLE CHANGES INSTEAD OF THE EDGES, IT SHOULD BE THE OPPISITE
float verticalDistanceFromCenter(Player *player, Ball *ball)
{

    return ball->y - player->y;
}

// I think this works the way i want it to, edges narrow / widen the angle based on what side the ball is coming from and center will keep ball same angle

float calculateBallAngle(Player *player, Ball *ball)
{

    if (sin(ball->angle) >= 0.0f)
    {
        // return (M_PI - ball->angle);
        if (verticalDistanceFromCenter(player, ball) < 0.0f)
        {

            return fmod((M_PI - ball->angle) + map(abs(verticalDistanceFromCenter(player, ball)), 0.0f, player->height / 2.0f, 0.0f, (MAX_BOUNCE_ANGLE * M_PI / 180.0f)), 2 * M_PI);
        }
        else
        {
            return fmod((M_PI - ball->angle) - map(abs(verticalDistanceFromCenter(player, ball)), 0.0f, player->height / 2.0f, 0.0f, (MAX_BOUNCE_ANGLE * M_PI / 180.0f)), 2 * M_PI);
        }
    }
    else if (sin(ball->angle) < 0.0f)
    {
        // return (M_PI - ball->angle);
        if (verticalDistanceFromCenter(player, ball) > 0.0f)
        {

            return fmod((M_PI - ball->angle) - map(abs(verticalDistanceFromCenter(player, ball)), 0.0f, player->height / 2.0f, 0.0f, (MAX_BOUNCE_ANGLE * M_PI / 180.0f)), 2 * M_PI);
        }
        else
        {
            return fmod((M_PI - ball->angle) + map(abs(verticalDistanceFromCenter(player, ball)), 0.0f, player->height / 2.0f, 0.0f, (MAX_BOUNCE_ANGLE * M_PI / 180.0f)), 2 * M_PI);
        }
    }
    else if (ball->angle == 0.0f)
    {
        return (M_PI - ball->angle);
    }
}

int checkPlayerCollision(Player *leftPlayer, Player *rightPlayer, Ball *ball)
{

    // left player collision check
    if (ball->x - ball->radius < 0.0f)
    {
        ball->angle = (-45.0f * (2 * (rand() % 4) + 1)) * (M_PI / 180.0);

        ball->x = SCREEN_W / 2.0f;
        ball->y = SCREEN_H / 2.0f;
        ball->speed = DEFAULT_BALL_SPEED;
        rightPlayer->score++;
    }
    else if ((ball->y + ball->radius > leftPlayer->y - leftPlayer->height / 2.0f) && (ball->y - ball->radius < leftPlayer->y + leftPlayer->height / 2.0f))
    {
        if ((ball->x - ball->radius < leftPlayer->x + leftPlayer->width))
        {
            ball->x = leftPlayer->x + leftPlayer->width + ball->radius;
            ball->angle = calculateBallAngle(leftPlayer, ball);
            ball->speed *= BALL_SPEED_MULTIPLIER;
        }
    }

    // right player collision check
    if (ball->x + ball->radius > SCREEN_W)
    {
        ball->angle = (-45.0f * (2 * (rand() % 4) + 1)) * (M_PI / 180.0);

        ball->x = SCREEN_W / 2.0f;
        ball->y = SCREEN_H / 2.0f;
        ball->speed = DEFAULT_BALL_SPEED;
        leftPlayer->score++;
    }
    else if ((ball->y + ball->radius > rightPlayer->y - rightPlayer->height / 2.0f) && (ball->y - ball->radius < rightPlayer->y + rightPlayer->height / 2.0f))
    {
        if ((ball->x + ball->radius > rightPlayer->x))
        {
            ball->x = rightPlayer->x - ball->radius;
            ball->angle = calculateBallAngle(rightPlayer, ball);
            ball->speed *= BALL_SPEED_MULTIPLIER;
        }
    }

    return 0;
}

int aiPlayerHandler(Player *leftPlayer, Ball *ball)
{

    if (leftPlayer->y < ball->y && leftPlayer->y + leftPlayer->height / 2.0f < SCREEN_H)
    {
        leftPlayer->y += leftPlayer->speed;
    }
    else if (leftPlayer->y > ball->y  && leftPlayer->y - leftPlayer->height / 2.0f > 0)
    {
        leftPlayer->y -= leftPlayer->speed;
    }
    return 0;
}
int gamePhysics(Ball *ball)
{
    ball->y += sin(ball->angle) * ball->speed;
    ball->x += cos(ball->angle) * ball->speed;
    if (ball->y + ball->radius > SCREEN_H)
    {
        ball->y = SCREEN_H - ball->radius;
        ball->angle = fmod(-ball->angle, 2 * M_PI);
    }
    else if (ball->y - ball->radius < 0)
    {
        ball->y = ball->radius;
        ball->angle = fmod(-ball->angle, 2 * M_PI);
    }
    return 0;
}
#endif