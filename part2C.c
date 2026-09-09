#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "ships.h"
#include "battle.h"
#include "part2C.h"
#include "part2Settings.h"

#define MAX_E_TYPES 5
#define MAX_SIM_TIME 100000.0

static void copyShips(EscortShip to[], EscortShip from[], int count)
{
    int i;
    for (i = 0; i < count; i++)
        to[i] = from[i];
}

/* Impact power after n shots. */
static double impactAfterShots(double initial, double gamma, int shots)
{
    return initial * exp(-gamma * shots);
}

/* Simple strategy: attack the E with the highest current impact/time score. */
static int chooseTarget(Battleship b, EscortShip e[], int count,
                        double minAngle, double maxAngle,
                        double *hitTime)
{
    int i, best = -1;
    double bestScore = -1.0;

    for (i = 0; i < count; i++)
    {
        double time, score;

        if (e[i].destroyed)
            continue;

        if (!canBattleshipHitEscort(b, e[i], minAngle, maxAngle, &time))
            continue;

        score = e[i].currentImpact / (time + 1.0);

        if (score > bestScore)
        {
            bestScore = score;
            best = i;
            *hitTime = time;
        }
    }

    return best;
}

static double escortInterval(EscortShip e, double intervals[])
{
    int type = e.notation - 'A';
    if (type < 0 || type >= MAX_E_TYPES)
        type = 0;
    return intervals[type];
}

/* Process E shots that are due before the next B shot. */
static int processEscortShots(Battleship *b, EscortShip e[], int count,
                              double intervals[], double eGamma[], double *nextFire,
                              double untilTime, double *damage, int cumulative,
                              FILE *file)
{
    int i;
    double nextEvent;
    int sinker = -1;

    while (1)
    {
        nextEvent = untilTime + 1.0;

        for (i = 0; i < count; i++)
        {
            if (!e[i].destroyed && nextFire[i] < nextEvent)
                nextEvent = nextFire[i];
        }

        if (nextEvent > untilTime)
            break;

        for (i = 0; i < count; i++)
        {
            double hitTime;

            if (e[i].destroyed || nextFire[i] > nextEvent + 0.000001)
                continue;

            if (canEscortHitBattleship(*b, e[i], &hitTime))
            {
                e[i].shotsFired++;
                e[i].currentImpact = impactAfterShots(
                    e[i].impactPower, eGamma[e[i].notation - 'A'],
                    e[i].shotsFired);

                if (!cumulative)
                {
                    sinker = e[i].index;
                    fprintf(file, "E%d fired and destroyed B at %.2f seconds.\n",
                            e[i].index, nextEvent + hitTime);
                    return sinker;
                }

                *damage += e[i].currentImpact;
                fprintf(file, "E%d fired at %.2f s, current impact %.4f, B damage %.4f.\n",
                        e[i].index, nextEvent, e[i].currentImpact,
                        e[i].currentImpact);

                if (*damage >= 1.0)
                {
                    sinker = e[i].index;
                    return sinker;
                }
            }

            nextFire[i] += escortInterval(e[i], intervals);
        }
    }

    return sinker;
}

static void runSimulation(Battleship originalB, EscortShip originalE[], int count,
                          Position path[], int k, double bInterval,
                          double eIntervals[], double eGamma[], int jammed, int jamAfter,
                          double jamAngle, int cumulative, const char *fileName,
                          const char *title)
{
    Battleship b = originalB;
    EscortShip e[MAX_ESCORTS];
    double nextFire[MAX_ESCORTS];
    double currentTime = 0.0;
    double damage = 0.0;
    double remainingHealth[MAX_ESCORTS];
    double hitTime;
    int attackOrder[MAX_ESCORTS];
    int attackCount = 0;
    int i, target, sinker = -1;
    FILE *file = fopen(fileName, "w");

    copyShips(e, originalE, count);

    for (i = 0; i < count; i++)
        remainingHealth[i] = 1.0;

    if (file == NULL)
    {
        printf("Could not create %s\n", fileName);
        return;
    }

    for (i = 0; i < count; i++)
        nextFire[i] = 0.0;

    fprintf(file, "NAVAL BATTLE SIMULATOR\n%s\n\n", title);
    fprintf(file, "B firing interval: %.2f seconds\n", bInterval);
    fprintf(file, "B gamma: %.5f\n\n", b.gamma);

    for (i = 0; i < k && currentTime < MAX_SIM_TIME; i++)
    {
        double minAngle = 0.0;
        double maxAngle = 90.0;

        b.position = path[i];
        if (jammed && i >= jamAfter)
            minAngle = jamAngle;

        if (processEscortShots(&b, e, count, eIntervals, eGamma, nextFire,
                               currentTime, &damage, cumulative, file) != -1)
        {
            b.destroyed = 1;
            break;
        }

        target = chooseTarget(b, e, count, minAngle, maxAngle, &hitTime);
        if (target == -1)
            continue;

        if (attackCount > 0)
            currentTime += bInterval;

        currentTime += hitTime;
        b.shotsFired++;
        b.currentImpact = impactAfterShots(1.0, b.gamma, b.shotsFired);

        attackOrder[attackCount++] = e[target].index;
        fprintf(file, "B attack %d: E%d E_%c, hit time %.2f s, B impact %.4f\n",
                attackCount, e[target].index, e[target].notation,
                hitTime, b.currentImpact);

        /* B damage is applied to the E ship health. */
        remainingHealth[target] -= b.currentImpact;
        if (remainingHealth[target] <= 0.0)
        {
            e[target].destroyed = 1;
            remainingHealth[target] = 0.0;
        }

        sinker = processEscortShots(&b, e, count, eIntervals, eGamma, nextFire,
                                    currentTime, &damage, cumulative, file);
        if (sinker != -1)
        {
            b.destroyed = 1;
            break;
        }
    }

    fprintf(file, "\nFINAL RESULT\n");
    fprintf(file, "B status: %s\n", b.destroyed ? "DESTROYED" : "ALIVE");
    fprintf(file, "B shots fired: %d\n", b.shotsFired);
    fprintf(file, "B current impact power: %.6f\n", b.currentImpact);
    if (b.destroyed && sinker != -1)
        fprintf(file, "E%d sank B\n", sinker);
    fprintf(file, "Attack order:\n");

    for (i = 0; i < attackCount; i++)
        fprintf(file, "%d. E%d\n", i + 1, attackOrder[i]);

    fprintf(file, "Current E impact powers and health:\n");
    for (i = 0; i < count; i++)
        fprintf(file, "E%d E_%c: impact %.6f, remaining health %.6f\n",
                e[i].index, e[i].notation, e[i].currentImpact,
                remainingHealth[i]);

    fclose(file);

    printf("%s\n", title);
    printf("B status          : %s\n", b.destroyed ? "DESTROYED" : "ALIVE");
    printf("B shots fired     : %d\n", b.shotsFired);
    printf("B impact power    : %.4f\n", b.currentImpact);
    printf("E attack order    : %d ships\n", attackCount);
    printf("Detailed results  : %s\n", fileName);
}

void runPart2C(Battleship battleship, EscortShip escorts[],
               int numberOfEscorts, double battlefieldSize)
{
    Position path[MAX_POINTS];
    double eIntervals[MAX_E_TYPES];
    double bInterval, jamAngle;
    int k, t, i;
    double eGamma[MAX_E_TYPES];

    printf("\n========== PART 2-C SETUP ==========\n");

    if (isPart2FullProgram() && loadPart2BasicSettings(&bInterval, &k, &t, &jamAngle))
    {
        printf("Using T_Bq, k, t and jamming angle from Part 2-A.\n");
    }
    else
    {
        do
        {
            printf("Enter T_Bq (seconds between two consecutive B gun firings): ");
            scanf("%lf", &bInterval);
        } while (bInterval <= 0.0);

        do
        {
            printf("Enter k (number of points in the B movement path, 2-%d): ", MAX_POINTS);
            scanf("%d", &k);
        } while (k < 2 || k > MAX_POINTS);
    }

    do
    {
        printf("Enter Battleship gamma (small positive degradation rate): ");
        scanf("%lf", &battleship.gamma);
    } while (battleship.gamma <= 0.0);

    if (isPart2FullProgram() && loadPart2EscortIntervals(eIntervals, MAX_E_TYPES))
    {
        printf("Using escort firing intervals from Part 2-B.\n");
    }
    else
    {
        printf("\nT_E is the time between two consecutive gun firings of an escort ship type.\n");

        for (i = 0; i < MAX_E_TYPES; i++)
        {
            do
            {
                printf("Enter T_E_%c (seconds between E_%c firings): ", 'A' + i, 'A' + i);
                scanf("%lf", &eIntervals[i]);
            } while (eIntervals[i] <= 0.0);
        }
    }

    for (i = 0; i < MAX_E_TYPES; i++)
    {
        do
        {
            printf("Enter gamma for E_%c (impact power degradation rate): ", 'A' + i);
            scanf("%lf", &eGamma[i]);
        } while (eGamma[i] < 0.0);
    }

    for (i = 0; i < k; i++)
    {
        path[i].x = ((double)rand() / RAND_MAX) * battlefieldSize;
        path[i].y = ((double)rand() / RAND_MAX) * battlefieldSize;
    }

    if (!isPart2FullProgram())
    {
        do
        {
            printf("Enter t (iteration when jamming starts, 1 to %d): ", k - 1);
            scanf("%d", &t);
        } while (t < 1 || t >= k);

        do
        {
            printf("Enter minimum firing angle after jamming (0-30 degrees): ");
            scanf("%lf", &jamAngle);
        } while (jamAngle <= 0.0 || jamAngle >= 30.0);

        savePart2BasicSettings(bInterval, k, t, jamAngle);
    }

    /* Store the user supplied E gamma values in the working copies. */
    for (i = 0; i < numberOfEscorts; i++)
        escorts[i].currentImpact = escorts[i].impactPower;

    /* The same Part 1-A/B/C style simulations are used with degradation. */
    runSimulation(battleship, escorts, numberOfEscorts,
                   path, 1, bInterval, eIntervals, eGamma,
                   0, 0, 0.0, 1,
                   resultPath("part2C_simulationA.txt"),
                   "PART 2-C - SIMULATION A");

    runSimulation(battleship, escorts, numberOfEscorts,
                   path, k, bInterval, eIntervals, eGamma,
                   0, t, jamAngle, 1,
                   resultPath("part2C_simulationB1.txt"),
                   "PART 2-C - SIMULATION B1");

    runSimulation(battleship, escorts, numberOfEscorts,
                   path, k, bInterval, eIntervals, eGamma,
                   1, t, jamAngle, 1,
                   resultPath("part2C_simulationB2.txt"),
                   "PART 2-C - SIMULATION B2");

    runSimulation(battleship, escorts, numberOfEscorts,
                   path, 1, bInterval, eIntervals, eGamma,
                   0, 0, 0.0, 1,
                   resultPath("part2C_simulationC.txt"),
                   "PART 2-C - SIMULATION C");

    printf("Part 2-C detailed results saved in the current results/run folder.\n");
}
