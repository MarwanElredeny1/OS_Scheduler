cmake_minimum_required(VERSION 3.22)
project(OS_MS2 C)

# Set C standard
set(CMAKE_C_STANDARD 11)

# Find and configure GTK3
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

# Include GTK headers and libraries
include_directories(${GTK3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})

# Add executable
add_executable(
        OS_MS2
        main.c
        memory.c
        os.c
        pcb.c
        ui/dashboard.c
        ui/log_and_console.c
        ui/memory_viewer.c
        ui/process_creation_and_config.c
        ui/resource_management.c
        ui/scheduler_control.c
        ui/visualization_execution.c
        schedulers/queue.c
        schedulers/fcfs.c
        schedulers/round_robin.c
        schedulers/mlfq.c
)

# Link GTK libraries to your project
target_link_libraries(OS_MS2 ${GTK3_LIBRARIES} m)
