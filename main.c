#include <stdio.h>

#include "ships.h"
#include "battle.h"

int main(void)
{
    double battlefieldSize;

    Battleship battleship;
    EscortShip escorts[MAX_ESCORTS];

    double escortX[MAX_ESCORTS];
    double escortY[MAX_ESCORTS];

    int numberOfEscorts;
    int i;

    printf("=====================================\n");
    printf("   ADVANCED NAVAL BATTLE SIMULATOR\n");
    printf("=====================================\n\n");

    /*
     * Battlefield size
     */
    printf("Enter battlefield size: ");
    scanf("%lf", &battlefieldSize);

    if (battlefieldSize <= 0)
    {
        printf("Invalid battlefield size!\n");
        return 0;
    }

    /*
     * Battleship position
     */
    printf("Enter battleship X position: ");
    scanf("%lf", &battleship.position.x);

    printf("Enter battleship Y position: ");
    scanf("%lf", &battleship.position.y);

    /*
     * Validate Battleship
     */
    if (!isInsideBattlefield(battleship.position.x,
                             battleship.position.y,
                             battlefieldSize))
    {
        printf("\nInvalid battleship position!\n");
        printf("Position must be inside the battlefield.\n");

        return 0;
    }

    /*
     * Number of Escort Ships
     */
    printf("\nEnter number of escort ships: ");
    scanf("%d", &numberOfEscorts);

    if (numberOfEscorts <= 0 ||
        numberOfEscorts > MAX_ESCORTS)
    {
        printf("\nInvalid number of escort ships!\n");
        printf("Please enter a number between 1 and %d.\n",
               MAX_ESCORTS);

        return 0;
    }

    /*
     * Generate Escort Ships
     */
    createEscortShips(escorts,
                      numberOfEscorts,
                      battlefieldSize);

    /*
     * Copy coordinates for display.
     */
    for (i = 0; i < numberOfEscorts; i++)
    {
        escortX[i] = escorts[i].position.x;
        escortY[i] = escorts[i].position.y;
    }

    /*
     * Display ship information.
     */
    printf("\n=====================================\n");
    printf("         SHIPS CREATED\n");
    printf("=====================================\n");

    printf("Battleship: (%.2f, %.2f)\n",
           battleship.position.x,
           battleship.position.y);

    for (i = 0; i < numberOfEscorts; i++)
    {
        printf("E%d: (%.2f, %.2f)\n",
               escorts[i].id,
               escorts[i].position.x,
               escorts[i].position.y);
    }

    /*
     * Display visual battlefield.
     */
    displayBattlefield(battlefieldSize,
                       battleship.position.x,
                       battleship.position.y,
                       escortX,
                       escortY,
                       numberOfEscorts);

    return 0;
}
