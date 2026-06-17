#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "flight.h"
#include "queue.h"
#include "priority_queue.h"
#include "report.h"

Flight *bstRoot = NULL;
Queue normalQueue;
PriorityQueue emergencyPQ;

void addFlight();
void processNextFlight();
void cancelFlight();
void showMenu();

int main()
{
    srand(time(NULL));
    initializeQueue(&normalQueue);
    initializePriorityQueue(&emergencyPQ);

    int choice;
    while (1)
    {
        showMenu();
        printf("Enter choice: ");
        scanf("%d", &choice);
        getchar();

        switch (choice)
        {
        case 1:
            addFlight();
            break;
        case 2:
            processNextFlight();
            break;
        case 3:
            cancelFlight();
            break;
        case 4:
            printf("\nGenerating reports...\n");
            generateReportByID(bstRoot);
            generateReportByTime(bstRoot);
            break;
        case 5:
            printf("Exiting simulation...\n");
            return 0;
        default:
            printf("Invalid choice!\n");
        }
    }
    return 0;
}

void showMenu()
{
    printf("\n=== Airport Runway Simulation ===\n");
    printf("1. Add Flight (manual or random)\n");
    printf("2. Process Next Flight\n");
    printf("3. Cancel Flight\n");
    printf("4. Generate Reports\n");
    printf("5. Exit\n");
}

void addFlight()
{
    int id, typeChoice, priorityChoice, emergChoice;
    char name[50];

    printf("\nAdd Random Flight (1) or Manual (2)? ");
    int mode;
    scanf("%d", &mode);

    Flight *f = NULL;

    if (mode == 1)
    {
        id = rand() % 900 + 100;
        sprintf(name, "Flight%d", id);
        typeChoice = rand() % 2;
        priorityChoice = (rand() % 10 < 2) ? 1 : 0;
        emergChoice = (priorityChoice == 1) ? rand() % 3 : 3;

        f = createFlight(
            id,
            name,
            (typeChoice == 0) ? LANDING : TAKEOFF,
            (priorityChoice == 1) ? EMERGENCY : NORMAL,
            (emergChoice == 0) ? ENGINE_FAILURE : (emergChoice == 1) ? MEDICAL
                                              : (emergChoice == 2)   ? FUEL_LOW
                                                                     : NONE);
        printf("Random Flight Added!\n");
    }
    else
    {
        printf("Enter Flight ID: ");
        scanf("%d", &id);
        getchar();

        printf("Enter Flight Name: ");
        fgets(name, sizeof(name), stdin);
        name[strcspn(name, "\n")] = '\0';

        printf("Type (1-Landing, 2-Takeoff): ");
        scanf("%d", &typeChoice);

        printf("Priority (1-Emergency, 2-Normal): ");
        scanf("%d", &priorityChoice);

        EmergencyType emType = NONE;
        if (priorityChoice == 1)
        {
            printf("Emergency Type (1-Engine Failure, 2-Medical, 3-Fuel Low): ");
            scanf("%d", &emergChoice);
            if (emergChoice == 1)
                emType = ENGINE_FAILURE;
            else if (emergChoice == 2)
                emType = MEDICAL;
            else if (emergChoice == 3)
                emType = FUEL_LOW;
        }

        f = createFlight(
            id,
            name,
            (typeChoice == 1) ? LANDING : TAKEOFF,
            (priorityChoice == 1) ? EMERGENCY : NORMAL,
            emType);
    }

    bstRoot = insertFlight(bstRoot, f);

    if (f == NULL)
    {
        printf("❌ Failed to create flight.\n");
        return;
    }

    if (f->priority == EMERGENCY)
        enqueuePriority(&emergencyPQ, f);
    else
        enqueue(&normalQueue, f);

    printf("✅ Flight %d (%s) added successfully at %s\n", f->id, f->name, ctime(&f->entryTime));
}

void processNextFlight()
{
    Flight *f = NULL;

    if (!isEmptyPriorityQueue(&emergencyPQ))
        f = dequeuePriority(&emergencyPQ);
    else if (!isEmptyQueue(&normalQueue))
        f = dequeue(&normalQueue);
    else
    {
        printf("No flights to process!\n");
        return;
    }

    f->status = PROCESSED;
    recordExitTime(f);
    printf("\n Processed Flight ID: %d (%s) | Exit: %s", f->id, f->name, ctime(&f->exitTime));
}

void cancelFlight()
{
    int id;
    printf("Enter Flight ID to cancel: ");
    scanf("%d", &id);

    Flight *f = searchFlight(bstRoot, id);
    if (f == NULL)
    {
        printf("❌ Flight not found.\n");
        return;
    }

    f->status = CANCELLED;
    recordExitTime(f);
    printf("❌ Flight ID %d (%s) cancelled at %s\n", f->id, f->name, ctime(&f->exitTime));
}
