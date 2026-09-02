#ifndef BATTLE_H
#define BATTLE_H

#include "ships.h"

int isInsideBattlefield(double x,
                        double y,
                        double battlefieldSize);

double calculateDistance(Position a,
                         Position b);

int canBattleshipHitEscort(Battleship battleship,
                           EscortShip escort);

int canEscortHitBattleship(Battleship battleship,
                           EscortShip escort);

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
                         double battlefieldSize);

#endif
