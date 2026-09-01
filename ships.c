#include <stdlib.h>
#include <time.h>
#include "ships.h"

void createEscortShips(EscortShip escorts[],
                       int numberOfEscorts,
                       double battlefieldSize)
{
    int i;

    srand((unsigned int)time(NULL));

    for (i = 0; i < numberOfEscorts; i++)
    {
        escorts[i].id = i + 1;

        escorts[i].position.x =
            ((double)rand() / RAND_MAX) * battlefieldSize;

        escorts[i].position.y =
            ((double)rand() / RAND_MAX) * battlefieldSize;
    }
}
