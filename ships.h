#ifndef SHIPS_H
#define SHIPS_H

#define MAX_ESCORTS 100
#define MAX_POINTS 100

typedef struct
{
    double x;
    double y;
} Position;

typedef struct
{
    int index;

    char notation;
    char name[50];

    Position position;

    double vmax;
    double vmin;

    double angleMin;
    double angleRange;

    double impactPower;

    int destroyed;

    int shotsFired;

    double currentImpact;

} EscortShip;

typedef struct
{
    char notation;
    char name[50];
    char gunName[60];

    Position position;

    double vmax;

    double angleMin;
    double angleMax;

    int destroyed;

    int shotsFired;

    double gamma;

    double currentImpact;

} Battleship;


void setupBattleship(Battleship *battleship);

void createEscortShips(
    EscortShip escorts[],
    int numberOfEscorts,
    double battlefieldSize,
    double battleshipVmax);

#endif
