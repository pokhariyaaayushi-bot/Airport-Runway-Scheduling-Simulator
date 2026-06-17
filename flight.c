#include "flight.h"

Flight *createFlight(int id, char *name, FlightType type, PriorityType priority, EmergencyType emergency)
{
    Flight *f = (Flight *)malloc(sizeof(Flight));
    f->id = id;
    strcpy(f->name, name);
    f->type = type;
    f->priority = priority;
    f->emergency = emergency;
    f->status = PENDING;
    f->entryTime = time(NULL);
    f->exitTime = 0;
    f->waitMinutes = 0;
    f->left = f->right = NULL;
    return f;
}

void recordEntryTime(Flight *f)
{
    f->entryTime = time(NULL);
}

void recordExitTime(Flight *f)
{
    f->exitTime = time(NULL);
    calculateWaitTime(f);
}

void calculateWaitTime(Flight *f)
{
    if (f->exitTime > f->entryTime)
        f->waitMinutes = difftime(f->exitTime, f->entryTime) / 60.0;
    else
        f->waitMinutes = 0;
}

char *formatTime(time_t t, char *buffer)
{
    struct tm *timeinfo = localtime(&t);
    strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", timeinfo);
    return buffer;
}

Flight *insertFlight(Flight *root, Flight *flight)
{
    if (root == NULL)
        return flight;

    if (flight->id < root->id)
        root->left = insertFlight(root->left, flight);
    else if (flight->id > root->id)
        root->right = insertFlight(root->right, flight);

    return root;
}

Flight *searchFlight(Flight *root, int id)
{
    if (root == NULL)
        return NULL;

    if (id == root->id)
        return root;
    else if (id < root->id)
        return searchFlight(root->left, id);
    else
        return searchFlight(root->right, id);
}
