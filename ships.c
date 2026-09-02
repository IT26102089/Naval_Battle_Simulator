#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "ships.h"


void setupBattleship(Battleship *battleship)
{
    int choice;

    printf("\n");
    printf("========================================\n");
    printf("       BATTLESHIP TYPE SELECTION\n");
    printf("========================================\n");

    printf("1. U - USS Iowa (BB-61)\n");
    printf("2. M - MS King George V\n");
    printf("3. R - Richelieu\n");
    printf("4. S - Sovetsky Soyuz-class\n");

    printf("\nSelect battleship type: ");
    scanf("%d", &choice);

    switch (choice)
    {
        case 1:
            battleship->notation = 'U';

            strcpy(battleship->name,
                   "USS Iowa (BB-61)");

            strcpy(battleship->gunName,
                   "50-caliber Mark 7 gun");

            break;

        case 2:
            battleship->notation = 'M';

            strcpy(battleship->name,
                   "MS King George V");

            strcpy(battleship->gunName,
                   "356 mm Mark VII gun");

            break;

        case 3:
            battleship->notation = 'R';

            strcpy(battleship->name,
                   "Richelieu");

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
            printf("Invalid choice. Using USS Iowa.\n");

            battleship->notation = 'U';

            strcpy(battleship->name,
                   "USS Iowa (BB-61)");

            strcpy(battleship->gunName,
                   "50-caliber Mark 7 gun");
    }

    /*
     * Placeholder Vmax.
     *
     * We will replace this with the exact
     * projectile/battleship velocity model
     * from the assignment.
     */
    battleship->vmax = 100.0;

    battleship->angleMin = 0.0;
    battleship->angleMax = 90.0;
}


void createEscortShips(EscortShip escorts[],
                       int numberOfEscorts,
                       double battlefieldSize,
                       double battleshipVmax)
{
    int i;
    int type;

    srand((unsigned int)time(NULL));

    for (i = 0; i < numberOfEscorts; i++)
    {
        type = rand() % ESCORT_TYPES;

        /*
         * Generate position.
         */
        escorts[i].position.x =
            ((double)rand() / RAND_MAX) * battlefieldSize;

        escorts[i].position.y =
            ((double)rand() / RAND_MAX) * battlefieldSize;


        /*
         * Assign escort type.
         */
        switch (type)
        {
            case 0:

                escorts[i].notation = 'A';

                strcpy(escorts[i].name,
                       "1936A-class Destroyer");

                /*
                 * Table 1:
                 * Impact Power = 0.08
                 * Angle range = 20 degrees
                 */
                escorts[i].impactPower = 0.08;
                escorts[i].angleMax = 20.0;

                /*
                 * Random minimum angle.
                 */
                escorts[i].angleMin =
                    ((double)rand() / RAND_MAX) *
                    escorts[i].angleMax;

                /*
                 * EA maximum velocity:
                 *
                 * Vmax = 1.2 * Vmax(B)
                 */
                escorts[i].vmax =
                    1.2 * battleshipVmax;

                break;


            case 1:

                escorts[i].notation = 'B';

                strcpy(escorts[i].name,
                       "Gabbiano-class Corvette");

                /*
                 * Table 1:
                 * Impact Power = 0.06
                 * Angle range = 30 degrees
                 */
                escorts[i].impactPower = 0.06;
                escorts[i].angleMax = 30.0;

                escorts[i].angleMin =
                    ((double)rand() / RAND_MAX) *
                    escorts[i].angleMax;

                /*
                 * Other escort maximum velocities
                 * must be less than B Vmax.
                 */
                escorts[i].vmax =
                    ((double)rand() / RAND_MAX)
                    * battleshipVmax;

                break;


            case 2:

                escorts[i].notation = 'C';

                strcpy(escorts[i].name,
                       "Matsu-class Destroyer");

                /*
                 * Table 1:
                 * Impact Power = 0.07
                 * Angle range = 25 degrees
                 */
                escorts[i].impactPower = 0.07;
                escorts[i].angleMax = 25.0;

                escorts[i].angleMin =
                    ((double)rand() / RAND_MAX) *
                    escorts[i].angleMax;

                escorts[i].vmax =
                    ((double)rand() / RAND_MAX)
                    * battleshipVmax;

                break;


            case 3:

                escorts[i].notation = 'D';

                strcpy(escorts[i].name,
                       "F-class Escort Ships");

                /*
                 * Table 1:
                 * Impact Power = 0.05
                 * Angle range = 50 degrees
                 */
                escorts[i].impactPower = 0.05;
                escorts[i].angleMax = 50.0;

                escorts[i].angleMin =
                    ((double)rand() / RAND_MAX) *
                    escorts[i].angleMax;

                escorts[i].vmax =
                    ((double)rand() / RAND_MAX)
                    * battleshipVmax;

                break;


            default:

                escorts[i].notation = 'E';

                strcpy(escorts[i].name,
                       "Japanese Kaibokan");

                /*
                 * Table 1:
                 * Impact Power = 0.04
                 * Angle range = 70 degrees
                 */
                escorts[i].impactPower = 0.04;
                escorts[i].angleMax = 70.0;

                escorts[i].angleMin =
                    ((double)rand() / RAND_MAX) *
                    escorts[i].angleMax;

                escorts[i].vmax =
                    ((double)rand() / RAND_MAX)
                    * battleshipVmax;

                break;
        }


        /*
         * Minimum velocity is randomly generated.
         *
         * It must be lower than Vmax.
         */
        escorts[i].vmin =
            ((double)rand() / RAND_MAX)
            * escorts[i].vmax;
    }
}
