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

    int escortsHit = 0;

    int sinkingEscort = -1;

    double hitTime;

    double maximumBattleTime = 0.0;


    printf("\n");
    printf("================================================\n");
    printf("          NAVAL BATTLE SIMULATOR\n");
    printf("                 PART 1-A\n");
    printf("================================================\n");


    /*
     * Get battlefield size.
     */
    printf("\nEnter battlefield size: ");
    scanf("%lf", &battlefieldSize);


    if (battlefieldSize <= 0)
    {
        printf("Invalid battlefield size.\n");
        return 1;
    }


    /*
     * Setup Battleship.
     */
    setupBattleship(&battleship);


    /*
     * Battleship position.
     */
    printf("\nEnter Battleship X position: ");
    scanf("%lf", &battleship.position.x);

    printf("Enter Battleship Y position: ");
    scanf("%lf", &battleship.position.y);


    if (battleship.position.x < 0 ||
        battleship.position.x > battlefieldSize ||
        battleship.position.y < 0 ||
        battleship.position.y > battlefieldSize)
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
     * Generate Escort ships.
     */
    createEscortShips(
        escorts,
        numberOfEscorts,
        battlefieldSize,
        battleship.vmax);


    /*
     * Save initial conditions BEFORE battle.
     */
    saveInitialConditions(
        battleship,
        escorts,
        numberOfEscorts,
        battlefieldSize);


    /*
     * Show battlefield.
     */
    displayBattlefield(
        battlefieldSize,
        battleship,
        escorts,
        numberOfEscorts);


    /*
     * ==========================================
     * STEP 1
     *
     * Battleship fires.
     *
     * According to Part 1-A:
     * B can reload and fire in 0 seconds.
     *
     * Therefore B can attack every E
     * that is inside its attack range.
     * ==========================================
     */

    printf("\n");
    printf("============================================\n");
    printf("          BATTLESHIPS ATTACK\n");
    printf("============================================\n");


    for (i = 0;
         i < numberOfEscorts;
         i++)
    {
        hitTime = 0.0;


        if (canBattleshipHitEscort(
                battleship,
                escorts[i],
                &hitTime))
        {
            /*
             * One shell destroys one E.
             */
            escorts[i].destroyed = 1;

            escortsHit++;


            printf("\nB hit E%d (E_%c)\n",
                   i + 1,
                   escorts[i].notation);

            printf("Time to hit: %.2f seconds\n",
                   hitTime);


            /*
             * Battle ends when the last B
             * projectile hits.
             */
            if (hitTime > maximumBattleTime)
            {
                maximumBattleTime = hitTime;
            }
        }
    }


    /*
     * ==========================================
     * STEP 2
     *
     * Remaining E ships fire once.
     *
     * Destroyed E ships cannot fire because
     * they were destroyed by B.
     * ==========================================
     */

    printf("\n");
    printf("============================================\n");
    printf("             ESCORT ATTACK\n");
    printf("============================================\n");


    for (i = 0;
         i < numberOfEscorts;
         i++)
    {
        /*
         * Destroyed E cannot attack.
         */
        if (escorts[i].destroyed)
        {
            continue;
        }


        hitTime = 0.0;


        if (canEscortHitBattleship(
                battleship,
                escorts[i],
                &hitTime))
        {
            /*
             * One shell impact destroys B.
             */
            battleship.destroyed = 1;

            sinkingEscort = i + 1;


            printf("\nE%d (E_%c) hit B!\n",
                   i + 1,
                   escorts[i].notation);

            printf("Time to hit B: %.2f seconds\n",
                   hitTime);

            break;
        }
    }


    /*
     * ==========================================
     * FINAL RESULT
     * ==========================================
     */

    printf("\n");
    printf("============================================\n");
    printf("             FINAL BATTLE RESULT\n");
    printf("============================================\n");


    if (battleship.destroyed)
    {
        printf("\nBATTLESHIP B HAS SUNK!\n");

        printf("E%d was the Escort ship that sank B.\n",
               sinkingEscort);

        printf("Escort ships destroyed by B: %d\n",
               escortsHit);
    }
    else
    {
        printf("\nBATTLESHIP B SURVIVED!\n");

        printf("Number of E ships hit by B: %d\n",
               escortsHit);

        printf("Time to end battle: %.2f seconds\n",
               maximumBattleTime);


        /*
         * Save details of all E ships hit.
         */
        {
            FILE *file;

            file =
                fopen(
                    "results/escort_hits.txt",
                    "w");


            if (file != NULL)
            {
                fprintf(file,
                        "PART 1-A - ESCORT SHIPS HIT BY B\n\n");


                fprintf(file,
                        "Number of E ships hit: %d\n\n",
                        escortsHit);


                for (i = 0;
                     i < numberOfEscorts;
                     i++)
                {
                    if (escorts[i].destroyed)
                    {
                        hitTime = 0.0;

                        canBattleshipHitEscort(
                            battleship,
                            escorts[i],
                            &hitTime);


                        fprintf(file,
                                "Index: E%d\n",
                                i + 1);

                        fprintf(file,
                                "Type: E_%c\n",
                                escorts[i].notation);

                        fprintf(file,
                                "Ship: %s\n",
                                escorts[i].name);

                        fprintf(file,
                                "Time to hit: %.2f seconds\n\n",
                                hitTime);
                    }
                }

                fclose(file);
            }
        }
    }


    /*
     * Save final conditions.
     */
    saveFinalConditions(
        battleship,
        escorts,
        numberOfEscorts,
        battlefieldSize,
        escortsHit,
        sinkingEscort);


    printf("\n");
    printf("Initial conditions saved:\n");
    printf("results/initial_conditions.txt\n");

    printf("\nFinal conditions saved:\n");
    printf("results/final_conditions.txt\n");

    if (!battleship.destroyed)
    {
        printf("\nHit details saved:\n");
        printf("results/escort_hits.txt\n");
    }


    printf("\nPart 1-A simulation complete.\n");


    return 0;
}
