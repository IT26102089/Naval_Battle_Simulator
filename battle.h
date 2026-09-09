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
    double battlefieldSize,
    unsigned int seed);


void savePart1AFinal(
    Battleship battleship,
    EscortShip escorts[],
    int numberOfEscorts,
    int escortsHit,
    int sinkingEscort);



/* Start a new folder for this simulation session. */
void startNewResultRun(void);

/* Get the full path of a result file in the current run folder. */
const char *resultPath(const char *fileName);

/* Save setup values for the current simulation session. */
void saveSessionSettingsBase(
    unsigned int seed,
    double battlefieldSize,
    Battleship battleship,
    EscortShip escorts[],
    int numberOfEscorts);

void appendSessionSetting(const char *text);

#endif
