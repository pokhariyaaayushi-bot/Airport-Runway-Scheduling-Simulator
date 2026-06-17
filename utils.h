#ifndef UTILS_H
#define UTILS_H

#include "flight.h"

void clearInputBuffer(void);
void runwayDelayDisplay(const char *msg, int seconds);
void autoCancelLongWaitFlights(Flight *root, int thresholdMinutes);

#endif
