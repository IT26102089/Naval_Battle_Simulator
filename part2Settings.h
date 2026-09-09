#ifndef PART2SETTINGS_H
#define PART2SETTINGS_H

/* Values shared between Part 2 simulations in Full Program mode. */
void setPart2FullProgram(int enabled);
int isPart2FullProgram(void);

void savePart2BasicSettings(double bInterval, int k, int t, double jamAngle);
int loadPart2BasicSettings(double *bInterval, int *k, int *t, double *jamAngle);

void savePart2PathSettings(int k, int t, double jamAngle);
int loadPart2PathSettings(int *k, int *t, double *jamAngle);

void savePart2EscortIntervals(double intervals[], int count);
int loadPart2EscortIntervals(double intervals[], int count);

#endif
