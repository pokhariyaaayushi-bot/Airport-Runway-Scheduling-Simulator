#ifndef REPORT_H
#define REPORT_H
#include "flight.h"

void addFlightByTime(Flight *f);
void initializeSummary(void);
void finalizeSummary(void);
void generateReportByID(Flight *root);
void generateReportByTime(Flight *root);
void inorderTraversal(Flight *root);
void printFlights(Flight *root);

#endif
