#ifndef FLIGHT_H
#define FLIGHT_H

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum { NORMAL, EMERGENCY } PriorityType;
typedef enum { LANDING, TAKEOFF } FlightType;
typedef enum { ENGINE_FAILURE, MEDICAL, FUEL_LOW, NONE } EmergencyType;
typedef enum { PENDING, PROCESSED, CANCELLED } FlightStatus;

typedef struct Flight {
    int id;
    char name[50];
    FlightType type;
    PriorityType priority;
    EmergencyType emergency;
    FlightStatus status;

    time_t entryTime, exitTime;
    double waitMinutes;

    struct Flight *left, *right;
} Flight;

Flight* createFlight(int id, char *name, FlightType type, PriorityType priority, EmergencyType emergency);
Flight* insertFlight(Flight *root, Flight *f);
Flight* searchFlight(Flight *root, int id);
void recordExitTime(Flight *f);
void calculateWaitTime(Flight *f);
char* formatTime(time_t t, char *buffer);

#endif
