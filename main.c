#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Function to print the battlefield and ship coordinates
void displayBattlefield(double size,
                        double battleshipX,
                        double battleshipY,
                        double escortX[],
                        double escortY[],
                        int numberOfEscorts)
{
    int i;

    printf("\n-------------------------------------\n");
    printf("BATTLEFIELD\n");
    printf("-------------------------------------\n");

    // Print battleship location
    printf("B at (%.2f, %.2f)\n",
           battleshipX, battleshipY);

    // Loop through and print each escort ship position
    for (i = 0; i < numberOfEscorts; i++)
    {
        printf("E%d at (%.2f, %.2f)\n",
               i + 1,
               escortX[i],
               escortY[i]);
    }

    printf("-------------------------------------\n");
}

int main()
{
    // Variables for grid size and battleship coordinates
    double battlefieldSize;
    double battleshipX;
    double battleshipY;

    // Number of escort ships and loop index
    int numberOfEscorts;
    int i;

    // Arrays to store X and Y coordinates of escort ships
    double escortX[100];
    double escortY[100];

    // random number generator
    srand(time(NULL));

    // Welcome message
    printf("=====================================\n");
    printf("    ADVANCED NAVAL BATTLE SIMULATOR\n");
    printf("=====================================\n\n");

    // Get battlefield size from user
    printf("Enter battlefield size: ");
    scanf("%lf", &battlefieldSize);

    // Check if size is valid
    if (battlefieldSize <= 0)
    {
        printf("Invalid battlefield size!\n");
        return 0;
    }

    // Get battleship coordinates
    printf("Enter battleship X position: ");
    scanf("%lf", &battleshipX);

    printf("Enter battleship Y position: ");
    scanf("%lf", &battleshipY);

    // Check if battleship is inside the grid boundary
    if (battleshipX < 0 || battleshipX > battlefieldSize ||
        battleshipY < 0 || battleshipY > battlefieldSize)
    {
        printf("\nInvalid battleship position!\n");
        printf("Position must be inside the battlefield.\n");

        return 0;
    }

    // Get total number of escort ships
    printf("\nEnter number of escort ships: ");
    scanf("%d", &numberOfEscorts);

    // Validate number of escort ships (1 to 100)
    if (numberOfEscorts <= 0 || numberOfEscorts > 100)
    {
        printf("\nInvalid number of escort ships!\n");
        printf("Please enter a number between 1 and 100.\n");

        return 0;
    }

    // Randomly place escort ships within the battlefield size
    for (i = 0; i < numberOfEscorts; i++)
    {
        escortX[i] = ((double)rand() / RAND_MAX) * battlefieldSize;
        escortY[i] = ((double)rand() / RAND_MAX) * battlefieldSize;
    }

    // Output summary
    printf("\n=====================================\n");
    printf("          BATTLEFIELD CREATED\n");
    printf("=====================================\n");

    printf("Battlefield size: %.2f x %.2f\n",
           battlefieldSize, battlefieldSize);

    printf("Battleship position: (%.2f, %.2f)\n",
           battleshipX, battleshipY);

    printf("\nEscort Ships:\n");

    for (i = 0; i < numberOfEscorts; i++)
    {
        printf("E%d -> (%.2f, %.2f)\n",
               i + 1, escortX[i], escortY[i]);
    }

    // Calling function to display full battlefield output
    displayBattlefield(battlefieldSize,
                       battleshipX,
                       battleshipY,
                       escortX,
                       escortY,
                       numberOfEscorts);

    return 0;
}
