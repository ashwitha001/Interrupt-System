# Interrupt Simulation

This project simulates the handling of CPU execution, system calls, and interrupt events based on a provided trace file. 
The simulation uses a vector table to manage interrupts and generates a detailed log of execution events.

## Files in the Project
### Source Code
- **`interrupts.h`**: Header file containing structure definitions, constants, and function declarations for the simulation.
- **`interrupts.c`**: Implementation of the simulation logic, including trace file reading, event handling, and output logging.

### Input Files
- **`trace#.txt`**: A file describing a sequence of events (CPU execution, SYSCALLs, and END_IOs) with durations for each event.

### Output Files
- **`execution#.txt`**: The output file generated by the simulation, logs each event with timestamps, durations, and descriptions.

### Test Scripts
- **`test#.sh`**: Automates the run process. Double-clicking these files will execute the tests and produce the `execution.txt` output.

## How It Works

### Simulation Flow
1. The program reads the event trace from `trace.txt`.
3. Outputs all logged events to `execution.txt`.