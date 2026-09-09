#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#ifndef _WIN32
#include <dirent.h>
#endif

#include "ships.h"
#include "battle.h"
#include "part2A.h"
#include "part2B.h"
#include "part2C.h"
#include "part2Settings.h"



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
 * Find the earliest escort that can hit B.
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


            if (cumulativeImpact >= 1.0)
            {
                b.destroyed = 1;

                sinkingEscort =
                    e[i].index;

                printf("\nB HAS BEEN SUNK!\n");
                printf("Cumulative impact: %.2f%%\n",
                       cumulativeImpact * 100.0);

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
                resultPath("part1C_simulationA.txt"),
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

            if (b.destroyed)
                fprintf(file,
                        "E%d sank B\n",
                        sinkingEscort);

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
                    ? resultPath("part1C_simulationB2.txt")
                    : resultPath("part1C_simulationB1.txt"),
                "w");
    }
    else
    {
        file =
            fopen(
                jammed
                    ? resultPath("part1B_simulation2.txt")
                    : resultPath("part1B_simulation1.txt"),
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
                    "Time to impact: %.2f seconds\n",
                    battleTime);

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

    if (b.destroyed)
        fprintf(file,
                "E%d sank B\n",
                sinkingEscort);

    fprintf(file, "Final escort states:\n");
    for (i = 0; i < numberOfEscorts; i++)
        fprintf(file, "E%d E_%c - %s\n",
                e[i].index, e[i].notation,
                e[i].destroyed ? "DESTROYED" : "ALIVE");

    if (partCMode)
    {
        fprintf(file,
                "Cumulative impact: %.2f%%\n",
                cumulativeImpact * 100.0);
    }

    printf("\nFINAL RESULT: B %s\n", b.destroyed ? "DESTROYED" : "ALIVE");
    if (partCMode)
        printf("Cumulative impact on B: %.2f%%\n", cumulativeImpact * 100.0);

    printf("Detailed results saved in: %s\n",
           resultPath(partCMode
                       ? (jammed ? "part1C_simulationB2.txt" : "part1C_simulationB1.txt")
                       : (jammed ? "part1B_simulation2.txt" : "part1B_simulation1.txt")));

    fclose(file);
}


static void showInstructions(void)
{
    printf("\n=============== INSTRUCTIONS ===============\n");
    printf("1. Choose a simulation option from the main menu.\n");
    printf("2. Enter the required battleship, escort and simulation values.\n");
    printf("3. Part 1-A simulates a direct attack by B and E ships.\n");
    printf("4. Part 1-B adds B movement and the jamming angle restriction.\n");
    printf("5. Part 1-C uses cumulative impact from surviving E ships.\n");
    printf("6. Part 2-A adds a fixed time between B gun firings.\n");
    printf("7. Part 2-B allows E ships to fire continuously.\n");
    printf("8. Part 2-C adds gamma-based impact degradation.\n");
    printf("9. Detailed simulation results are saved as text files.\n");
    printf("10. Each simulation session is stored in a new results/run_XXX folder.\n");
    printf("\nInput help:\n");
    printf("  k = number of points in the B movement path.\n");
    printf("  t = iteration at which B jamming starts.\n");
    printf("  T_Bq = seconds between consecutive B gun firings.\n");
    printf("  T_Ep = seconds between consecutive E gun firings.\n");
    printf("  gamma = rate used to reduce impact power after firing.\n");
    printf("=============================================\n");
    printf("Press Enter to return to the main menu...");
    getchar();
    getchar();
}

static void showSimulationStatistics(void)
{
#ifdef _WIN32
    printf("\nSimulation Statistics is available in Ubuntu/Linux builds.\n");
    printf("Open the results folder to view saved run files.\n");
    printf("Press Enter to return to the main menu...");
    getchar();
    getchar();
#else
    DIR *dir;
    struct dirent *entry;
    char runs[100][64];
    int count = 0;
    int i;
    int selected;
    char path[256];
    char line[512];
    FILE *file;

    dir = opendir("results");

    printf("\n========== SIMULATION STATISTICS ==========\n");

    if (dir == NULL)
    {
        printf("No previous simulation results were found.\n");
        printf("===========================================\n");
        printf("Press Enter to return to the main menu...");
        getchar();
        getchar();
        return;
    }

    while ((entry = readdir(dir)) != NULL && count < 100)
    {
        int number;

        if (sscanf(entry->d_name, "run_%d", &number) == 1)
        {
            strcpy(runs[count], entry->d_name);
            count++;
        }
    }

    closedir(dir);

    if (count == 0)
    {
        printf("No previous simulation results were found.\n");
        printf("===========================================\n");
        printf("Press Enter to return to the main menu...");
        getchar();
        getchar();
        return;
    }

    printf("Previous simulation runs:\n");
    for (i = 0; i < count; i++)
        printf("%d. %s\n", i + 1, runs[i]);

    printf("\nSelect a run (1-%d): ", count);
    scanf("%d", &selected);

    if (selected < 1 || selected > count)
    {
        printf("Invalid run number.\n");
        printf("Press Enter to return to the main menu...");
        getchar();
        getchar();
        return;
    }

    snprintf(path, sizeof(path), "results/%s", runs[selected - 1]);
    dir = opendir(path);

    if (dir == NULL)
    {
        printf("Could not open %s.\n", path);
        printf("Press Enter to return to the main menu...");
        getchar();
        getchar();
        return;
    }

    printf("\n========== %s ==========\n", runs[selected - 1]);

    while ((entry = readdir(dir)) != NULL)
    {
        if (strstr(entry->d_name, ".txt") == NULL)
            continue;

        snprintf(line, sizeof(line), "%s/%s", path, entry->d_name);
        file = fopen(line, "r");

        if (file == NULL)
            continue;

        printf("\n--- %s ---\n", entry->d_name);

        while (fgets(line, sizeof(line), file) != NULL)
            printf("%s", line);

        fclose(file);
    }

    closedir(dir);

    printf("\n===========================================\n");
    printf("Press Enter to return to the main menu...");
    getchar();
    getchar();
#endif
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

    int k;
    int t;
    double jamAngle;

    printf("\n============================================\n");
    printf("       ADVANCED NAVAL BATTLE SIMULATOR\n");
    printf("============================================\n");

    while (1)
    {
        printf("\n=============== MAIN MENU ==================\n");
        printf("1. Run Part 1-A\n");
        printf("2. Run Part 1-B\n");
        printf("3. Run Part 1-C\n");
        printf("4. Run ALL Part 1 (A + B + C)\n");
        printf("5. Run Part 2-A\n");
        printf("6. Run Part 2-B\n");
        printf("7. Run Part 2-C\n");
        printf("8. Run FULL PROGRAM (Part 1 + Part 2)\n");
        printf("9. View Instructions\n");
        printf("10. Simulation Statistics\n");
        printf("11. Exit\n");
        printf("=============================================\n");
        printf("Select an option: ");

        if (scanf("%d", &choice) != 1)
        {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF) {}
            printf("Invalid input. Please enter a menu number.\n");
            continue;
        }

        if (choice == 9)
        {
            showInstructions();
            continue;
        }

        if (choice == 10)
        {
            showSimulationStatistics();
            continue;
        }

        if (choice == 11)
        {
            char answer;

            printf("\nAre you sure you want to exit? (Y/N): ");
            scanf(" %c", &answer);

            if (answer == 'Y' || answer == 'y')
            {
                printf("\nThank you for using Advanced Naval Battle Simulator.\n");
                printf("Exiting...\n");
                break;
            }

            printf("Returning to main menu...\n");
            continue;
        }

        if (choice < 1 || choice > 8)
        {
            printf("Invalid option. Please select 1-11.\n");
            continue;
        }

        /* A new run folder is created only for a simulation. */
        startNewResultRun();
        setPart2FullProgram(choice == 8);

        printf("\nEnter random seed (used to create repeatable random positions): ");
        scanf("%u", &seed);
        srand(seed);

        printf("\nEnter battlefield size D (length/width of the square battlefield): ");
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
            printf("Enter Battleship Y coordinate (0 - %.2f): ", battlefieldSize);
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

        /* Save common setup values for this session. */
        saveSessionSettingsBase(seed, battlefieldSize, battleship,
                                escorts, numberOfEscorts);

        displayBattlefield(battlefieldSize, battleship,
                           escorts, numberOfEscorts);

        /* Part 1-A */
        if (choice == 1 || choice == 4 || choice == 8)
        {
            printf("\n============================================\n");
            printf("               PART 1-A\n");
            printf("============================================\n");

            savePart1AInitial(battleship, escorts,
                              numberOfEscorts, battlefieldSize, seed);
            runPart1A(battleship, escorts, numberOfEscorts, 0);
            printf("Detailed results saved in:\n");
            printf("  %s\n", resultPath("part1A_initial.txt"));
            printf("  %s\n", resultPath("part1A_final.txt"));
        }

        /* Part 1-B needs k, t and jam angle. */
        if (choice == 2 || choice == 4 || choice == 8)
        {
            printf("\n============================================\n");
            printf("               PART 1-B\n");
            printf("============================================\n");

            do
            {
                printf("\nEnter k (number of points in the B movement path, max %d): ", MAX_POINTS);
                scanf("%d", &k);
            }
            while (k <= 0 || k > MAX_POINTS);

            generatePath(path, k, battlefieldSize);

            do
            {
                printf("Enter t (iteration when jamming starts, 1 to %d): ", k - 1);
                scanf("%d", &t);
            }
            while (t < 1 || t >= k);

            do
            {
                printf("Enter minimum firing angle after jamming (0-30 degrees): ");
                scanf("%lf", &jamAngle);
            }
            while (jamAngle <= 0 || jamAngle >= 30);

            /* Save these values for Part 2 in full-program mode. */
            savePart2PathSettings(k, t, jamAngle);

            {
                char settings[200];
                snprintf(settings, sizeof(settings),
                         "Part 1-B: k = %d, jam starts at t = %d, minimum firing angle = %.2f degrees",
                         k, t, jamAngle);
                appendSessionSetting(settings);
            }

            printf("\nSIMULATION 1\n");
            runPart1B(battleship, escorts, numberOfEscorts,
                      path, k, t, jamAngle, 0, 0);

            printf("\nSIMULATION 2\n");
            runPart1B(battleship, escorts, numberOfEscorts,
                      path, k, t, jamAngle, 0, 1);
        }

        /* Part 1-C includes its own A, B1 and B2 simulations. */
        if (choice == 3 || choice == 4 || choice == 8)
        {
            printf("\n============================================\n");
            printf("               PART 1-C\n");
            printf("============================================\n");

            if (choice == 3)
            {
                do
                {
                    printf("\nEnter k (number of points in the B movement path, max %d): ", MAX_POINTS);
                    scanf("%d", &k);
                }
                while (k <= 0 || k > MAX_POINTS);

                generatePath(path, k, battlefieldSize);

                do
                {
                    printf("Enter t (iteration when jamming starts, 1 to %d): ", k - 1);
                    scanf("%d", &t);
                }
                while (t < 1 || t >= k);

                do
                {
                    printf("Enter minimum firing angle after jamming (0-30 degrees): ");
                    scanf("%lf", &jamAngle);
                }
                while (jamAngle <= 0 || jamAngle >= 30);

                {
                    char settings[200];
                    snprintf(settings, sizeof(settings),
                             "Part 1-C: k = %d, jam starts at t = %d, minimum firing angle = %.2f degrees",
                             k, t, jamAngle);
                    appendSessionSetting(settings);
                }
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

        /* Part 2-A asks for its own values. In Full Program these are reused. */
        if (choice == 5 || choice == 8)
        {
            printf("\n============================================\n");
            printf("               PART 2-A\n");
            printf("============================================\n");

            runPart2A(battleship, escorts, numberOfEscorts, battlefieldSize);
        }

        if (choice == 6 || choice == 8)
        {
            printf("\n============================================\n");
            printf("               PART 2-B\n");
            printf("============================================\n");

            runPart2B(battleship, escorts, numberOfEscorts, battlefieldSize);
        }

        if (choice == 7 || choice == 8)
        {
            printf("\n============================================\n");
            printf("               PART 2-C\n");
            printf("============================================\n");

            runPart2C(battleship, escorts, numberOfEscorts, battlefieldSize);
        }

        printf("\n============================================\n");
        printf("              SIMULATION COMPLETE\n");
        printf("============================================\n");
        printf("Detailed results are saved in: %s\n", resultPath(""));
        printf("Returning to main menu...\n");
    }

    return 0;
}
