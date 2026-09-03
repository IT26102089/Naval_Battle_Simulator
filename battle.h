#ifndef BATTLE_H
#define BATTLE_H

#include "ships.h"

double calculateDistance(Position a,
                         Position b);

double calculateProjectileRange(double velocity,
                                double angleDegrees);

double calculateFlightTime(double distance,
                           double velocity,
                           double angleDegrees);

int canBattleshipHitEscort(Battleship battleship,
                           EscortShip escort,
                           double *hitTime);

int canEscortHitBattleship(Battleship battleship,
                           EscortShip escort,
                           double *hitTime);

void displayBattlefield(double battlefieldSize,
                        Battleship battleship,
                        EscortShip escorts[],
                        int numberOfEscorts);

void saveInitialConditions(Battleship battleship,
                           EscortShip escorts[],
                           int numberOfEscorts,
                           double battlefieldSize);

void saveFinalConditions(Battleship battleship,
                         EscortShip escorts[],
                         int numberOfEscorts,
                         double battlefieldSize,
                         int escortsHit,
                         int sinkingEscort);

#endif
