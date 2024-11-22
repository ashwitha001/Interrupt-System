#include "interrupts.h"


//Read Trace file and populate TraceEvent array with events
void readFile(char* filename, TraceEvent trace[], int* size) {
    //open file in read mode
    FILE* file = fopen(filename, "r");
    //if file cannot open, print error message and exit
    if (!file) {
        fprintf(stderr, "Error opening trace file: %s\n", filename);
        exit(1);
    }

    int i = 0;
    //read lines from file until END OF FILE
    //reads string until event type, followed by the integer duration
    while (fscanf(file, "%[^,], %d", trace[i].type, &trace[i].duration) != EOF) {
        i++;
    }
    //set size to num of trace events read
    *size = i;

    fclose(file);
}

void run(TraceEvent event, int* time, FILE* output, const VectorTable vector_table[]) {
    //If the event type is "CPU", start CPU execution. 
    if (strcmp(event.type, "CPU") == 0) {
        //CPU execution, increment the time by duration of event
        fprintf(output, "%d, %d, CPU execution\n", *time, event.duration);  
        *time += event.duration; 
        //If the event type is "SYSCALL", handle the system call interrupt
    } else if (strncmp(event.type, "SYSCALL", 7) == 0) {
        int interrupt_num;
        //from event type find the interrupt number
        if (sscanf(event.type, "SYSCALL %d", &interrupt_num) == 1) {
            //Check if interrupt num is valid, and within the vector table size
            if (interrupt_num >= 0 && interrupt_num < VECTOR_TABLE_SIZE) {
                //switch to kernel mode and increment the time by 1ms
                fprintf(output, "%d, %d, switch to kernel mode\n", *time, 1);
                *time += 1;
                //save context and increment time
                fprintf(output, "%d, %d, context saved\n", *time, 3);
                *time += 3;
                //find the location of interrupt vector in mem and increment time
                char find_vector_msg[50];
                snprintf(find_vector_msg, sizeof(find_vector_msg), 
                         "find vector %d in memory position 0x%04X", interrupt_num, interrupt_num * 2);
                fprintf(output, "%d, %d, %s\n", *time, 1, find_vector_msg);
                *time += 1;

                //load ISR address into PC and increment time
                char load_isr_msg[50];
                snprintf(load_isr_msg, sizeof(load_isr_msg), 
                         "load address 0x%04X into the PC", vector_table[interrupt_num].address);
                fprintf(output, "%d, %d, %s\n", *time, 1, load_isr_msg);
                *time += 1;

                //simulate ISR. increment time
                fprintf(output, "%d, %d, SYSCALL: run the ISR\n", *time, 230);
                *time += 47;
                //simulate data transfer during syscall. increment time
                fprintf(output, "%d, %d, transfer data\n", *time, 110);
                *time += 57;

                //check for errors
                fprintf(output, "%d, %d, check for errors\n", *time, 50);
                *time += 15;

                //return from the interrupt using IRET. increment time
                fprintf(output, "%d, %d, IRET\n", *time, 1);
                *time += 1;
            } else {
                //error if the interrupt number is invalid
                fprintf(stderr, "Invalid SYSCALL interrupt number: %d\n", interrupt_num);
            }
        }
        //If event type is "END_IO", handle the end of the I/O interrupt
    } else if (strncmp(event.type, "END_IO", 6) == 0) {
        int interrupt_num;
        //get interrupt num from event type
        if (sscanf(event.type, "END_IO %d", &interrupt_num) == 1) {
            if (interrupt_num >= 0 && interrupt_num < VECTOR_TABLE_SIZE) {
                //check priorty of interrupt. increment time
                fprintf(output, "%d, %d, check priority of interrupt\n", *time, 1);
                *time += 1;

                //check if the interrupt is masked. increment time
                fprintf(output, "%d, %d, check if masked\n", *time, 1);
                *time += 1;

                //switch to kernel mode
                fprintf(output, "%d, %d, switch to kernel mode\n", *time, 1);
                *time += 1;

                //save context
                fprintf(output, "%d, %d, context saved\n", *time, 3);
                *time += 3;

                //find the location of interrupt vector in mem and increment time
                char find_vector_msg[50];
                snprintf(find_vector_msg, sizeof(find_vector_msg), 
                         "find vector %d in memory position 0x%04X", interrupt_num, interrupt_num * 2);
                fprintf(output, "%d, %d, %s\n", *time, 1, find_vector_msg);
                *time += 1;

                //load ISR address into PC and increment time
                char load_isr_msg[50];
                snprintf(load_isr_msg, sizeof(load_isr_msg), 
                         "load address 0x%04X into the PC", vector_table[interrupt_num].address);
                fprintf(output, "%d, %d, %s\n", *time, 1, load_isr_msg);
                *time += 1;

                //End of the I/O event
                fprintf(output, "%d, %d, END_IO\n", *time, 248);
                *time += 248;

                //return from interrupt using IRET
                fprintf(output, "%d, %d, IRET\n", *time, 1);
                *time += 1;
            } else {
                //print error if interrupt number is invalid
                fprintf(stderr, "Invalid END_IO interrupt number: %d\n", interrupt_num);
            }
        }
    }
}



void startSimulation(TraceEvent trace[], int size, FILE *output_file, VectorTable vector_table[VECTOR_TABLE_SIZE]) {
    int time = 0; // Simulation time is initialized to 0

    // Loop over each trace event in the array
    for (int i = 0; i < size; i++) {
        TraceEvent event = trace[i];  // Get the current trace event
        printf("eventType: %s, duration: %d\n", event.type, event.duration); 

        // Run the event
        run(event, &time, output_file, vector_table);  
    }
}


int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "%s <input_trace_file> <output_file>\n", argv[0]);
        return 1;
    }

    char *trace_filename = argv[1];
    char *output_filename = argv[2];
    TraceEvent trace[MAX_TRACE];
    int size;

    FILE *output_file = fopen(output_filename, "w");
    if (output_file == NULL) {
        perror("Error opening output file");
        return 1;
    }

    //hard coded vector table
    VectorTable vector_table[VECTOR_TABLE_SIZE] = {
        { 0, 0x01E3 }, { 1, 0x029C }, { 2, 0x0695 }, { 3, 0x042B },
        { 4, 0x0292 }, { 5, 0x048B }, { 6, 0x0639 }, { 7, 0x00BD },
        { 8, 0x06EF }, { 9, 0x036C }, { 10, 0x07B0 }, { 11, 0x01F8 },
        { 12, 0x03B9 }, { 13, 0x06C7 }, { 14, 0x0165 }, { 15, 0x0584 },
        { 16, 0x02DF }, { 17, 0x05B3 }, { 18, 0x060A }, { 19, 0x0765 },
        { 20, 0x07B7 }, { 21, 0x0523 }, { 22, 0x03B7 }, { 23, 0x028C },
        { 24, 0x05E8 }, { 25, 0x05D3 }
    };

    // Start the simulation
    readFile(trace_filename, trace, &size);
    startSimulation(trace, size, output_file, vector_table);

    fclose(output_file);

    return 0;
}
