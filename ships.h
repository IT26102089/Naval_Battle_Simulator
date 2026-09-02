#ifndef SHIPS_H
#define SHIPS_H

#define MAX_ESCORTS 100
#define ESCORT_TYPES 5
#define BATTLESHIP_TYPES 4

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

    double angleMax;
    double angleMin;

    double impactPower;

} EscortShip;

typedef struct
{
    char notation;
    char name[50];
    char gunName[60];

    Position position;

    double vmax;

    double angleMax;
    double angleMin;

} Battleship;


/* Create escort ships with random properties */
void createEscortShips(EscortShip escorts[],
                       int numberOfEscorts,
                       double battlefieldSize,
                       double battleshipVmax);


/* Select battleship type */
void setupBattleship(Battleship *battleship);

#endif
