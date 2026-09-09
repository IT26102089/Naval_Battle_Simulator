#include "part2Settings.h"

static int fullProgram = 0;
static int basicReady = 0;
static int escortReady = 0;

static double savedBInterval;
static int savedPathK;
static int savedPathT;
static double savedPathJamAngle;
static int savedK;
static int savedT;
static double savedJamAngle;
static double savedEIntervals[5];

void setPart2FullProgram(int enabled)
{
    fullProgram = enabled;
    basicReady = 0;
    escortReady = 0;
}

int isPart2FullProgram(void)
{
    return fullProgram;
}

void savePart2PathSettings(int k, int t, double jamAngle)
{
    savedPathK = k;
    savedPathT = t;
    savedPathJamAngle = jamAngle;
}

int loadPart2PathSettings(int *k, int *t, double *jamAngle)
{
    if (savedPathK <= 0)
        return 0;

    *k = savedPathK;
    *t = savedPathT;
    *jamAngle = savedPathJamAngle;
    return 1;
}

void savePart2BasicSettings(double bInterval, int k, int t, double jamAngle)
{
    savedBInterval = bInterval;
    savedK = k;
    savedT = t;
    savedJamAngle = jamAngle;
    basicReady = 1;
}

int loadPart2BasicSettings(double *bInterval, int *k, int *t, double *jamAngle)
{
    if (!basicReady)
        return 0;

    *bInterval = savedBInterval;
    *k = savedK;
    *t = savedT;
    *jamAngle = savedJamAngle;
    return 1;
}

void savePart2EscortIntervals(double intervals[], int count)
{
    int i;

    for (i = 0; i < count && i < 5; i++)
        savedEIntervals[i] = intervals[i];

    escortReady = 1;
}

int loadPart2EscortIntervals(double intervals[], int count)
{
    int i;

    if (!escortReady)
        return 0;

    for (i = 0; i < count && i < 5; i++)
        intervals[i] = savedEIntervals[i];

    return 1;
}
