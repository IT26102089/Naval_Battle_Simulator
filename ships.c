#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ships.h"


void setupBattleship(Battleship *battleship)
{
    int choice;

    printf("\n========================================\n");
    printf("        BATTLESHIP TYPE\n");
    printf("========================================\n");

    printf("1. U - USS Iowa (BB-61)\n");
    printf("2. M - MS King George V\n");
    printf("3. R - Richelieu\n");
    printf("4. S - Sovetsky Soyuz-class\n");

    printf("Select type: ");
    scanf("%d", &choice);

    switch (choice)
    {
        case 1:
            battleship->notation = 'U';
            strcpy(battleship->name, "USS Iowa (BB-61)");
            strcpy(battleship->gunName,
                   "50-caliber Mark 7 gun");
            break;

        case 2:
            battleship->notation = 'M';
            strcpy(battleship->name, "MS King George V");
            strcpy(battleship->gunName,
                   "356 mm Mark VII gun");
            break;

        case 3:
            battleship->notation = 'R';
            strcpy(battleship->name, "Richelieu");
            strcpy(battleship->gunName,
                   "15 inch Mle 1935 gun");
            break;

        case 4:
            battleship->notation = 'S';
            strcpy(battleship->name,
                   "Sovetsky Soyuz-class");
            strcpy(battleship->gunName,
                   "16 inch B-37 gun");
            break;

        default:
            printf("Invalid choice. Using U.\n");

            battleship->notation = 'U';
            strcpy(battleship->name,
                   "USS Iowa (BB-61)");
            strcpy(battleship->gunName,
                   "50-caliber Mark 7 gun");
            break;
    }

    printf("Enter Battleship shell Vmax: ");
    scanf("%lf", &battleship->vmax);

    while (battleship->vmax <= 0)
    {
        printf("Vmax must be greater than 0: ");
        scanf("%lf", &battleship->vmax);
    }

    battleship->angleMin = 0.0;
    battleship->angleMax = 90.0;

    battleship->destroyed = 0;
    battleship->shotsFired = 0;

    /*
     * Gamma is mainly used in Part 2-C.
     * Keep a small value for now.
     */
    battleship->gamma = 0.001;

    battleship->currentImpact = 1.0;
}


void createEscortShips(
    EscortShip escorts[],
    int numberOfEscorts,
    double battlefieldSize,
    double battleshipVmax)
{
    int i;
    int type;

    for (i = 0; i < numberOfEscorts; i++)
    {
        escorts[i].index = i + 1;

        escorts[i].destroyed = 0;
        escorts[i].shotsFired = 0;
        escorts[i].currentImpact = 1.0;

        /*
         * Random position.
         */
        escorts[i].position.x =
            ((double)rand() / RAND_MAX)
            * battlefieldSize;

        escorts[i].position.y =
            ((double)rand() / RAND_MAX)
            * battlefieldSize;


        /*
         * Random escort type.
         */
        type = rand() % 5;


        switch (type)
        {
            case 0:

                escorts[i].notation = 'A';

                strcpy(escorts[i].name,
                       "1936A-class Destroyer");

                escorts[i].impactPower = 0.08;

                escorts[i].angleRange = 20.0;

                /*
                 * EA can have 1.2 * B Vmax.
                 */
                escorts[i].vmax =
                    1.2 * battleshipVmax;

                break;


            case 1:

                escorts[i].notation = 'B';

                strcpy(escorts[i].name,
                       "Gabbiano-class Corvette");

                escorts[i].impactPower = 0.06;

                escorts[i].angleRange = 30.0;

                escorts[i].vmax =
                    0.8 * battleshipVmax;

                break;


            case 2:

                escorts[i].notation = 'C';

                strcpy(escorts[i].name,
                       "Matsu-class Destroyer");

                escorts[i].impactPower = 0.07;

                escorts[i].angleRange = 25.0;

                escorts[i].vmax =
                    0.8 * battleshipVmax;

                break;


            case 3:

                escorts[i].notation = 'D';

                strcpy(escorts[i].name,
                       "F-class Escort Ships");

                escorts[i].impactPower = 0.05;

                escorts[i].angleRange = 50.0;

                escorts[i].vmax =
                    0.8 * battleshipVmax;

                break;


            default:

                escorts[i].notation = 'E';

                strcpy(escorts[i].name,
                       "Japanese Kaibokan");

                escorts[i].impactPower = 0.04;

                escorts[i].angleRange = 70.0;

                escorts[i].vmax =
                    0.8 * battleshipVmax;

                break;
        }


        /*
         * Vmin randomly generated from 0 to Vmax.
         */
        escorts[i].vmin =
            ((double)rand() / RAND_MAX)
            * escorts[i].vmax;


        /*
         * Random theta_L.
         */
        escorts[i].angleMin =
            ((double)rand() / RAND_MAX)
            * (90.0 - escorts[i].angleRange);
    }
}
