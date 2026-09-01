#ifndef BATTLE_H
#define BATTLE_H

int isInsideBattlefield(double x,
                        double y,
                        double battlefieldSize);

void displayBattlefield(double battlefieldSize,
                        double battleshipX,
                        double battleshipY,
                        double escortX[],
                        double escortY[],
                        int numberOfEscorts);

#endif
