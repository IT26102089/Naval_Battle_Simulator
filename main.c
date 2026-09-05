#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "ships.h"
#include "battle.h"


/*
 * Copy escort array.
 */
void copyEscorts(
    EscortShip destination[],
    EscortShip source[],
    int number)
{
    int i;

    for (i = 0; i < number; i++)
    {
        destination[i] = source[i];
    }
}


/*
 * Find earliest escort that can sink B.
 *
 * This fixes the problem in the previous version:
 * we don't simply use E1, E2, E3 order.
 */
int findSinkingEscort(
    Battleship battleship,
    EscortShip escorts[],
    int numberOfEscorts,
    double *earliestTime)
{
    int i;

    int sinkingEscort = -1;

    double bestTime = 999999999.0;

    double hitTime;


    for (i = 0;
         i < numberOfEscorts;
         i++)
    {
        if (escorts[i].destroyed)
            continue;


        if (canEscortHitBattleship(
                battleship,
                escorts[i],
                &hitTime))
        {
            if (hitTime < bestTime)
            {
                bestTime = hitTime;
                sinkingEscort = i + 1;
            }
        }
    }


    if (sinkingEscort != -1)
    {
        *earliestTime = bestTime;
    }


    return sinkingEscort;
}


/*
 * Part 1-A.
 *
 * normalMode = 1:
 *   E attacks can destroy B.
 *
 * partCMode = 0:
 *   one attack destroys B.
 *
 * partCMode = 1:
 *   cumulative impact is used.
 */
void runPart1A(
    Battleship originalB,
    EscortShip originalE[],
    int numberOfEscorts,
    int partCMode)
{
    Battleship b;
    EscortShip e[MAX_ESCORTS];

    int i;

    int escortsHit = 0;

    double hitTime;

    double battleEndTime = 0.0;

    double cumulativeImpact = 0.0;

    int sinkingEscort;


    b = originalB;

    copyEscorts(
        e,
        originalE,
        numberOfEscorts);


    /*
     * B attacks every E in range.
     *
     * Reload time = 0.
     */
    for (i = 0;
         i < numberOfEscorts;
         i++)
    {
        if (e[i].destroyed)
            continue;


        if (canBattleshipHitEscort(
                b,
                e[i],
                0.0,
                90.0,
                &hitTime))
        {
            e[i].destroyed = 1;

            escortsHit++;

            b.shotsFired++;

            if (hitTime > battleEndTime)
                battleEndTime = hitTime;
        }
    }


    /*
     * Now remaining E ships attack B.
     */
    sinkingEscort =
        findSinkingEscort(
            b,
            e,
            numberOfEscorts,
            &hitTime);


    /*
     * Part 1-A:
     * one hit destroys B.
     */
    if (!partCMode)
    {
        if (sinkingEscort != -1)
        {
            b.destroyed = 1;

            printf("\nB HAS BEEN SUNK!\n");
            printf("E%d sank B.\n",
                   sinkingEscort);

            printf("Time to impact: %.2f seconds\n",
                   hitTime);
        }
        else
        {
            printf("\nB SURVIVED!\n");

            printf("E ships hit by B: %d\n",
                   escortsHit);

            printf("Battle end time: %.2f seconds\n",
                   battleEndTime);
        }


        savePart1AFinal(
            b,
            e,
            numberOfEscorts,
            escortsHit,
            sinkingEscort);


        /*
         * Save hit details.
         */
        {
            FILE *file =
                fopen(
                    "results/part1A_hits.txt",
                    "w");

            if (file != NULL)
            {
                fprintf(file,
                        "PART 1-A ESCORT HITS\n\n");

                fprintf(file,
                        "Number hit: %d\n\n",
                        escortsHit);


                for (i = 0;
                     i < numberOfEscorts;
                     i++)
                {
                    if (e[i].destroyed)
                    {
                        if (canBattleshipHitEscort(
                                b,
                                e[i],
                                0.0,
                                90.0,
                                &hitTime))
                        {
                            fprintf(file,
                                    "E%d E_%c\n",
                                    e[i].index,
                                    e[i].notation);

                            fprintf(file,
                                    "Time: %.2f seconds\n\n",
                                    hitTime);
                        }
                    }
                }

                fclose(file);
            }
        }

        return;
    }


    /*
     * ================================
     * PART 1-C
     * ================================
     *
     * E ships that survive B's attack
     * can attack B.
     */
    for (i = 0;
         i < numberOfEscorts;
         i++)
    {
        if (e[i].destroyed)
            continue;


        if (canEscortHitBattleship(
                b,
                e[i],
                &hitTime))
        {
            /*
             * E attacks only once in Part 1-C.
             */
            cumulativeImpact +=
                e[i].impactPower;

            e[i].shotsFired++;

            e[i].currentImpact =
                e[i].impactPower;

            printf("\nE%d attacked B: %.2f%% damage\n",
                   e[i].index,
                   e[i].impactPower * 100.0);


            if (cumulativeImpact >= 1.0)
            {
                b.destroyed = 1;

                sinkingEscort =
                    e[i].index;

                printf("\nB HAS BEEN SUNK!\n");
                printf("Cumulative impact: %.2f%%\n",
                       cumulativeImpact * 100.0);

                printf("E%d contributed the final impact.\n",
                       sinkingEscort);

                break;
            }
        }
    }


    if (!b.destroyed)
    {
        printf("\nB SURVIVED PART 1-C!\n");

        printf("E ships hit by B: %d\n",
               escortsHit);

        printf("Cumulative impact on B: %.2f%%\n",
               cumulativeImpact * 100.0);
    }


    /*
     * Save Part 1-C result.
     */
    {
        FILE *file =
            fopen(
                "results/part1C_simulationA.txt",
                "w");


        if (file != NULL)
        {
            fprintf(file,
                    "PART 1-C SIMULATION A\n\n");

            fprintf(file,
                    "B status: %s\n",
                    b.destroyed
                        ? "DESTROYED"
                        : "ALIVE");

            fprintf(file,
                    "Escort ships destroyed by B: %d\n",
                    escortsHit);

            fprintf(file,
                    "Cumulative impact on B: %.2f%%\n\n",
                    cumulativeImpact * 100.0);


            for (i = 0;
                 i < numberOfEscorts;
                 i++)
            {
                fprintf(file,
                        "E%d E_%c\n",
                        e[i].index,
                        e[i].notation);

                fprintf(file,
                        "Destroyed: %s\n",
                        e[i].destroyed
                            ? "YES"
                            : "NO");

                fprintf(file,
                        "Impact power: %.2f\n",
                        e[i].impactPower);

                fprintf(file,
                        "Shots fired: %d\n\n",
                        e[i].shotsFired);
            }


            fclose(file);
        }
    }
}


/*
 * Generate a path of k points.
 */
void generatePath(
    Position path[],
    int k,
    double battlefieldSize)
{
    int i;

    for (i = 0; i < k; i++)
    {
        path[i].x =
            ((double)rand() / RAND_MAX)
            * battlefieldSize;

        path[i].y =
            ((double)rand() / RAND_MAX)
            * battlefieldSize;
    }
}


/*
 * Part 1-B Simulation.
 *
 * jammed = 0:
 * Simulation 1.
 *
 * jammed = 1:
 * Simulation 2.
 */
void runPart1B(
    Battleship originalB,
    EscortShip originalE[],
    int numberOfEscorts,
    Position path[],
    int k,
    int jamAfter,
    double jamMinAngle,
    int partCMode,
    int jammed)
{
    Battleship b;
    EscortShip e[MAX_ESCORTS];

    FILE *file;

    int iteration;
    int i;

    int escortsHit;

    double hitTime;

    double battleTime;

    double cumulativeImpact;

    int sinkingEscort;


    b = originalB;

    copyEscorts(
        e,
        originalE,
        numberOfEscorts);


    if (partCMode)
    {
        file =
            fopen(
                jammed
                    ? "results/part1C_simulationB2.txt"
                    : "results/part1C_simulationB1.txt",
                "w");
    }
    else
    {
        file =
            fopen(
                jammed
                    ? "results/part1B_simulation2.txt"
                    : "results/part1B_simulation1.txt",
                "w");
    }


    if (file == NULL)
    {
        printf("Could not create result file.\n");
        return;
    }


    fprintf(file,
            "NAVAL BATTLE SIMULATOR\n");


    if (partCMode)
        fprintf(file,
                "PART 1-C ");
    else
        fprintf(file,
                "PART 1-B ");


    fprintf(file,
            "%s\n\n",
            jammed
                ? "SIMULATION 2"
                : "SIMULATION 1");


    fprintf(file,
            "Iterations: %d\n",
            k);


    if (jammed)
    {
        fprintf(file,
                "B gun jammed after iteration: %d\n",
                jamAfter);

        fprintf(file,
                "B minimum firing angle after jam: %.2f\n\n",
                jamMinAngle);
    }


    cumulativeImpact = 0.0;


    for (iteration = 0;
         iteration < k;
         iteration++)
    {
        /*
         * Move B to next generated point.
         */
        b.position = path[iteration];


        /*
         * Determine B angle range.
         */
        double minAngle = 0.0;

        double maxAngle = 90.0;


        if (jammed &&
            iteration >= jamAfter)
        {
            minAngle =
                jamMinAngle;
        }


        escortsHit = 0;


        fprintf(file,
                "====================================\n");

        fprintf(file,
                "ITERATION %d\n",
                iteration + 1);

        fprintf(file,
                "B position: (%.2f, %.2f)\n",
                b.position.x,
                b.position.y);


        /*
         * B fires at all surviving E ships.
         */
        for (i = 0;
             i < numberOfEscorts;
             i++)
        {
            if (e[i].destroyed)
                continue;


            if (canBattleshipHitEscort(
                    b,
                    e[i],
                    minAngle,
                    maxAngle,
                    &hitTime))
            {
                e[i].destroyed = 1;

                escortsHit++;

                b.shotsFired++;


                fprintf(file,
                        "B hit E%d E_%c\n",
                        e[i].index,
                        e[i].notation);

                fprintf(file,
                        "Time to hit: %.2f\n",
                        hitTime);
            }
        }


        /*
         * E attacks B.
         */
        sinkingEscort = -1;

        battleTime = 0.0;


        for (i = 0;
             i < numberOfEscorts;
             i++)
        {
            if (e[i].destroyed)
                continue;


            if (canEscortHitBattleship(
                    b,
                    e[i],
                    &hitTime))
            {
                /*
                 * Part 1-B:
                 * one hit destroys B.
                 */
                if (!partCMode)
                {
                    if (sinkingEscort == -1 ||
                        hitTime < battleTime)
                    {
                        sinkingEscort =
                            e[i].index;

                        battleTime =
                            hitTime;
                    }
                }
                else
                {
                    /*
                     * Part 1-C:
                     * cumulative impact.
                     */
                    cumulativeImpact +=
                        e[i].impactPower;

                    e[i].shotsFired++;

                    fprintf(file,
                            "E%d hit B: %.2f%% impact\n",
                            e[i].index,
                            e[i].impactPower * 100.0);


                    if (cumulativeImpact >= 1.0)
                    {
                        sinkingEscort =
                            e[i].index;

                        b.destroyed = 1;

                        break;
                    }
                }
            }
        }


        /*
         * For Part 1-B, destroy B after
         * determining earliest attacker.
         */
        if (!partCMode &&
            sinkingEscort != -1)
        {
            b.destroyed = 1;
        }


        fprintf(file,
                "E ships hit this iteration: %d\n",
                escortsHit);


        if (b.destroyed)
        {
            fprintf(file,
                    "B DESTROYED\n");

            fprintf(file,
                    "E%d sank B\n",
                    sinkingEscort);

            fprintf(file,
                    "Simulation stopped at iteration %d\n\n",
                    iteration + 1);

            break;
        }


        if (partCMode)
        {
            fprintf(file,
                    "Cumulative B impact: %.2f%%\n",
                    cumulativeImpact * 100.0);
        }


        fprintf(file,
                "B survives this iteration\n\n");
    }


    fprintf(file,
            "\nFINAL RESULT\n");


    fprintf(file,
            "B status: %s\n",
            b.destroyed
                ? "DESTROYED"
                : "ALIVE");


    fprintf(file,
            "B shots fired: %d\n",
            b.shotsFired);


    if (partCMode)
    {
        fprintf(file,
                "Cumulative impact: %.2f%%\n",
                cumulativeImpact * 100.0);
    }


    fclose(file);
}


int main(void)
{
    Battleship battleship;

    EscortShip escorts[MAX_ESCORTS];

    int numberOfEscorts;

    double battlefieldSize;

    unsigned int seed;

    Position path[MAX_POINTS];

    int k;

    int t;

    double jamAngle;


    printf("\n");
    printf("============================================\n");
    printf("       ADVANCED NAVAL BATTLE SIMULATOR\n");
    printf("============================================\n");


    /*
     * Seed.
     */
    printf("\nEnter random seed: ");
    scanf("%u", &seed);

    srand(seed);


    /*
     * Battlefield.
     */
    printf("\nEnter battlefield size D: ");
    scanf("%lf", &battlefieldSize);


    while (battlefieldSize <= 0)
    {
        printf("D must be positive: ");
        scanf("%lf", &battlefieldSize);
    }


    /*
     * Battleship.
     */
    setupBattleship(&battleship);


    printf("\nEnter Battleship X: ");
    scanf("%lf",
          &battleship.position.x);

    printf("Enter Battleship Y: ");
    scanf("%lf",
          &battleship.position.y);


    /*
     * Escort count.
     */
    printf("\nEnter number of Escort Ships: ");
    scanf("%d", &numberOfEscorts);


    if (numberOfEscorts <= 0 ||
        numberOfEscorts > MAX_ESCORTS)
    {
        printf("Invalid number.\n");
        return 1;
    }


    /*
     * Generate E ships.
     */
    createEscortShips(
        escorts,
        numberOfEscorts,
        battlefieldSize,
        battleship.vmax);


    /*
     * Display battlefield.
     */
    displayBattlefield(
        battlefieldSize,
        battleship,
        escorts,
        numberOfEscorts);


    /*
     * Create results directory externally
     * using mkdir.
     */


    /*
     * ==========================================
     * PART 1-A
     * ==========================================
     */
    printf("\n");
    printf("============================================\n");
    printf("               PART 1-A\n");
    printf("============================================\n");


    savePart1AInitial(
        battleship,
        escorts,
        numberOfEscorts,
        battlefieldSize);


    runPart1A(
        battleship,
        escorts,
        numberOfEscorts,
        0);


    /*
     * ==========================================
     * PART 1-B
     * ==========================================
     */

    printf("\n");
    printf("============================================\n");
    printf("               PART 1-B\n");
    printf("============================================\n");


    printf("\nEnter k (number of path points, max %d): ",
           MAX_POINTS);

    scanf("%d", &k);


    if (k <= 0 || k > MAX_POINTS)
    {
        printf("Invalid k. Using 10.\n");
        k = 10;
    }


    generatePath(
        path,
        k,
        battlefieldSize);


    /*
     * t must be less than k.
     */
    printf("Enter t (jam point, less than k): ");
    scanf("%d", &t);


    if (t < 1)
        t = 1;

    if (t >= k)
        t = k - 1;


    /*
     * Required:
     * 0 < theta_min < 30
     */
    printf("Enter jammed minimum angle (0-30): ");
    scanf("%lf", &jamAngle);


    if (jamAngle <= 0 ||
        jamAngle >= 30)
    {
        jamAngle = 15.0;
    }


    /*
     * Simulation 1.
     */
    printf("\nRunning Part 1-B Simulation 1...\n");


    runPart1B(
        battleship,
        escorts,
        numberOfEscorts,
        path,
        k,
        t,
        jamAngle,
	1,
        0);


    /*
     * Simulation 2.
     */
    printf("Running Part 1-B Simulation 2...\n");


    runPart1B(
        battleship,
        escorts,
        numberOfEscorts,
        path,
        k,
        t,
        jamAngle,
	1,
        1);


    /*
     * ==========================================
     * PART 1-C
     * ==========================================
     */

    printf("\n");
    printf("============================================\n");
    printf("               PART 1-C\n");
    printf("============================================\n");


    printf("\nRunning Part 1-C Simulation A...\n");


    runPart1A(
        battleship,
        escorts,
        numberOfEscorts,
        1);


    printf("\nRunning Part 1-C Simulation B1...\n");


    runPart1B(
        battleship,
        escorts,
        numberOfEscorts,
        path,
        k,
        t,
        jamAngle,
        1,
	0);


    printf("\nRunning Part 1-C Simulation B2...\n");


    runPart1B(
        battleship,
        escorts,
        numberOfEscorts,
        path,
        k,
        t,
        jamAngle,
	1,
        1);


    printf("\n");
    printf("============================================\n");
    printf("       PARTS 1-A, 1-B AND 1-C DONE\n");
    printf("============================================\n");


    printf("\nResults saved in results/\n");


    return 0;
}
