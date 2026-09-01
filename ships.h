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
    Position position;
} Battleship;

typedef struct
{
    int id;
    Position position;
} EscortShip;

void createEscortShips(EscortShip escorts[],
                       int numberOfEscorts,
                       double battlefieldSize);

#endif
