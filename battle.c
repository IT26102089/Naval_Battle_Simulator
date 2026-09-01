#include <stdio.h>
#include "battle.h"

int isInsideBattlefield(double x,
                        double y,
                        double battlefieldSize)
{
    if (x < 0 || x > battlefieldSize ||
        y < 0 || y > battlefieldSize)
    {
        return 0;
    }

    return 1;
}

void displayBattlefield(double battlefieldSize,
                        double battleshipX,
                        double battleshipY,
                        double escortX[],
                        double escortY[],
                        int numberOfEscorts)
{
    int rows = 10;
    int columns = 10;
    int row;
    int column;
    int i;

    printf("\n\n");
    printf("====================================================\n");
    printf("              NAVAL BATTLEFIELD\n");
    printf("====================================================\n\n");

    for (row = rows - 1; row >= 0; row--)
    {
        printf("%4d |", (int)(battlefieldSize * row / rows));

        for (column = 0; column < columns; column++)
        {
            double cellMinX =
                battlefieldSize * column / columns;

            double cellMaxX =
                battlefieldSize * (column + 1) / columns;

            double cellMinY =
                battlefieldSize * row / rows;

            double cellMaxY =
                battlefieldSize * (row + 1) / rows;

            int found = 0;

            if (battleshipX >= cellMinX &&
                battleshipX < cellMaxX &&
                battleshipY >= cellMinY &&
                battleshipY < cellMaxY)
            {
                printf(" B ");
                found = 1;
            }

            if (!found)
            {
                for (i = 0; i < numberOfEscorts; i++)
                {
                    if (escortX[i] >= cellMinX &&
                        escortX[i] < cellMaxX &&
                        escortY[i] >= cellMinY &&
                        escortY[i] < cellMaxY)
                    {
                        printf(" E ");
                        found = 1;
                        break;
                    }
                }
            }

            if (!found)
            {
                printf(" . ");
            }
        }

        printf("\n");
    }

    printf("     +--------------------------------\n");
    printf("       ");

    for (column = 0; column <= columns; column++)
    {
        printf("%-4d",
               (int)(battlefieldSize * column / columns));
    }

    printf("\n\n");

    printf("B = Battleship\n");
    printf("E = Escort Ship\n");
    printf(". = Empty area\n");
}
