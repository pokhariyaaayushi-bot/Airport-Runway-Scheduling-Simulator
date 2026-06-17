#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#include <conio.h>

#define MAX_FLIGHTS 100
#define MAX_NAME 20

typedef struct
{
    int id;
    char name[MAX_NAME];
    char type[10];
    char priority[10];
    char emergency[20];
    char status[10];
    time_t entryTime;
    int waitMinutes;
} Flight;

Flight flights[MAX_FLIGHTS];
int flightCount = 0;
Flight emergencyQueue[MAX_FLIGHTS];
int emergencyCount = 0;
Flight normalQueue[MAX_FLIGHTS];
int normalCount = 0;

// Windows Console Colors
enum COLORS
{
    BLACK,
    BLUE,
    GREEN,
    CYAN,
    RED,
    MAGENTA,
    BROWN,
    LIGHTGRAY,
    DARKGRAY,
    LIGHTBLUE,
    LIGHTGREEN,
    LIGHTCYAN,
    LIGHTRED,
    LIGHTMAGENTA,
    YELLOW,
    WHITE
};

void setColor(int color)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

void clearScreen()
{
    system("cls");
}

void hideCursor()
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo = {0};
    cursorInfo.dwSize = 20;
    cursorInfo.bVisible = 0;
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}

void printTitle()
{
    setColor(LIGHTCYAN);
    printf("\n");
    printf("  ███╗   ███╗ █████╗ ██╗██████╗  ██████╗██╗  ██╗███████╗██████╗ \n");
    printf("  ████╗ ████║██╔══██╗██║██╔══██╗██╔════╝██║ ██╔╝██╔════╝██╔══██╗\n");
    printf("  ██╔████╔██║███████║██║██████╔╝██║     █████╔╝ █████╗  ██████╔╝\n");
    printf("  ██║╚██╔╝██║██╔══██║██║██╔══██╗██║     ██╔═██╗ ██╔══╝  ██╔══██╗\n");
    printf("  ██║ ╚═╝ ██║██║  ██║██║██║  ██║╚██████╗██║  ██╗███████╗██║  ██║\n");
    printf("  ╚═╝     ╚═╝╚═╝  ╚═╝╚═╝╚═╝  ╚═╝ ╚═════╝╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝\n");
    setColor(WHITE);
    printf("\n═══════════════════════════════════════════════════════════════════════════════\n\n");
}

void drawKPIs()
{
    int total = flightCount;
    int processed = 0, pending = 0, cancelled = 0;
    float avgWait = 0;

    for (int i = 0; i < flightCount; i++)
    {
        if (strcmp(flights[i].status, "PROCESSED") == 0)
        {
            processed++;
            avgWait += flights[i].waitMinutes;
        }
        else if (strcmp(flights[i].status, "PENDING") == 0)
        {
            pending++;
        }
        else if (strcmp(flights[i].status, "CANCELLED") == 0)
        {
            cancelled++;
        }
    }
    if (processed > 0)
        avgWait /= processed;

    setColor(LIGHTCYAN);
}
