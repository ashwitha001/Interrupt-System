#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TRACE 100
#define VECTOR_TABLE_SIZE 100
#define MAX_EVENT_TYPE 100

// Structure to hold trace events
typedef struct {
    char type[50];
    int number;
    int duration;
} TraceEvent;

// ISR structure for the vector table
typedef struct {
    int interrupt_num;
    int address;
} VectorTable;

// Function declarations
void run(TraceEvent event, int* time, FILE* output, const VectorTable vector_table[]);
void startSimulation(TraceEvent trace[], int size, FILE *output_file, VectorTable vector_table[VECTOR_TABLE_SIZE]);
void readFile(char* filename, TraceEvent trace[], int* size);
#endif
