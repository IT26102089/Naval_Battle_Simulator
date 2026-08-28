#include <stdio.h>

int main()
{
    double battlefieldSize;
    double battleshipX;
    double battleshipY;

    int numberOfEscorts;
    int i;

    printf("=====================================\n");
    printf("   ADVANCED NAVAL BATTLE SIMULATOR\n");
    printf("=====================================\n\n");

    printf("Enter battlefield size: ");
    scanf("%lf", &battlefieldSize);

    printf("Enter battleship X position: ");
    scanf("%lf", &battleshipX);

    printf("Enter battleship Y position: ");
    scanf("%lf", &battleshipY);

    if (battleshipX < 0 || battleshipX > battlefieldSize ||
        battleshipY < 0 || battleshipY > battlefieldSize)
    {
        printf("\nInvalid battleship position!\n");
        printf("Position must be inside the battlefield.\n");

        return 0;
    }

    printf("\nEnter number of escort ships: ");
    scanf("%d", &numberOfEscorts);

    if (numberOfEscorts <= 0)
    {
        printf("\nInvalid number of escort ships!\n");
        return 0;
    }

    printf("\nBattlefield created!\n");
    printf("Size: %.2f x %.2f\n", battlefieldSize, battlefieldSize);

    printf("Battleship position: (%.2f, %.2f)\n",
           battleshipX, battleshipY);

    printf("\nEscort Ships:\n");

    for (i = 0; i < numberOfEscorts; i++)
    {
        printf("Escort Ship E%d\n", i + 1);
    }

    return 0;
}
