#include <stdio.h>
#include <math.h>

#include "battle.h"

#define GRAVITY 9.81


double calculateDistance(Position a,
                         Position b)
{
    double dx;
    double dy;

    dx = b.x - a.x;
    dy = b.y - a.y;

    return sqrt(dx * dx + dy * dy);
}


/*
 * A/L projectile motion equation:
 *
 * R = u^2 sin(2theta) / g
 */
double calculateProjectileRange(double velocity,
                                double angleDegrees)
{
    double angleRadians;

    angleRadians =
        angleDegrees * acos(-1.0) / 180.0;

    return (velocity * velocity *
            sin(2.0 * angleRadians))
            / GRAVITY;
}


/*
 * Time required for the shell to reach
 * horizontal distance R.
 *
 * R = u cos(theta) * t
 *
 * Therefore:
 *
 * t = R / (u cos(theta))
 */
double calculateFlightTime(double distance,
                           double velocity,
                           double angleDegrees)
{
    double angleRadians;

    angleRadians =
        angleDegrees * acos(-1.0) / 180.0;

    return distance /
           (velocity * cos(angleRadians));
}


/*
 * Find a firing angle that can hit the target.
 *
 * For Part 1-A, B can fire from 0 to 90 degrees.
 *
 * We use the lower possible angle because
 * it gives a valid projectile path.
 */
int canBattleshipHitEscort(Battleship battleship,
                           EscortShip escort,
                           double *hitTime)
{
    double distance;
    double maximumRange;
    double value;
    double angleRadians;
    double angleDegrees;

    distance =
        calculateDistance(battleship.position,
                          escort.position);


    /*
     * Maximum range occurs at 45 degrees.
     */
    maximumRange =
        calculateProjectileRange(
            battleship.vmax,
            45.0);


    if (distance > maximumRange)
    {
        return 0;
    }


    /*
     * Solve:
     *
     * R = u^2 sin(2theta) / g
     *
     * sin(2theta) =
     * Rg / u^2
     */
    value =
        (distance * GRAVITY) /
        (battleship.vmax *
         battleship.vmax);


    /*
     * Numerical protection.
     */
    if (value > 1.0)
    {
        value = 1.0;
    }


    if (value < 0.0)
    {
        value = 0.0;
    }


    angleRadians =
        0.5 * asin(value);

    angleDegrees =
        angleRadians * 180.0 /
        acos(-1.0);


    /*
     * Calculate shell flight time.
     */
    if (distance == 0.0)
    {
        *hitTime = 0.0;
    }
    else
    {
        *hitTime =
            calculateFlightTime(
                distance,
                battleship.vmax,
                angleDegrees);
    }

    return 1;
}


/*
 * Determine whether an Escort can hit B.
 *
 * Escort has:
 *
 * theta_L = angleMin
 * theta_H = angleMin + angleRange
 *
 * We check the projectile range at the
 * allowed angles.
 */
int canEscortHitBattleship(Battleship battleship,
                           EscortShip escort,
                           double *hitTime)
{
    double distance;

    double thetaL;
    double thetaH;

    double rangeL;
    double rangeH;
    double range45;

    double minimumRange;
    double maximumRange;

    double value;

    double angleRadians;
    double angleDegrees;


    distance =
        calculateDistance(
            escort.position,
            battleship.position);


    thetaL = escort.angleMin;

    thetaH =
        escort.angleMin +
        escort.angleRange;


    /*
     * thetaH should never exceed 90.
     */
    if (thetaH > 90.0)
    {
        thetaH = 90.0;
    }


    rangeL =
        calculateProjectileRange(
            escort.vmax,
            thetaL);

    rangeH =
        calculateProjectileRange(
            escort.vmax,
            thetaH);


    /*
     * Normally the minimum range is one
     * of the two boundary angles.
     */
    if (rangeL < rangeH)
    {
        minimumRange = rangeL;
    }
    else
    {
        minimumRange = rangeH;
    }


    if (rangeL > rangeH)
    {
        maximumRange = rangeL;
    }
    else
    {
        maximumRange = rangeH;
    }


    /*
     * If 45 degrees is inside the allowed
     * angle range, it gives maximum range.
     */
    if (thetaL <= 45.0 &&
        thetaH >= 45.0)
    {
        range45 =
            calculateProjectileRange(
                escort.vmax,
                45.0);

        if (range45 > maximumRange)
        {
            maximumRange = range45;
        }
    }


    /*
     * Target must be inside the annular
     * attack range.
     */
    if (distance < minimumRange ||
        distance > maximumRange)
    {
        return 0;
    }


    /*
     * Find a possible angle.
     *
     * sin(2theta) = Rg/u^2
     */
    value =
        (distance * GRAVITY) /
        (escort.vmax *
         escort.vmax);


    if (value > 1.0)
    {
        value = 1.0;
    }

    if (value < 0.0)
    {
        value = 0.0;
    }


    angleRadians =
        0.5 * asin(value);

    angleDegrees =
        angleRadians * 180.0 /
        acos(-1.0);


    /*
     * If the lower angle is outside the
     * allowed range, use the complementary
     * projectile angle.
     */
    if (angleDegrees < thetaL ||
        angleDegrees > thetaH)
    {
        angleDegrees =
            90.0 - angleDegrees;
    }


    /*
     * Final angle validation.
     */
    if (angleDegrees < thetaL ||
        angleDegrees > thetaH)
    {
        return 0;
    }


    *hitTime =
        calculateFlightTime(
            distance,
            escort.vmax,
            angleDegrees);

    return 1;
}


/*
 * Simple visual representation of the
 * battlefield.
 */
void displayBattlefield(double battlefieldSize,
                        Battleship battleship,
                        EscortShip escorts[],
                        int numberOfEscorts)
{
    int rows = 10;
    int columns = 10;

    int row;
    int column;
    int i;

    int found;


    printf("\n");
    printf("============================================\n");
    printf("             NAVAL BATTLEFIELD\n");
    printf("============================================\n");


    for (row = rows - 1;
         row >= 0;
         row--)
    {
        printf("%4d |",
               (int)(battlefieldSize *
                     row / rows));


        for (column = 0;
             column < columns;
             column++)
        {
            double minX;
            double maxX;
            double minY;
            double maxY;


            minX =
                battlefieldSize *
                column / columns;

            maxX =
                battlefieldSize *
                (column + 1) / columns;

            minY =
                battlefieldSize *
                row / rows;

            maxY =
                battlefieldSize *
                (row + 1) / rows;


            found = 0;


            /*
             * Display B.
             */
            if (battleship.position.x >= minX &&
                battleship.position.x < maxX &&
                battleship.position.y >= minY &&
                battleship.position.y < maxY)
            {
                printf(" B ");
                found = 1;
            }


            /*
             * Display E.
             */
            if (!found)
            {
                for (i = 0;
                     i < numberOfEscorts;
                     i++)
                {
                    if (!escorts[i].destroyed &&
                        escorts[i].position.x >= minX &&
                        escorts[i].position.x < maxX &&
                        escorts[i].position.y >= minY &&
                        escorts[i].position.y < maxY)
                    {
                        printf(" E ");
                        found = 1;
                        break;
                    }
                }
            }


            if (!found)
            {
                printf(" . ");
            }
        }

        printf("\n");
    }

    printf("      +-------------------------------\n");

    printf("\nB = Battleship\n");
    printf("E = Escort Ship\n");
    printf(". = Empty area\n");
}


/*
 * Save all initial conditions.
 */
void saveInitialConditions(Battleship battleship,
                           EscortShip escorts[],
                           int numberOfEscorts,
                           double battlefieldSize)
{
    FILE *file;
    int i;


    file =
        fopen("results/initial_conditions.txt", "w");


    if (file == NULL)
    {
        printf("Could not create initial_conditions.txt\n");
        return;
    }


    fprintf(file,
            "NAVAL BATTLE SIMULATOR\n");
    fprintf(file,
            "PART 1-A - INITIAL CONDITIONS\n\n");


    fprintf(file,
            "Battlefield Size: %.2f\n\n",
            battlefieldSize);


    fprintf(file,
            "BATTLESHIP\n");
    fprintf(file,
            "Notation: %c\n",
            battleship.notation);

    fprintf(file,
            "Type: %s\n",
            battleship.name);

    fprintf(file,
            "Gun: %s\n",
            battleship.gunName);

    fprintf(file,
            "Position: (%.2f, %.2f)\n",
            battleship.position.x,
            battleship.position.y);

    fprintf(file,
            "Vmax: %.2f\n",
            battleship.vmax);

    fprintf(file,
            "Minimum Angle: %.2f\n",
            battleship.angleMin);

    fprintf(file,
            "Maximum Angle: %.2f\n\n",
            battleship.angleMax);


    fprintf(file,
            "ESCORT SHIPS\n");


    for (i = 0;
         i < numberOfEscorts;
         i++)
    {
        fprintf(file,
                "\nE%d\n",
                i + 1);

        fprintf(file,
                "Notation: E_%c\n",
                escorts[i].notation);

        fprintf(file,
                "Type: %s\n",
                escorts[i].name);

        fprintf(file,
                "Position: (%.2f, %.2f)\n",
                escorts[i].position.x,
                escorts[i].position.y);

        fprintf(file,
                "Vmin: %.2f\n",
                escorts[i].vmin);

        fprintf(file,
                "Vmax: %.2f\n",
                escorts[i].vmax);

        fprintf(file,
                "Minimum Angle: %.2f\n",
                escorts[i].angleMin);

        fprintf(file,
                "Angle Range: %.2f\n",
                escorts[i].angleRange);

        fprintf(file,
                "Maximum Angle: %.2f\n",
                escorts[i].angleMin +
                escorts[i].angleRange);

        fprintf(file,
                "Impact Power: %.2f\n",
                escorts[i].impactPower);
    }


    fclose(file);
}


/*
 * Save final conditions and battle result.
 */
void saveFinalConditions(Battleship battleship,
                         EscortShip escorts[],
                         int numberOfEscorts,
                         double battlefieldSize,
                         int escortsHit,
                         int sinkingEscort)
{
    FILE *file;
    int i;


    file =
        fopen("results/final_conditions.txt", "w");


    if (file == NULL)
    {
        printf("Could not create final_conditions.txt\n");
        return;
    }


    fprintf(file,
            "NAVAL BATTLE SIMULATOR\n");
    fprintf(file,
            "PART 1-A - FINAL CONDITIONS\n\n");


    fprintf(file,
            "Battlefield Size: %.2f\n\n",
            battlefieldSize);


    fprintf(file,
            "BATTLESHIP\n");

    fprintf(file,
            "Notation: %c\n",
            battleship.notation);

    fprintf(file,
            "Type: %s\n",
            battleship.name);

    fprintf(file,
            "Position: (%.2f, %.2f)\n",
            battleship.position.x,
            battleship.position.y);

    fprintf(file,
            "Status: %s\n\n",
            battleship.destroyed
                ? "DESTROYED"
                : "ALIVE");


    fprintf(file,
            "BATTLE RESULT\n");

    fprintf(file,
            "Escort ships destroyed by B: %d\n",
            escortsHit);


    if (sinkingEscort > 0)
    {
        fprintf(file,
                "E that sank B: E%d\n",
                sinkingEscort);
    }
    else
    {
        fprintf(file,
                "B survived the battle.\n");
    }


    fprintf(file,
            "\nESCORT FINAL CONDITIONS\n");


    for (i = 0;
         i < numberOfEscorts;
         i++)
    {
        fprintf(file,
                "E%d - E_%c - %s - "
                "Position (%.2f, %.2f) - "
                "Status: %s\n",
                i + 1,
                escorts[i].notation,
                escorts[i].name,
                escorts[i].position.x,
                escorts[i].position.y,
                escorts[i].destroyed
                    ? "DESTROYED"
                    : "ALIVE");
    }


    fclose(file);
}
