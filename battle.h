#ifndef BATTLE_H
#define BATTLE_H

#include "ships.h"


double calculateDistance(
    Position a,
    Position b);


double calculateProjectileRange(
    double velocity,
    double angleDegrees);


double calculateFlightTime(
    double distance,
    double velocity,
    double angleDegrees);


int canBattleshipHitEscort(
    Battleship battleship,
    EscortShip escort,
    double minimumAngle,
    double maximumAngle,
    double *hitTime);


int canEscortHitBattleship(
    Battleship battleship,
    EscortShip escort,
    double *hitTime);


void displayBattlefield(
    double battlefieldSize,
    Battleship battleship,
    EscortShip escorts[],
    int numberOfEscorts);


void savePart1AInitial(
    Battleship battleship,
    EscortShip escorts[],
    int numberOfEscorts,
    double battlefieldSize);


void savePart1AFinal(
    Battleship battleship,
    EscortShip escorts[],
    int numberOfEscorts,
    int escortsHit,
    int sinkingEscort);


#endif
