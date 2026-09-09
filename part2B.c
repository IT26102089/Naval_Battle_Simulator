#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "ships.h"
#include "battle.h"

/*
 * Part 2-B
 *
 * B still fires with a fixed time between shots.
 * E ships can now fire continuously.
 * Each E type has its own firing interval.
 */

static void copyShips(EscortShip to[], EscortShip from[], int count)
{
    int i;

    for (i = 0; i < count; i++)
        to[i] = from[i];
}

/*
 * Simple target strategy used by B.
 * Dangerous E ships are given a higher score.
 * A shorter flight time also gives a better score.
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

        if (e[i].destroyed)
            continue;

        if (!canBattleshipHitEscort(b, e[i],
                                    minAngle, maxAngle, &time))
            continue;

        score = e[i].impactPower / (time + 1.0);

        if (score > bestScore)
        {
            bestScore = score;
            best = i;
            *hitTime = time;
        }
    }

    return best;
}

/* Return the firing interval for an E type. */
static double getEscortInterval(EscortShip e[], int index,
                                double intervals[])
{
    switch (e[index].notation)
    {
        case 'A': return intervals[0];
        case 'B': return intervals[1];
        case 'C': return intervals[2];
        case 'D': return intervals[3];
        default:  return intervals[4];
    }
}

/*
 * Let E ships fire whenever their next firing time is reached.
 * In Part 2-B they are allowed to fire more than once.
 *
 * mode 0: one hit destroys B.
 * mode 1: impact is added to B.
 */
static int processEscortShots(Battleship *b, EscortShip e[], int count,
                               double intervals[], double currentTime,
                               double *damage, int mode, FILE *file,
                               double nextFire[])
{
    int i;
    int sinker = -1;

    for (i = 0; i < count; i++)
    {
        double hitTime;
        double interval;

        if (e[i].destroyed)
            continue;

        interval = getEscortInterval(e, i, intervals);

        /* E can fire at time 0 and then after its own interval. */
        while (nextFire[i] <= currentTime + 0.000001)
        {
            if (canEscortHitBattleship(*b, e[i], &hitTime))
            {
                e[i].shotsFired++;

                fprintf(file,
                        "E%d E_%c fired at %.2f seconds",
                        e[i].index, e[i].notation, nextFire[i]);

                fprintf(file,
                        " | Time to B impact: %.2f seconds\n",
                        nextFire[i] + hitTime);

                if (mode == 0)
                {
                    fprintf(file,
                            "E%d destroyed B.\n", e[i].index);
                    return e[i].index;
                }

                *damage += e[i].impactPower;

                fprintf(file,
                        "Impact: %.2f%%\n",
                        e[i].impactPower * 100.0);

                if (*damage >= 1.0)
                {
                    fprintf(file,
                            "E%d caused the final impact.\n",
                            e[i].index);
                    return e[i].index;
                }
            }

            nextFire[i] += interval;
        }
    }

    return sinker;
}

/*
 * Run one Part 2-B simulation.
 */
static void runSimulation(Battleship originalB, EscortShip originalE[],
                          int count, Position path[], int k,
                          double bInterval, double eIntervals[],
                          int usePath, int jammed, int jamAfter,
                          double jamAngle, int cumulative,
                          const char *fileName, const char *title)
{
    Battleship b;
    EscortShip e[MAX_ESCORTS];
    double nextFire[MAX_ESCORTS];
    double currentTime = 0.0;
    double totalDamage = 0.0;
    double hitTime;
    int attackOrder[MAX_ESCORTS];
    int attackCount = 0;
    int i;
    int target;
    int sinker = -1;
    int destroyed = 0;
    FILE *file;

    b = originalB;
    copyShips(e, originalE, count);

    for (i = 0; i < count; i++)
        nextFire[i] = 0.0;

    file = fopen(fileName, "w");

    if (file == NULL)
    {
        printf("Could not create %s\n", fileName);
        return;
    }

    fprintf(file, "NAVAL BATTLE SIMULATOR\n");
    fprintf(file, "%s\n\n", title);
    fprintf(file, "B firing interval: %.2f seconds\n", bInterval);
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

        /* E ships that are currently able to attack get their shots. */
        sinker = processEscortShots(&b, e, count, eIntervals,
                                    currentTime, &totalDamage,
                                    cumulative ? 1 : 0,
                                    file, nextFire);

        if (sinker != -1)
        {
            b.destroyed = 1;
            break;
        }

        target = chooseTarget(b, e, count,
                              minAngle, maxAngle, &hitTime);

        if (target != -1)
        {
            if (i > 0)
                currentTime += bInterval;

            currentTime += hitTime;

            e[target].destroyed = 1;
            b.shotsFired++;
            destroyed++;

            attackOrder[attackCount++] = e[target].index;

            fprintf(file,
                    "B attack order %d: E%d E_%c\n",
                    attackCount, e[target].index,
                    e[target].notation);
            fprintf(file,
                    "B time to hit: %.2f seconds\n",
                    hitTime);
        }
        else
        {
            fprintf(file, "No E ship in B attack range.\n");
        }

        /* E ships continue firing after B's shot. */
        sinker = processEscortShots(&b, e, count, eIntervals,
                                    currentTime, &totalDamage,
                                    cumulative ? 1 : 0,
                                    file, nextFire);

        if (sinker != -1)
        {
            b.destroyed = 1;
            break;
        }
    }

    fprintf(file, "\nFINAL RESULT\n");
    fprintf(file, "B status: %s\n",
            b.destroyed ? "DESTROYED" : "ALIVE");
    fprintf(file, "E ships destroyed by B: %d\n", destroyed);
    fprintf(file, "B shots fired: %d\n", b.shotsFired);
    fprintf(file, "Attack order of E ships:\n");

    if (attackCount == 0)
        fprintf(file, "None\n");
    else
    {
        for (i = 0; i < attackCount; i++)
            fprintf(file, "%d. E%d\n",
                    i + 1, attackOrder[i]);
    }

    if (cumulative)
        fprintf(file, "Cumulative impact on B: %.2f%%\n",
                totalDamage * 100.0);

    fclose(file);

    /* Show only the main result in the terminal. */
    printf("\n%s\n", title);
    printf("Battleship status  : %s\n",
           b.destroyed ? "DESTROYED" : "ALIVE");
    printf("E ships destroyed  : %d\n", destroyed);
    printf("Attack order saved : %d E ships\n", attackCount);

    if (cumulative)
        printf("Cumulative impact  : %.2f%%\n",
               totalDamage * 100.0);

    printf("Detailed results saved in:\n%s\n", fileName);
}

void runPart2B(Battleship battleship, EscortShip escorts[],
               int numberOfEscorts, double battlefieldSize)
{
    Position path[MAX_POINTS];
    double eIntervals[5];
    double bInterval;
    double jamAngle;
    int k;
    int t;
    int i;

    printf("\n========== PART 2-B SETUP ==========\n");

    do
    {
        printf("Enter B firing interval T_Bq (seconds): ");
        scanf("%lf", &bInterval);
    }
    while (bInterval <= 0);

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
                  0, 0, 0, 0.0, 0,
                  resultPath("part2B_simulationA.txt"),
                  "PART 2-B - SIMULATION A");

    runSimulation(battleship, escorts, numberOfEscorts,
                  path, k, bInterval, eIntervals,
                  1, 0, t, jamAngle, 0,
                  resultPath("part2B_simulationB1.txt"),
                  "PART 2-B - SIMULATION B1");

    runSimulation(battleship, escorts, numberOfEscorts,
                  path, k, bInterval, eIntervals,
                  1, 1, t, jamAngle, 0,
                  resultPath("part2B_simulationB2.txt"),
                  "PART 2-B - SIMULATION B2");

    runSimulation(battleship, escorts, numberOfEscorts,
                  path, 1, bInterval, eIntervals,
                  0, 0, 0, 0.0, 1,
                  resultPath("part2B_simulationC.txt"),
                  "PART 2-B - SIMULATION C");

    printf("\nPart 2-B detailed results saved in the current results/run folder.\n");
}
