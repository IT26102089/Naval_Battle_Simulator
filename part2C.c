#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "ships.h"
#include "battle.h"

/*
 * Part 2-C
 *
 * Gun firing causes impact power to decrease.
 * IPn = IP0 * e^(-gamma * n)
 *
 * Part 2-C is built on the timed firing idea from Part 2-B.
 */

static void copyShips(EscortShip to[], EscortShip from[], int count)
{
    int i;

    for (i = 0; i < count; i++)
        to[i] = from[i];
}

/* Get the gamma value of an escort ship. */
static double getEscortGamma(EscortShip e[], int index)
{
    return e[index].gamma;
}

/* Calculate the current impact power after n firings. */
static double calculateImpact(double initialImpact,
                              double gamma,
                              int shots)
{
    return initialImpact * exp(-gamma * shots);
}

/*
 * Set the gamma value for all escort ships of the same type.
 */
static void setEscortGammas(EscortShip e[], int count,
                            double gammaValues[])
{
    int i;

    for (i = 0; i < count; i++)
    {
        switch (e[i].notation)
        {
            case 'A': e[i].gamma = gammaValues[0]; break;
            case 'B': e[i].gamma = gammaValues[1]; break;
            case 'C': e[i].gamma = gammaValues[2]; break;
            case 'D': e[i].gamma = gammaValues[3]; break;
            default:  e[i].gamma = gammaValues[4]; break;
        }
    }
}

/*
 * Choose an E ship using the same simple strategy as Part 2-A/B.
 * A high impact and a short flight time get a higher score.
 */
static int chooseTarget(Battleship b, EscortShip e[], int count,
                        double minAngle, double maxAngle,
                        double *hitTime)
{
    int i;
    int best = -1;
    double bestScore = -1.0;

    for (i = 0; i < count; i++)
    {
        double time;
        double score;
        double currentImpact;

        if (e[i].destroyed)
            continue;

        if (!canBattleshipHitEscort(b, e[i],
                                    minAngle, maxAngle, &time))
            continue;

        currentImpact =
            calculateImpact(e[i].impactPower,
                            getEscortGamma(e, i),
                            e[i].shotsFired);

        score = currentImpact / (time + 1.0);

        if (score > bestScore)
        {
            bestScore = score;
            best = i;
            *hitTime = time;
        }
    }

    return best;
}

/*
 * Process E firing events up to currentTime.
 * E ships can fire continuously in Part 2-C.
 */
static int processEscortShots(Battleship *b,
                              EscortShip e[],
                              int count,
                              double intervals[],
                              double nextFire[],
                              double currentTime,
                              double *damage,
                              FILE *file)
{
    int i;

    for (i = 0; i < count; i++)
    {
        double interval;
        double impact;
        double hitTime;

        if (e[i].destroyed)
            continue;

        switch (e[i].notation)
        {
            case 'A': interval = intervals[0]; break;
            case 'B': interval = intervals[1]; break;
            case 'C': interval = intervals[2]; break;
            case 'D': interval = intervals[3]; break;
            default:  interval = intervals[4]; break;
        }

        while (nextFire[i] <= currentTime + 0.000001)
        {
            if (canEscortHitBattleship(*b, e[i], &hitTime))
            {
                e[i].shotsFired++;

                impact = calculateImpact(
                    e[i].impactPower,
                    e[i].gamma,
                    e[i].shotsFired);

                e[i].currentImpact = impact;
                *damage += impact;

                fprintf(file,
                        "E%d fired at %.2f s | IP: %.4f | Damage: %.2f%%\n",
                        e[i].index,
                        nextFire[i],
                        impact,
                        impact * 100.0);

                fprintf(file,
                        "Time to B impact: %.2f s\n",
                        nextFire[i] + hitTime);

                if (*damage >= 1.0)
                    return e[i].index;
            }

            nextFire[i] += interval;
        }
    }

    return -1;
}

/*
 * Run one Part 2-C simulation.
 */
static void runSimulation(Battleship originalB,
                          EscortShip originalE[],
                          int count,
                          Position path[],
                          int k,
                          double bInterval,
                          double eIntervals[],
                          int usePath,
                          int jammed,
                          int jamAfter,
                          double jamAngle,
                          const char *fileName,
                          const char *title)
{
    Battleship b;
    EscortShip e[MAX_ESCORTS];
    double nextFire[MAX_ESCORTS];
    double currentTime = 0.0;
    double bDamage = 0.0;
    double hitTime;
    int attackOrder[MAX_ESCORTS];
    int attackCount = 0;
    int destroyed = 0;
    int i;
    int target;
    int sinker;
    FILE *file;

    b = originalB;
    copyShips(e, originalE, count);

    for (i = 0; i < count; i++)
        nextFire[i] = 0.0;

    b.shotsFired = 0;
    b.currentImpact = 1.0;
    bDamage = 0.0;

    for (i = 0; i < count; i++)
    {
        e[i].shotsFired = 0;
        e[i].damageTaken = 0.0;
        e[i].currentImpact = e[i].impactPower;
    }

    file = fopen(fileName, "w");

    if (file == NULL)
    {
        printf("Could not create %s\n", fileName);
        return;
    }

    fprintf(file, "NAVAL BATTLE SIMULATOR\n");
    fprintf(file, "%s\n\n", title);
    fprintf(file, "B firing interval: %.2f seconds\n", bInterval);
    fprintf(file, "B gamma: %.5f\n", b.gamma);
    fprintf(file, "E firing intervals:\n");
    fprintf(file, "E_A: %.2f\n", eIntervals[0]);
    fprintf(file, "E_B: %.2f\n", eIntervals[1]);
    fprintf(file, "E_C: %.2f\n", eIntervals[2]);
    fprintf(file, "E_D: %.2f\n", eIntervals[3]);
    fprintf(file, "E_E: %.2f\n\n", eIntervals[4]);

    for (i = 0; i < k; i++)
    {
        double minAngle = 0.0;
        double maxAngle = 90.0;

        if (usePath)
            b.position = path[i];

        if (jammed && i >= jamAfter)
            minAngle = jamAngle;

        fprintf(file, "\nITERATION %d\n", i + 1);
        fprintf(file, "B position: (%.2f, %.2f)\n",
                b.position.x, b.position.y);
        fprintf(file, "B angle range: %.2f - %.2f\n",
                minAngle, maxAngle);

        /* E ships fire before B's next shot. */
        sinker = processEscortShots(&b, e, count,
                                    eIntervals, nextFire,
                                    currentTime, &bDamage, file);

        if (sinker != -1)
        {
            b.destroyed = 1;
            break;
        }

        target = chooseTarget(b, e, count,
                              minAngle, maxAngle,
                              &hitTime);

        if (target != -1)
        {
            /* Wait for B's gun between consecutive firings. */
            if (b.shotsFired > 0)
                currentTime += bInterval;

            currentTime += hitTime;

            b.shotsFired++;
            b.currentImpact = calculateImpact(
                1.0, b.gamma, b.shotsFired);

            /*
             * B damage to E is no longer always 100%.
             */
            e[target].damageTaken += b.currentImpact;

            fprintf(file,
                    "B attack order %d: E%d E_%c\n",
                    attackCount + 1,
                    e[target].index,
                    e[target].notation);

            fprintf(file,
                    "B shots fired: %d | B current impact: %.4f\n",
                    b.shotsFired,
                    b.currentImpact);

            fprintf(file,
                    "Damage on E%d: %.2f%%\n",
                    e[target].index,
                    b.currentImpact * 100.0);

            if (e[target].damageTaken >= 1.0)
            {
                e[target].destroyed = 1;
                destroyed++;
                attackOrder[attackCount++] = e[target].index;

                fprintf(file,
                        "E%d DESTROYED\n",
                        e[target].index);
            }
            else
            {
                /*
                 * B may need another attack on the same E later.
                 * Keep it in the battlefield until damage reaches 100%.
                 */
                attackOrder[attackCount++] = e[target].index;

                fprintf(file,
                        "E%d survives with %.2f%% damage\n",
                        e[target].index,
                        e[target].damageTaken * 100.0);
            }
        }
        else
        {
            fprintf(file, "No E ship in B attack range.\n");
        }

        /* E ships may fire again after B's shot. */
        sinker = processEscortShots(&b, e, count,
                                    eIntervals, nextFire,
                                    currentTime, &bDamage, file);

        if (sinker != -1)
        {
            b.destroyed = 1;
            break;
        }

        if (bDamage >= 1.0)
        {
            b.destroyed = 1;
            break;
        }
    }

    fprintf(file, "\nFINAL RESULT\n");
    fprintf(file, "B status: %s\n",
            b.destroyed ? "DESTROYED" : "ALIVE");
    fprintf(file, "B shots fired: %d\n", b.shotsFired);
    fprintf(file, "B current impact power: %.4f\n", b.currentImpact);
    fprintf(file, "Cumulative impact on B: %.2f%%\n",
            bDamage * 100.0);
    fprintf(file, "E ships destroyed by B: %d\n", destroyed);
    fprintf(file, "Attack order:\n");

    if (attackCount == 0)
        fprintf(file, "None\n");
    else
    {
        for (i = 0; i < attackCount; i++)
            fprintf(file, "%d. E%d\n",
                    i + 1, attackOrder[i]);
    }

    fprintf(file, "\nCURRENT IMPACT POWER\n");
    fprintf(file, "B: %.4f\n", b.currentImpact);

    for (i = 0; i < count; i++)
    {
        fprintf(file,
                "E%d E_%c | Gamma: %.5f | Shots: %d | Current IP: %.4f | Damage taken: %.2f%% | %s\n",
                e[i].index,
                e[i].notation,
                e[i].gamma,
                e[i].shotsFired,
                e[i].currentImpact,
                e[i].damageTaken * 100.0,
                e[i].destroyed ? "DESTROYED" : "ALIVE");
    }

    fclose(file);

    printf("\n%s\n", title);
    printf("Battleship status : %s\n",
           b.destroyed ? "DESTROYED" : "ALIVE");
    printf("E ships destroyed : %d\n", destroyed);
    printf("B current IP      : %.4f\n", b.currentImpact);
    printf("B cumulative hit  : %.2f%%\n",
           bDamage * 100.0);
    printf("Attack order count: %d\n", attackCount);
    printf("Detailed results saved in:\n%s\n", fileName);
}

void runPart2C(Battleship battleship,
               EscortShip escorts[],
               int numberOfEscorts,
               double battlefieldSize)
{
    Position path[MAX_POINTS];
    double eIntervals[5];
    double gammaValues[5];
    double bInterval;
    double bGamma;
    double jamAngle;
    int k;
    int t;
    int i;

    printf("\n========== PART 2-C SETUP ==========\n");

    do
    {
        printf("Enter B firing interval T_Bq (seconds): ");
        scanf("%lf", &bInterval);
    }
    while (bInterval <= 0);

    do
    {
        printf("Enter B gamma (small positive value): ");
        scanf("%lf", &bGamma);
    }
    while (bGamma < 0);

    battleship.gamma = bGamma;

    printf("\nEnter firing interval for each escort type.\n");

    for (i = 0; i < 5; i++)
    {
        char type = 'A' + i;

        do
        {
            printf("T_E_%c (seconds): ", type);
            scanf("%lf", &eIntervals[i]);
        }
        while (eIntervals[i] <= 0);
    }

    printf("\nEnter gamma for each escort type.\n");

    for (i = 0; i < 5; i++)
    {
        char type = 'A' + i;

        do
        {
            printf("Gamma E_%c: ", type);
            scanf("%lf", &gammaValues[i]);
        }
        while (gammaValues[i] < 0);
    }

    setEscortGammas(escorts, numberOfEscorts, gammaValues);

    do
    {
        printf("\nEnter number of path points k (2-%d): ", MAX_POINTS);
        scanf("%d", &k);
    }
    while (k < 2 || k > MAX_POINTS);

    for (i = 0; i < k; i++)
    {
        path[i].x = ((double)rand() / RAND_MAX) * battlefieldSize;
        path[i].y = ((double)rand() / RAND_MAX) * battlefieldSize;
    }

    do
    {
        printf("Enter t (jam point, 1 to %d): ", k - 1);
        scanf("%d", &t);
    }
    while (t < 1 || t >= k);

    do
    {
        printf("Enter jammed minimum angle (0-30): ");
        scanf("%lf", &jamAngle);
    }
    while (jamAngle <= 0 || jamAngle >= 30);

    runSimulation(battleship, escorts, numberOfEscorts,
                  path, 1, bInterval, eIntervals,
                  0, 0, 0, 0.0,
                  resultPath("part2C_simulationA.txt"),
                  "PART 2-C - SIMULATION A (Part 1-C style)");

    runSimulation(battleship, escorts, numberOfEscorts,
                  path, k, bInterval, eIntervals,
                  1, 0, t, jamAngle,
                  resultPath("part2C_simulationB1.txt"),
                  "PART 2-C - SIMULATION B1 (Part 1-C path style)");

    runSimulation(battleship, escorts, numberOfEscorts,
                  path, k, bInterval, eIntervals,
                  1, 1, t, jamAngle,
                  resultPath("part2C_simulationB2.txt"),
                  "PART 2-C - SIMULATION B2 (jammed Part 1-C style)");

    printf("\nPart 2-C detailed results saved in the current results/run folder.\n");
}
