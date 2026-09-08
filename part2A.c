#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "ships.h"
#include "battle.h"

/*
 * Part 2-A is built on the Part 1 functions.
 * The original Part 1 files are not changed.
 */

static void copyShips(EscortShip to[], EscortShip from[], int count)
{
    int i;

    for (i = 0; i < count; i++)
        to[i] = from[i];
}

/*
 * Simple strategy:
 * choose an E ship which gives a good amount of damage
 * and can be hit quickly by B.
 *
 * A higher impact and a shorter flight time give a higher score.
 */
static int chooseTarget(Battleship b, EscortShip e[], int count,
                        double minAngle, double maxAngle,
                        double *chosenTime)
{
    int i;
    int best = -1;
    double bestScore = -1.0;
    double hitTime;

    for (i = 0; i < count; i++)
    {
        double score;

        if (e[i].destroyed)
            continue;

        if (!canBattleshipHitEscort(b, e[i],
                                    minAngle, maxAngle,
                                    &hitTime))
            continue;

        /*
         * This is only a simple student-made strategy.
         * It prefers dangerous E ships and quick shots.
         */
        score = e[i].impactPower / (hitTime + 1.0);

        if (score > bestScore)
        {
            bestScore = score;
            best = i;
            *chosenTime = hitTime;
        }
    }

    return best;
}

/*
 * Let the surviving E ships attack B once.
 *
 * mode = 0 -> Part 1-A style: first successful hit sinks B.
 * mode = 1 -> Part 1-C style: damage is accumulated.
 */
static int escortAttack(Battleship *b, EscortShip e[], int count,
                        int mode, double *damage,
                        FILE *file, double currentTime)
{
    int i;
    int sinker = -1;

    for (i = 0; i < count; i++)
    {
        double hitTime;

        if (e[i].destroyed)
            continue;

        if (canEscortHitBattleship(*b, e[i], &hitTime))
        {
            e[i].shotsFired++;

            if (mode == 0)
            {
                sinker = e[i].index;

                fprintf(file,
                        "E%d can hit B at %.2f seconds. B is destroyed.\n",
                        e[i].index, currentTime + hitTime);
                return sinker;
            }

            *damage += e[i].impactPower;

            fprintf(file,
                    "E%d hit B: %.2f%% impact\n",
                    e[i].index, e[i].impactPower * 100.0);

            if (*damage >= 1.0)
            {
                sinker = e[i].index;
                return sinker;
            }
        }
    }

    return sinker;
}

/*
 * Run one timed simulation.
 *
 * usePath = 0 -> Part 1-A style starting position.
 * usePath = 1 -> Part 1-B/C path simulation.
 * jammed = 1 -> apply the Part 1-B simulation 2 angle restriction.
 */
static void runTimedSimulation(Battleship originalB,
                               EscortShip originalE[],
                               int count,
                               Position path[], int k,
                               double fireInterval,
                               int usePath,
                               int jammed,
                               int jamAfter,
                               double jamMinAngle,
                               int cumulative,
                               const char *fileName,
                               const char *title)
{
    Battleship b;
    EscortShip e[MAX_ESCORTS];
    FILE *file;

    int i;
    int target;
    int sinker;
    int destroyed = 0;
    double currentTime = 0.0;
    double totalDamage = 0.0;
    double hitTime = 0.0;

    b = originalB;
    copyShips(e, originalE, count);

    file = fopen(fileName, "w");

    if (file == NULL)
    {
        printf("Could not create %s\n", fileName);
        return;
    }

    fprintf(file, "NAVAL BATTLE SIMULATOR\n");
    fprintf(file, "%s\n", title);
    fprintf(file, "B firing interval: %.2f seconds\n\n", fireInterval);

    printf("\n%s\n", title);
    printf("B firing interval: %.2f seconds\n", fireInterval);

    for (i = 0; i < k; i++)
    {
        double minAngle = 0.0;
        double maxAngle = 90.0;

        if (!b.destroyed && usePath)
            b.position = path[i];

        if (jammed && i >= jamAfter)
            minAngle = jamMinAngle;

        fprintf(file, "\n------------------------------\n");
        fprintf(file, "ITERATION %d\n", i + 1);
        fprintf(file, "B position: (%.2f, %.2f)\n",
                b.position.x, b.position.y);
        fprintf(file, "Attack angle: %.2f - %.2f degrees\n",
                minAngle, maxAngle);

        printf("\nIteration %d\n", i + 1);
        printf("B position: (%.2f, %.2f)\n",
               b.position.x, b.position.y);

        if (b.destroyed)
            break;

        /*
         * B has to wait between consecutive shots.
         */
        if (i > 0)
            currentTime += fireInterval;

        target = chooseTarget(b, e, count,
                              minAngle, maxAngle,
                              &hitTime);

        if (target == -1)
        {
            fprintf(file, "No E ship is in B attack range.\n");
            printf("No E ship is in B attack range.\n");
        }
        else
        {
            currentTime += hitTime;
            e[target].destroyed = 1;
            destroyed++;
            b.shotsFired++;

            fprintf(file,
                    "B attack order position %d: E%d E_%c\n",
                    destroyed, e[target].index, e[target].notation);
            fprintf(file,
                    "Flight time: %.2f seconds\n",
                    hitTime);

            printf("B attacks E%d E_%c\n",
                   e[target].index, e[target].notation);
            printf("Flight time: %.2f seconds\n", hitTime);
        }

        /*
         * E ships get a chance to attack between B shots.
         */
        sinker = escortAttack(&b, e, count,
                              cumulative,
                              &totalDamage,
                              file, currentTime);

        if (sinker != -1)
        {
            b.destroyed = 1;

            fprintf(file,
                    "B destroyed by E%d.\n",
                    sinker);
            printf("B destroyed by E%d.\n", sinker);
            break;
        }

        if (cumulative)
        {
            fprintf(file,
                    "Cumulative B impact: %.2f%%\n",
                    totalDamage * 100.0);
            printf("Cumulative B impact: %.2f%%\n",
                   totalDamage * 100.0);
        }
    }

    fprintf(file, "\nFINAL RESULT\n");
    fprintf(file, "B status: %s\n",
            b.destroyed ? "DESTROYED" : "ALIVE");
    fprintf(file, "E ships destroyed by B: %d\n", destroyed);
    fprintf(file, "B shots fired: %d\n", b.shotsFired);
    fprintf(file, "Total time: %.2f seconds\n", currentTime);

    if (cumulative)
        fprintf(file, "Cumulative B impact: %.2f%%\n",
                totalDamage * 100.0);

    printf("\nFinal result: B %s\n",
           b.destroyed ? "DESTROYED" : "ALIVE");
    printf("E ships destroyed by B: %d\n", destroyed);
    printf("Total time: %.2f seconds\n", currentTime);

    fclose(file);
}

void runPart2A(Battleship battleship,
               EscortShip escorts[],
               int numberOfEscorts,
               double battlefieldSize)
{
    Position path[MAX_POINTS];
    int k;
    int t;
    double jamAngle;
    double fireInterval;
    int i;

    printf("\n========== PART 2-A SETUP ==========\n");

    do
    {
        printf("Enter B firing interval T_Bq (seconds): ");
        scanf("%lf", &fireInterval);

        if (fireInterval <= 0)
            printf("Firing interval must be greater than 0.\n");
    }
    while (fireInterval <= 0);

    do
    {
        printf("Enter number of path points k (2-%d): ", MAX_POINTS);
        scanf("%d", &k);

        if (k < 2 || k > MAX_POINTS)
            printf("Please enter a value between 2 and %d.\n",
                   MAX_POINTS);
    }
    while (k < 2 || k > MAX_POINTS);

    /* Generate the same kind of random path used in Part 1-B. */
    for (i = 0; i < k; i++)
    {
        path[i].x = ((double)rand() / RAND_MAX) * battlefieldSize;
        path[i].y = ((double)rand() / RAND_MAX) * battlefieldSize;
    }

    do
    {
        printf("Enter t (1 to %d): ", k - 1);
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

    printf("\nSimple strategy: higher E impact and shorter B flight time are preferred.\n");

    /*
     * Part 2-A simulation based on Part 1-A.
     */
    runTimedSimulation(battleship, escorts, numberOfEscorts,
                       path, 1, fireInterval,
                       0, 0, 0, 0.0, 0,
                       "results/part2A_simulationA.txt",
                       "PART 2-A - SIMULATION A (Part 1-A style)");

    /*
     * Part 2-A simulation based on Part 1-B.
     */
    runTimedSimulation(battleship, escorts, numberOfEscorts,
                       path, k, fireInterval,
                       1, 0, t, jamAngle, 0,
                       "results/part2A_simulationB1.txt",
                       "PART 2-A - SIMULATION B1 (Part 1-B style)");

    runTimedSimulation(battleship, escorts, numberOfEscorts,
                       path, k, fireInterval,
                       1, 1, t, jamAngle, 0,
                       "results/part2A_simulationB2.txt",
                       "PART 2-A - SIMULATION B2 (jammed Part 1-B style)");

    /*
     * Part 2-A simulation based on Part 1-C.
     */
    runTimedSimulation(battleship, escorts, numberOfEscorts,
                       path, 1, fireInterval,
                       0, 0, 0, 0.0, 1,
                       "results/part2A_simulationC.txt",
                       "PART 2-A - SIMULATION C (Part 1-C style)");

    printf("\nPart 2-A result files were saved in results/.\n");
}
