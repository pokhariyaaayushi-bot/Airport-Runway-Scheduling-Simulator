#include <stdio.h>
#include <string.h>
#include <time.h>
#include "flight.h"
#include "report.h"

void generateReportByID(Flight *root) {
    printf("\n=== Report by Flight ID ===\n");
    inorderTraversal(root);
}

void generateReportByTime(Flight *root) {
    printf("\n=== Report by Entry Time ===\n");
    printFlights(root);
}

void inorderTraversal(Flight *root) {
    if (root == NULL) return;
    inorderTraversal(root->left);
    char entryStr[26];
    formatTime(root->entryTime, entryStr);
    printf("Flight %d (%s): Entry=%s, Status=%d\n", root->id, root->name, entryStr, root->status);
    inorderTraversal(root->right);
}

void printFlights(Flight *root) {
    if (root == NULL) return;
    printFlights(root->left);
    char entryStr[26], exitStr[26];
    formatTime(root->entryTime, entryStr);
    if (root->status == PROCESSED || root->status == CANCELLED)
        formatTime(root->exitTime, exitStr);
    else
        strcpy(exitStr, "N/A");
    printf("Flight %d: Entry=%s, Exit=%s\n", root->id, entryStr, exitStr);
    printFlights(root->right);
}
