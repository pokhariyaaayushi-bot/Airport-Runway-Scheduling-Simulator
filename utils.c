#include <stdio.h>
#include <unistd.h>
#include "utils.h"

void clearInputBuffer(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
    {
    }
}

void runwayDelayDisplay(const char *msg, int seconds)
{
    printf("%s", msg);
    fflush(stdout);
    for (int i = 0; i < seconds; i++)
    {
        printf(".");
        fflush(stdout);
        sleep(1);
    }
    printf(" Done\n");
}

void autoCancelLongWaitFlights(Flight *root, int thresholdMinutes)
{
    if (root == NULL)
        return;

    autoCancelLongWaitFlights(root->left, thresholdMinutes);

    time_t currentTime = time(NULL);
    int waitingTime = (int)difftime(currentTime, root->entryTime) / 60;

    if (root->status != PROCESSED && root->status != CANCELLED && waitingTime > thresholdMinutes)
    {
        root->status = CANCELLED;
        recordExitTime(root);
        printf("Auto-cancelled Flight %d (wait=%d > %d)\n", root->id, waitingTime, thresholdMinutes);
    }

    autoCancelLongWaitFlights(root->right, thresholdMinutes);
}
