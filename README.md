# OS Scheduler Simulator

A comprehensive operating system scheduler simulator with a graphical user interface built using GTK. This project implements various scheduling algorithms and provides visualization tools for process management and memory allocation.

## Features

- Multiple CPU Scheduling Algorithms:
  - First Come First Serve (FCFS)
  - Round Robin
  - Multi-Level Feedback Queue (MLFQ)
- Process Management
  - Process Creation and Configuration
  - Process Control Block (PCB) Management
  - Resource Management
- Memory Management
  - Memory Allocation and Deallocation
  - Memory Viewer
- Graphical User Interface
  - Interactive Dashboard
  - Memory Visualization
  - Process Execution Visualization
  - Logging and Console Output

## Project Structure

```
.
├── main.c                  # Main entry point
├── memory.c/h             # Memory management implementation
├── os.c/h                 # OS core functionality
├── pcb.c/h               # Process Control Block implementation
├── schedulers/           # Scheduling algorithms
│   ├── fcfs.c/h         # First Come First Serve
│   ├── mlfq.c/h         # Multi-Level Feedback Queue
│   ├── round_robin.c/h  # Round Robin
│   └── queue.c/h        # Queue data structure
├── ui/                   # User Interface components
│   ├── dashboard.c/h
│   ├── memory_viewer.c/h
│   ├── process_creation_and_config.c/h
│   ├── resource_management.c/h
│   └── scheduler_control.c/h
└── programs/             # Sample programs for testing
```

## Prerequisites

- C compiler (GCC recommended)
- GTK3 development libraries
- CMake build system

## Building the Project

1. Create a build directory:
```bash
mkdir build
cd build
```

2. Generate build files with CMake:
```bash
cmake ..
```

3. Build the project:
```bash
make
```

## Running the Simulator

After building, you can run the simulator from the build directory:

```bash
./OS_MS2
```

## Usage

1. Create processes using the Process Creation interface
2. Configure scheduling parameters through the Scheduler Control panel
3. Monitor process execution in the Dashboard
4. View memory allocation in the Memory Viewer
5. Check system logs in the Console Output

## Contributing

Feel free to contribute to this project by:
1. Fork the repository
2. Create a new feature branch
3. Submit a pull request

## License

This project is open source and available under the MIT License.