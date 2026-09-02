#include <stdio.h>
#include <math.h>

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


double calculateDistance(Position a,
                         Position b)
{
    double dx;
    double dy;

    dx = a.x - b.x;
    dy = a.y - b.y;

    return sqrt(dx * dx + dy * dy);
}


/*
 * TEMPORARY attack calculation.
 *
 * We will replace this with the actual
 * projectile-motion equations from the
 * assignment.
 */
int canBattleshipHitEscort(Battleship battleship,
                           EscortShip escort)
{
    double distance;

    distance = calculateDistance(battleship.position,
                                 escort.position);

    /*
     * Temporary range.
     */
    if (distance <= 0.40 * 1000.0)
    {
        return 1;
    }

    return 0;
}


/*
 * TEMPORARY escort attack calculation.
 *
 * This will be replaced by the actual
 * annular attack-range calculation.
 */
int canEscortHitBattleship(Battleship battleship,
                            EscortShip escort)
{
    double distance;

    distance = calculateDistance(battleship.position,
                                 escort.position);

    /*
     * Temporary range.
     */
    if (distance <= escort.vmax)
    {
        return 1;
    }

    return 0;
}


void displayBattlefield(double battlefieldSize,
                        Battleship battleship,
                        EscortShip escorts[],
                        int numberOfEscorts)
{
    int rows = 10;
    int columns = 10;

    int row;
    int column;
    int i;

    int found;

    printf("\n");
    printf("====================================================\n");
    printf("                NAVAL BATTLEFIELD\n");
    printf("====================================================\n");


    for (row = rows - 1; row >= 0; row--)
    {
        printf("%4d |",
               (int)(battlefieldSize * row / rows));

        for (column = 0;
             column < columns;
             column++)
        {
            double minX;
            double maxX;

            double minY;
            double maxY;

            minX = battlefieldSize *
                   column / columns;

            maxX = battlefieldSize *
                   (column + 1) / columns;

            minY = battlefieldSize *
                   row / rows;

            maxY = battlefieldSize *
                   (row + 1) / rows;

            found = 0;


            /*
             * Battleship.
             */
            if (battleship.position.x >= minX &&
                battleship.position.x < maxX &&
                battleship.position.y >= minY &&
                battleship.position.y < maxY)
            {
                printf(" B ");
                found = 1;
            }


            /*
             * Escort ships.
             */
            if (!found)
            {
                for (i = 0;
                     i < numberOfEscorts;
                     i++)
                {
                    if (escorts[i].position.x >= minX &&
                        escorts[i].position.x < maxX &&
                        escorts[i].position.y >= minY &&
                        escorts[i].position.y < maxY)
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

    printf("\nB = Battleship\n");
    printf("E = Escort Ship\n");
    printf(". = Empty area\n");
}


void saveInitialConditions(Battleship battleship,
                           EscortShip escorts[],
                           int numberOfEscorts,
                           double battlefieldSize)
{
    FILE *file;
    int i;

    file = fopen("results/initial_conditions.txt", "w");

    if (file == NULL)
    {
        printf("Error creating initial conditions file.\n");
        return;
    }


    fprintf(file, "========================================\n");
    fprintf(file, "       NAVAL BATTLE SIMULATOR\n");
    fprintf(file, "       INITIAL CONDITIONS\n");
    fprintf(file, "========================================\n\n");


    fprintf(file,
            "Battlefield Size: %.2f\n\n",
            battlefieldSize);


    fprintf(file, "BATTLESHIP\n");
    fprintf(file, "Notation: %c\n",
            battleship.notation);

    fprintf(file, "Name: %s\n",
            battleship.name);

    fprintf(file, "Gun: %s\n",
            battleship.gunName);

    fprintf(file,
            "Position: (%.2f, %.2f)\n",
            battleship.position.x,
            battleship.position.y);

    fprintf(file,
            "Vmax: %.2f\n",
            battleship.vmax);

    fprintf(file,
            "Minimum Angle: %.2f\n",
            battleship.angleMin);

    fprintf(file,
            "Maximum Angle: %.2f\n\n",
            battleship.angleMax);


    fprintf(file, "ESCORT SHIPS\n");
    fprintf(file, "========================================\n");


    for (i = 0; i < numberOfEscorts; i++)
    {
        fprintf(file, "\nE%d\n", i + 1);

        fprintf(file,
                "Notation: E_%c\n",
                escorts[i].notation);

        fprintf(file,
                "Type: %s\n",
                escorts[i].name);

        fprintf(file,
                "Position: (%.2f, %.2f)\n",
                escorts[i].position.x,
                escorts[i].position.y);

        fprintf(file,
                "Vmax: %.2f\n",
                escorts[i].vmax);

        fprintf(file,
                "Vmin: %.2f\n",
                escorts[i].vmin);

        fprintf(file,
                "Maximum Angle: %.2f\n",
                escorts[i].angleMax);

        fprintf(file,
                "Minimum Angle: %.2f\n",
                escorts[i].angleMin);

        fprintf(file,
                "Impact Power: %.2f\n",
                escorts[i].impactPower);
    }


    fclose(file);
}


void saveFinalConditions(Battleship battleship,
                         EscortShip escorts[],
                         int numberOfEscorts,
                         double battlefieldSize)
{
    FILE *file;
    int i;

    file = fopen("results/final_conditions.txt", "w");

    if (file == NULL)
    {
        printf("Error creating final conditions file.\n");
        return;
    }


    fprintf(file, "========================================\n");
    fprintf(file, "       NAVAL BATTLE SIMULATOR\n");
    fprintf(file, "       FINAL CONDITIONS\n");
    fprintf(file, "========================================\n\n");


    fprintf(file,
            "Battlefield Size: %.2f\n\n",
            battlefieldSize);


    fprintf(file, "BATTLESHIP\n");

    fprintf(file,
            "Notation: %c\n",
            battleship.notation);

    fprintf(file,
            "Name: %s\n",
            battleship.name);

    fprintf(file,
            "Position: (%.2f, %.2f)\n",
            battleship.position.x,
            battleship.position.y);


    fprintf(file, "\nESCORT SHIPS\n");


    for (i = 0; i < numberOfEscorts; i++)
    {
        fprintf(file,
                "E%d - E_%c - %s - "
                "Position (%.2f, %.2f)\n",
                i + 1,
                escorts[i].notation,
                escorts[i].name,
                escorts[i].position.x,
                escorts[i].position.y);
    }


    fclose(file);
}
