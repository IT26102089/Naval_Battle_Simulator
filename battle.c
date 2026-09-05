#include <stdio.h>
#include <math.h>

#include "battle.h"

#define G 9.81


double calculateDistance(
    Position a,
    Position b)
{
    double dx = b.x - a.x;
    double dy = b.y - a.y;

    return sqrt(dx * dx + dy * dy);
}


/*
 * A/L projectile equation:
 *
 * R = u^2 sin(2theta) / g
 */
double calculateProjectileRange(
    double velocity,
    double angleDegrees)
{
    double radians;

    radians =
        angleDegrees * acos(-1.0) / 180.0;

    return
        (velocity * velocity *
         sin(2.0 * radians)) / G;
}


/*
 * R = u cos(theta) t
 *
 * t = R / (u cos(theta))
 */
double calculateFlightTime(
    double distance,
    double velocity,
    double angleDegrees)
{
    double radians;

    radians =
        angleDegrees * acos(-1.0) / 180.0;

    if (velocity <= 0 ||
        fabs(cos(radians)) < 0.000001)
    {
        return 0.0;
    }

    return distance /
           (velocity * cos(radians));
}


/*
 * Checks whether B can hit E.
 *
 * minimumAngle and maximumAngle allow us
 * to use this function for:
 *
 * Part 1-A:
 * 0 - 90 degrees
 *
 * Part 1-B Simulation 2:
 * jammed minimum angle - 90 degrees
 */
int canBattleshipHitEscort(
    Battleship battleship,
    EscortShip escort,
    double minimumAngle,
    double maximumAngle,
    double *hitTime)
{
    double distance;
    double maxRange;
    double value;
    double theta1;
    double theta2;
    double selectedAngle;


    distance =
        calculateDistance(
            battleship.position,
            escort.position);


    /*
     * Maximum range within the permitted
     * angle range.
     *
     * Check several angles.
     */
    maxRange = 0.0;
    selectedAngle = minimumAngle;


    {
        double angle;

        for (angle = minimumAngle;
             angle <= maximumAngle;
             angle += 0.1)
        {
            double range =
                calculateProjectileRange(
                    battleship.vmax,
                    angle);

            if (range > maxRange)
            {
                maxRange = range;
                selectedAngle = angle;
            }
        }
    }


    if (distance > maxRange)
    {
        return 0;
    }


    /*
     * Solve:
     *
     * sin(2theta) = Rg/u^2
     */
    value =
        (distance * G) /
        (battleship.vmax *
         battleship.vmax);


    if (value > 1.0)
        value = 1.0;

    if (value < 0.0)
        value = 0.0;


    theta1 =
        0.5 * asin(value)
        * 180.0 / acos(-1.0);


    theta2 =
        90.0 - theta1;


    /*
     * Prefer lower-angle solution.
     */
    if (theta1 >= minimumAngle &&
        theta1 <= maximumAngle)
    {
        selectedAngle = theta1;
    }
    else if (theta2 >= minimumAngle &&
             theta2 <= maximumAngle)
    {
        selectedAngle = theta2;
    }
    else
    {
        return 0;
    }


    *hitTime =
        calculateFlightTime(
            distance,
            battleship.vmax,
            selectedAngle);

    return 1;
}


/*
 * Escort attack range.
 *
 * Escort has an angle range:
 *
 * theta_L to theta_H
 *
 * Therefore its attack range is not
 * necessarily a circle.
 */
int canEscortHitBattleship(
    Battleship battleship,
    EscortShip escort,
    double *hitTime)
{
    double distance;

    double thetaL;
    double thetaH;

    double value;
    double theta1;
    double theta2;
    double selectedAngle;

    double minRange = 999999999.0;
    double maxRange = 0.0;



    distance =
        calculateDistance(
            escort.position,
            battleship.position);


    thetaL =
        escort.angleMin;

    thetaH =
        escort.angleMin +
        escort.angleRange;


    if (thetaH > 90.0)
        thetaH = 90.0;


    /*
     * Find the exact minimum and maximum
     * projectile range in the allowed angle
     * interval.  R(theta) reaches its maximum
     * at 45 degrees, so the extrema are found
     * from the interval endpoints and 45 degrees
     * when it lies inside the interval.
     */
    {
        double rangeL =
            calculateProjectileRange(
                escort.vmax,
                thetaL);

        double rangeH =
            calculateProjectileRange(
                escort.vmax,
                thetaH);

        minRange = rangeL < rangeH
                 ? rangeL
                 : rangeH;

        maxRange = rangeL > rangeH
                 ? rangeL
                 : rangeH;

        if (thetaL <= 45.0 &&
            45.0 <= thetaH)
        {
            double range45 =
                calculateProjectileRange(
                    escort.vmax,
                    45.0);

            if (range45 > maxRange)
                maxRange = range45;
        }
    }


    /*
     * If target is outside the annular
     * attack region, it cannot be hit.
     */
    if (distance < minRange ||
        distance > maxRange)
    {
        return 0;
    }


    value =
        (distance * G) /
        (escort.vmax *
         escort.vmax);


    if (value > 1.0)
        value = 1.0;

    if (value < 0.0)
        value = 0.0;


    theta1 =
        0.5 * asin(value)
        * 180.0 / acos(-1.0);

    theta2 =
        90.0 - theta1;


    if (theta1 >= thetaL &&
        theta1 <= thetaH)
    {
        selectedAngle = theta1;
    }
    else if (theta2 >= thetaL &&
             theta2 <= thetaH)
    {
        selectedAngle = theta2;
    }
    else
    {
        return 0;
    }


    *hitTime =
        calculateFlightTime(
            distance,
            escort.vmax,
            selectedAngle);

    return 1;
}


/*
 * Simple coordinate-grid display.
 */
void displayBattlefield(
    double battlefieldSize,
    Battleship battleship,
    EscortShip escorts[],
    int numberOfEscorts)
{
    int row;
    int column;
    int i;

    int rows = 10;
    int columns = 10;


    printf("\n");
    printf("========== BATTLEFIELD ==========\n");


    for (row = rows - 1;
         row >= 0;
         row--)
    {
        for (column = 0;
             column < columns;
             column++)
        {
            double minX =
                battlefieldSize *
                column / columns;

            double maxX =
                battlefieldSize *
                (column + 1) / columns;

            double minY =
                battlefieldSize *
                row / rows;

            double maxY =
                battlefieldSize *
                (row + 1) / rows;

            int printed = 0;


            if (battleship.position.x >= minX &&
                battleship.position.x < maxX &&
                battleship.position.y >= minY &&
                battleship.position.y < maxY)
            {
                printf(" B ");
                printed = 1;
            }


            if (!printed)
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
                        printed = 1;
                        break;
                    }
                }
            }


            if (!printed)
                printf(" . ");
        }

        printf("\n");
    }

    printf("B = Battleship | E = Escort\n");
}


/*
 * Part 1-A initial file.
 */
void savePart1AInitial(
    Battleship battleship,
    EscortShip escorts[],
    int numberOfEscorts,
    double battlefieldSize)
{
    FILE *file;
    int i;


    file =
        fopen(
            "results/part1A_initial.txt",
            "w");


    if (file == NULL)
        return;


    fprintf(file,
            "NAVAL BATTLE SIMULATOR\n");
    fprintf(file,
            "PART 1-A INITIAL CONDITIONS\n\n");


    fprintf(file,
            "Battlefield: %.2f x %.2f\n\n",
            battlefieldSize,
            battlefieldSize);


    fprintf(file,
            "BATTLESHIP\n");

    fprintf(file,
            "Type: %c\n",
            battleship.notation);

    fprintf(file,
            "Name: %s\n",
            battleship.name);

    fprintf(file,
            "Gun: %s\n",
            battleship.gunName);

    fprintf(file,
            "Position: %.2f %.2f\n",
            battleship.position.x,
            battleship.position.y);

    fprintf(file,
            "Vmax: %.2f\n",
            battleship.vmax);

    fprintf(file,
            "Angle: 0 - 90 degrees\n\n");


    fprintf(file,
            "ESCORT SHIPS\n");


    for (i = 0;
         i < numberOfEscorts;
         i++)
    {
        fprintf(file,
                "\nE%d\n",
                escorts[i].index);

        fprintf(file,
                "Type: E_%c\n",
                escorts[i].notation);

        fprintf(file,
                "Name: %s\n",
                escorts[i].name);

        fprintf(file,
                "Position: %.2f %.2f\n",
                escorts[i].position.x,
                escorts[i].position.y);

        fprintf(file,
                "Vmin: %.2f\n",
                escorts[i].vmin);

        fprintf(file,
                "Vmax: %.2f\n",
                escorts[i].vmax);

        fprintf(file,
                "Theta_L: %.2f\n",
                escorts[i].angleMin);

        fprintf(file,
                "Theta_H: %.2f\n",
                escorts[i].angleMin +
                escorts[i].angleRange);

        fprintf(file,
                "Impact Power: %.2f\n",
                escorts[i].impactPower);
    }


    fclose(file);
}


/*
 * Part 1-A final file.
 */
void savePart1AFinal(
    Battleship battleship,
    EscortShip escorts[],
    int numberOfEscorts,
    int escortsHit,
    int sinkingEscort)
{
    FILE *file;
    int i;


    file =
        fopen(
            "results/part1A_final.txt",
            "w");


    if (file == NULL)
        return;


    fprintf(file,
            "PART 1-A FINAL CONDITIONS\n\n");


    fprintf(file,
            "Battleship: %c - %s\n",
            battleship.notation,
            battleship.name);


    fprintf(file,
            "Status: %s\n\n",
            battleship.destroyed
                ? "DESTROYED"
                : "ALIVE");


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
                "B survived.\n");
    }


    fprintf(file,
            "\nEscort final states:\n");


    for (i = 0;
         i < numberOfEscorts;
         i++)
    {
        fprintf(file,
                "E%d E_%c - %s\n",
                escorts[i].index,
                escorts[i].notation,
                escorts[i].destroyed
                    ? "DESTROYED"
                    : "ALIVE");
    }


    fclose(file);
}
