#include <stdio.h>

#include "ships.h"
#include "battle.h"


int main(void)
{
    double battlefieldSize;

    Battleship battleship;

    EscortShip escorts[MAX_ESCORTS];

    int numberOfEscorts;

    int i;

    int battleshipDestroyed = 0;

    int escortsHit = 0;


    printf("\n");
    printf("====================================================\n");
    printf("             NAVAL BATTLE SIMULATOR\n");
    printf("                 PART 1-A\n");
    printf("====================================================\n");


    /*
     * Battlefield size.
     */
    printf("\nEnter battlefield size: ");
    scanf("%lf", &battlefieldSize);


    if (battlefieldSize <= 0)
    {
        printf("Invalid battlefield size.\n");
        return 1;
    }


    /*
     * Select battleship.
     */
    setupBattleship(&battleship);


    /*
     * Battleship position.
     */
    printf("\nEnter Battleship X position: ");
    scanf("%lf", &battleship.position.x);

    printf("Enter Battleship Y position: ");
    scanf("%lf", &battleship.position.y);


    if (!isInsideBattlefield(
            battleship.position.x,
            battleship.position.y,
            battlefieldSize))
    {
        printf("Invalid Battleship position.\n");
        return 1;
    }


    /*
     * Number of escort ships.
     */
    printf("\nEnter number of Escort Ships: ");
    scanf("%d", &numberOfEscorts);


    if (numberOfEscorts <= 0 ||
        numberOfEscorts > MAX_ESCORTS)
    {
        printf("Invalid number of Escort Ships.\n");
        return 1;
    }


    /*
     * Create escort ships.
     */
    createEscortShips(
        escorts,
        numberOfEscorts,
        battlefieldSize,
        battleship.vmax);


    /*
     * Save initial conditions.
     */
    saveInitialConditions(
        battleship,
        escorts,
        numberOfEscorts,
        battlefieldSize);


    /*
     * Display battlefield.
     */
    displayBattlefield(
        battlefieldSize,
        battleship,
        escorts,
        numberOfEscorts);


    /*
     * Display ship information.
     */
    printf("\n");
    printf("====================================================\n");
    printf("              BATTLEFIELD INFORMATION\n");
    printf("====================================================\n");

    printf("\nBattleship:\n");

    printf("Notation : %c\n",
           battleship.notation);

    printf("Name     : %s\n",
           battleship.name);

    printf("Gun      : %s\n",
           battleship.gunName);

    printf("Vmax     : %.2f\n",
           battleship.vmax);

    printf("Position : (%.2f, %.2f)\n",
           battleship.position.x,
           battleship.position.y);


    printf("\nEscort Ships:\n");


    for (i = 0;
         i < numberOfEscorts;
         i++)
    {
        printf("\nE%d\n", i + 1);

        printf("Notation      : E_%c\n",
               escorts[i].notation);

        printf("Type          : %s\n",
               escorts[i].name);

        printf("Position      : (%.2f, %.2f)\n",
               escorts[i].position.x,
               escorts[i].position.y);

        printf("Vmin          : %.2f\n",
               escorts[i].vmin);

        printf("Vmax          : %.2f\n",
               escorts[i].vmax);

        printf("Angle range   : %.2f degrees\n",
               escorts[i].angleMax);

        printf("Minimum angle : %.2f degrees\n",
               escorts[i].angleMin);

        printf("Impact power  : %.2f\n",
               escorts[i].impactPower);
    }


    /*
     * PART 1-A:
     *
     * Check whether any Escort can hit B.
     */
    printf("\n");
    printf("====================================================\n");
    printf("                 BATTLE RESULT\n");
    printf("====================================================\n");


    for (i = 0;
         i < numberOfEscorts;
         i++)
    {
        if (canEscortHitBattleship(
                battleship,
                escorts[i]))
        {
            battleshipDestroyed = 1;

            printf("\nE%d (E_%c) can hit B!\n",
                   i + 1,
                   escorts[i].notation);

            printf("Battleship B has been destroyed.\n");

            printf("Index of E that sank B: E%d\n",
                   i + 1);

            break;
        }
    }


    /*
     * If B survives,
     * determine which E ships B can hit.
     */
    if (!battleshipDestroyed)
    {
        for (i = 0;
             i < numberOfEscorts;
             i++)
        {
            if (canBattleshipHitEscort(
                    battleship,
                    escorts[i]))
            {
                escortsHit++;

                printf("\nB can hit E%d (E_%c).\n",
                       i + 1,
                       escorts[i].notation);
            }
        }


        printf("\nBattleship B survived.\n");

        printf("Escort ships hit by B: %d\n",
               escortsHit);

        /*
         * Time is currently zero in Part 1-A
         * because the assignment states that
         * B can reload and fire in 0 seconds.
         */
        printf("Time to end battle: 0 seconds\n");
    }


    /*
     * Save final conditions.
     */
    saveFinalConditions(
        battleship,
        escorts,
        numberOfEscorts,
        battlefieldSize);


    printf("\n");
    printf("Initial conditions saved to:\n");
    printf("results/initial_conditions.txt\n");

    printf("\nFinal conditions saved to:\n");
    printf("results/final_conditions.txt\n");


    printf("\nSimulation complete.\n");


    return 0;
}
