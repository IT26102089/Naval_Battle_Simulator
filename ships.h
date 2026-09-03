#ifndef SHIPS_H
#define SHIPS_H

#define MAX_ESCORTS 100

typedef struct
{
    double x;
    double y;
} Position;

typedef struct
{
    char notation;
    char name[50];

    Position position;

    double vmax;
    double vmin;

    double angleMin;
    double angleRange;

    double impactPower;

    int destroyed;

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

} Battleship;


void setupBattleship(Battleship *battleship);

void createEscortShips(EscortShip escorts[],
                       int numberOfEscorts,
                       double battlefieldSize,
                       double battleshipVmax);

#endif
