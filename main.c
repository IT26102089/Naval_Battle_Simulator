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

        printf("\n--------------------------------------------\n");
        printf("ITERATION %d\n", iteration + 1);
        printf("B position: (%.2f, %.2f)\n",
               b.position.x, b.position.y);
        if (jammed && iteration >= jamAfter)
            printf("B gun angle restriction: %.2f - 90.00 degrees\n", jamMinAngle);
        else
            printf("B gun angle range: 0.00 - 90.00 degrees\n");


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

                printf("B hit E%d E_%c | Time to hit: %.2f seconds\n",
                       e[i].index, e[i].notation, hitTime);
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
                e[i].shotsFired++;

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

                    fprintf(file,
                            "E%d hit B: %.2f%% impact\n",
                            e[i].index,
                            e[i].impactPower * 100.0);

                    printf("E%d hit B: %.2f%% impact\n",
                           e[i].index, e[i].impactPower * 100.0);


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
        printf("E ships hit by B this iteration: %d\n", escortsHit);


        if (b.destroyed)
        {
            fprintf(file,
                    "B DESTROYED\n");

            fprintf(file,
                    "E%d sank B\n",
                    sinkingEscort);

            fprintf(file,
                    "Time to impact: %.2f seconds\n",
                    battleTime);

            fprintf(file,
                    "Simulation stopped at iteration %d\n\n",
                    iteration + 1);

            printf("B DESTROYED\n");
            printf("E%d sank B\n", sinkingEscort);
            printf("Time to impact: %.2f seconds\n", battleTime);
            printf("Simulation stopped at iteration %d\n", iteration + 1);

            break;
        }


        if (partCMode)
        {
            fprintf(file,
                    "Cumulative B impact: %.2f%%\n",
                    cumulativeImpact * 100.0);
            printf("Cumulative B impact: %.2f%%\n", cumulativeImpact * 100.0);
        }


        fprintf(file,
                "B survives this iteration\n\n");
        printf("B survives this iteration\n");
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

    printf("\nFINAL RESULT: B %s\n", b.destroyed ? "DESTROYED" : "ALIVE");
    if (partCMode)
        printf("Cumulative impact on B: %.2f%%\n", cumulativeImpact * 100.0);

    fclose(file);
}


int main(void)
{
    Battleship battleship;
    EscortShip escorts[MAX_ESCORTS];
    Position path[MAX_POINTS];

    int numberOfEscorts;
    double battlefieldSize;
    unsigned int seed;
    int choice;

    int k = 0;
    int t = 0;
    double jamAngle = 0.0;

    printf("\n============================================\n");
    printf("       ADVANCED NAVAL BATTLE SIMULATOR\n");
    printf("============================================\n");

    /* Main menu comes FIRST. */
    do
    {
        printf("\n=============== MAIN MENU ===============\n");
        printf("1. Run Part 1-A\n");
        printf("2. Run Part 1-B\n");
        printf("3. Run Part 1-C\n");
        printf("4. Run ALL Part 1 (A + B + C)\n");
        printf("5. Exit\n");
        printf("==========================================\n");
        printf("Select an option: ");
        scanf("%d", &choice);

        if (choice < 1 || choice > 5)
            printf("Invalid option. Please select 1-5.\n");
    }
    while (choice < 1 || choice > 5);

    if (choice == 5)
    {
        printf("\nExiting simulator. Goodbye!\n");
        return 0;
    }

    /* Common initial conditions. */
    printf("\nEnter random seed: ");
    scanf("%u", &seed);
    srand(seed);

    printf("\nEnter battlefield size D: ");
    scanf("%lf", &battlefieldSize);
    while (battlefieldSize <= 0)
    {
        printf("D must be positive: ");
        scanf("%lf", &battlefieldSize);
    }

    setupBattleship(&battleship);

    do
    {
        printf("\nEnter Battleship X (0 - %.2f): ", battlefieldSize);
        scanf("%lf", &battleship.position.x);
    }
    while (battleship.position.x < 0 ||
           battleship.position.x > battlefieldSize);

    do
    {
        printf("Enter Battleship Y (0 - %.2f): ", battlefieldSize);
        scanf("%lf", &battleship.position.y);
    }
    while (battleship.position.y < 0 ||
           battleship.position.y > battlefieldSize);

    do
    {
        printf("\nEnter number of Escort Ships (1-%d): ", MAX_ESCORTS);
        scanf("%d", &numberOfEscorts);
        if (numberOfEscorts <= 0 || numberOfEscorts > MAX_ESCORTS)
            printf("Please enter a value between 1 and %d.\n", MAX_ESCORTS);
    }
    while (numberOfEscorts <= 0 || numberOfEscorts > MAX_ESCORTS);

    createEscortShips(escorts, numberOfEscorts,
                      battlefieldSize, battleship.vmax);

    displayBattlefield(battlefieldSize, battleship,
                       escorts, numberOfEscorts);

    /* Part 1-A */
    if (choice == 1 || choice == 4)
    {
        printf("\n============================================\n");
        printf("               PART 1-A\n");
        printf("============================================\n");

        savePart1AInitial(battleship, escorts,
                          numberOfEscorts, battlefieldSize);
        runPart1A(battleship, escorts, numberOfEscorts, 0);
    }

    /* Part 1-B needs k, t and jam angle. */
    if (choice == 2 || choice == 4)
    {
        printf("\n============================================\n");
        printf("               PART 1-B\n");
        printf("============================================\n");

        do
        {
            printf("\nEnter k (number of path points, max %d): ", MAX_POINTS);
            scanf("%d", &k);
            if (k <= 0 || k > MAX_POINTS)
                printf("Please enter a value between 1 and %d.\n", MAX_POINTS);
        }
        while (k <= 0 || k > MAX_POINTS);

        generatePath(path, k, battlefieldSize);

        do
        {
            printf("Enter t (jam point, 1 to %d): ", k - 1);
            scanf("%d", &t);
            if (t < 1 || t >= k)
                printf("t must satisfy 0 < t < k.\n");
        }
        while (t < 1 || t >= k);

        do
        {
            printf("Enter jammed minimum angle (0-30): ");
            scanf("%lf", &jamAngle);
            if (jamAngle <= 0 || jamAngle >= 30)
                printf("Angle must satisfy 0 < theta_min < 30.\n");
        }
        while (jamAngle <= 0 || jamAngle >= 30);

        printf("\n========== SIMULATION 1 ==========\n");
        runPart1B(battleship, escorts, numberOfEscorts,
                  path, k, t, jamAngle, 0, 0);

        printf("\n========== SIMULATION 2 ==========\n");
        runPart1B(battleship, escorts, numberOfEscorts,
                  path, k, t, jamAngle, 0, 1);
    }

    /* Part 1-C includes its own A, B1 and B2 simulations. */
    if (choice == 3 || choice == 4)
    {
        printf("\n============================================\n");
        printf("               PART 1-C\n");
        printf("============================================\n");

        if (choice == 3)
        {
            do
            {
                printf("\nEnter k (number of path points, max %d): ", MAX_POINTS);
                scanf("%d", &k);
                if (k <= 0 || k > MAX_POINTS)
                    printf("Please enter a value between 1 and %d.\n", MAX_POINTS);
            }
            while (k <= 0 || k > MAX_POINTS);

            generatePath(path, k, battlefieldSize);

            do
            {
                printf("Enter t (jam point, 1 to %d): ", k - 1);
                scanf("%d", &t);
                if (t < 1 || t >= k)
                    printf("t must satisfy 0 < t < k.\n");
            }
            while (t < 1 || t >= k);

            do
            {
                printf("Enter jammed minimum angle (0-30): ");
                scanf("%lf", &jamAngle);
                if (jamAngle <= 0 || jamAngle >= 30)
                    printf("Angle must satisfy 0 < theta_min < 30.\n");
            }
            while (jamAngle <= 0 || jamAngle >= 30);
        }

        printf("\nRunning Part 1-C Simulation A...\n");
        runPart1A(battleship, escorts, numberOfEscorts, 1);

        printf("\nRunning Part 1-C Simulation B1...\n");
        runPart1B(battleship, escorts, numberOfEscorts,
                  path, k, t, jamAngle, 1, 0);

        printf("\nRunning Part 1-C Simulation B2...\n");
        runPart1B(battleship, escorts, numberOfEscorts,
                  path, k, t, jamAngle, 1, 1);
    }

    printf("\n============================================\n");
    printf("              SIMULATION COMPLETE\n");
    printf("============================================\n");

    return 0;
}
