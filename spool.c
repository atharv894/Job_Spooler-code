/*
 * This program simulates a print queue using three different
 * scheduling algorithms:
 * 1. First-Come, First-Served (FCFS)
 * 2. Shortest Job First (SJF) (non-preemptive)
 * 3. Priority Scheduling (non-preemptive)
 *
 * It allows a user to add print jobs (with page count and priority)
 * and then run simulations to compare the performance metrics.
 */

#include <stdio.h>
#include <stdlib.h> // For qsort
#include <string.h> // For memcpy

#define MAX_JOBS 100 // Maximum number of jobs in the queue

// Structure to represent a single print job
typedef struct {
    int job_id;
    int page_count; // Analogous to "Burst Time"
    int priority;   // Lower number = higher priority
} PrintJob;

// --- Global Variables ---
PrintJob job_queue[MAX_JOBS]; // This is our main job queue
int job_count = 0;            // Number of jobs currently in the queue
int next_job_id = 1;          // To assign unique IDs

// --- Function Declarations ---
void addJob();
void runFCFS();
void runSJF();
void runPriority();
void displayQueue();
void calculateMetrics(PrintJob queue[], int count, const char* algorithmName);

// --- qsort Comparator Functions ---

// Comparator for Shortest Job First (SJF)
// Sorts based on page_count (ascending)
int compareSJF(const void* a, const void* b) {
    PrintJob* jobA = (PrintJob*)a;
    PrintJob* jobB = (PrintJob*)b;
    return (jobA->page_count - jobB->page_count);
}

// Comparator for Priority Scheduling
// Sorts based on priority (ascending)
// If priorities are equal, uses job_id (FCFS) as a tie-breaker
int comparePriority(const void* a, const void* b) {
    PrintJob* jobA = (PrintJob*)a;
    PrintJob* jobB = (PrintJob*)b;

    if (jobA->priority != jobB->priority) {
        return (jobA->priority - jobB->priority);
    } else {
        // Tie-breaker: First-Come, First-Served
        return (jobA->job_id - jobB->job_id);
    }
}


// --- Main Function ---
int main() {
    int choice = 0;

    while (1) {
        printf("\n--- Print Job Spooler Simulation ---\n");
        printf("1. Add Print Job\n");
        printf("2. Display Current Queue (Unsorted)\n");
        printf("3. Run FCFS Simulation\n");
        printf("4. Run SJF Simulation\n");
        printf("5. Run Priority Simulation\n");
        printf("6. Exit\n");
        printf("--------------------------------------\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1) {
            // Clear invalid input
            while (getchar() != '\n');
            printf("Invalid input. Please enter a number.\n");
            continue;
        }

        switch (choice) {
            case 1:
                addJob();
                break;
            case 2:
                displayQueue();
                break;
            case 3:
                runFCFS();
                break;
            case 4:
                runSJF();
                break;
            case 5:
                runPriority();
                break;
            case 6:
                printf("Exiting simulation. Goodbye!\n");
                return 0;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }

    return 0;
}

// --- Function Implementations ---

/**
 * @brief Adds a new job to the global job_queue.
 * Takes user input for page count and priority.
 */
void addJob() {
    if (job_count >= MAX_JOBS) {
        printf("Error: Print queue is full. Cannot add more jobs.\n");
        return;
    }

    PrintJob newJob;
    newJob.job_id = next_job_id++;

    printf("  Enter Page Count (e.g., 50): ");
    scanf("%d", &newJob.page_count);

    printf("  Enter Priority (1=Faculty, 2=Student, 3=Guest): ");
    scanf("%d", &newJob.priority);

    if (newJob.page_count <= 0 || newJob.priority <= 0) {
        printf("Error: Page count and priority must be positive.\n");
        next_job_id--; // Roll back the ID
        return;
    }

    job_queue[job_count] = newJob;
    job_count++;

    printf("  Success: Added Job %d (%d pages, priority %d).\n",
           newJob.job_id, newJob.page_count, newJob.priority);
}

/**
 * @brief Displays all jobs currently in the queue in their arrival order.
 */
void displayQueue() {
    if (job_count == 0) {
        printf("The print queue is currently empty.\n");
        return;
    }

    printf("\n--- Current Print Queue (FCFS Order) ---\n");
    printf("Job ID | Page Count | Priority\n");
    printf("----------------------------------\n");
    for (int i = 0; i < job_count; i++) {
        printf("%-6d | %-10d | %-8d\n",
               job_queue[i].job_id,
               job_queue[i].page_count,
               job_queue[i].priority);
    }
}

/**
 * @brief Runs the FCFS simulation.
 * It's the simplest: just processes the queue as-is.
 */
void runFCFS() {
    if (job_count == 0) {
        printf("Cannot run simulation: The print queue is empty.\n");
        return;
    }
    // FCFS processes the queue in its original order.
    // No sorting is needed.
    calculateMetrics(job_queue, job_count, "First-Come, First-Served (FCFS)");
}

/**
 * @brief Runs the SJF simulation.
 * Creates a copy of the queue, sorts it by page_count,
 * and then calculates metrics.
 */
void runSJF() {
    if (job_count == 0) {
        printf("Cannot run simulation: The print queue is empty.\n");
        return;
    }

    // 1. Create a temporary copy of the queue to sort
    PrintJob temp_queue[MAX_JOBS];
    memcpy(temp_queue, job_queue, job_count * sizeof(PrintJob));

    // 2. Sort the temporary queue using qsort with the SJF comparator
    qsort(temp_queue, job_count, sizeof(PrintJob), compareSJF);

    // 3. Calculate metrics on the sorted queue
    calculateMetrics(temp_queue, job_count, "Shortest Job First (SJF)");
}

/**
 * @brief Runs the Priority simulation.
 * Creates a copy of the queue, sorts it by priority,
 * and then calculates metrics.
 */
void runPriority() {
    if (job_count == 0) {
        printf("Cannot run simulation: The print queue is empty.\n");
        return;
    }

    // 1. Create a temporary copy
    PrintJob temp_queue[MAX_JOBS];
    memcpy(temp_queue, job_queue, job_count * sizeof(PrintJob));

    // 2. Sort the copy using qsort with the Priority comparator
    qsort(temp_queue, job_count, sizeof(PrintJob), comparePriority);

    // 3. Calculate metrics on the sorted queue
    calculateMetrics(temp_queue, job_count, "Priority Scheduling");
}

/**
 * @brief The core logic engine. Calculates and prints performance
 * metrics for a given (potentially sorted) queue.
 *
 * @param queue The array of print jobs to process.
 * @param count The number of jobs in the array.
 * @param algorithmName The name of the algorithm for display.
 */
void calculateMetrics(PrintJob queue[], int count, const char* algorithmName) {
    double total_wait_time = 0;
    double total_turnaround_time = 0;
    int current_time = 0; // Represents the printer's clock

    printf("\n--- Simulation Results: %s ---\n", algorithmName);
    printf("Job ID | Pages | Priority | Wait Time | Turnaround Time\n");
    printf("----------------------------------------------------------\n");

    for (int i = 0; i < count; i++) {
        PrintJob job = queue[i];

        // Wait Time: Time from submission (assumed 0) until printing starts.
        // This is simply the time all previous jobs took to complete.
        int wait_time = current_time;

        // Turnaround Time: Time from submission (0) until job completion.
        // (Wait Time + Burst Time)
        int turnaround_time = wait_time + job.page_count;

        // Update totals
        total_wait_time += wait_time;
        total_turnaround_time += turnaround_time;

        // The printer is now busy for the duration of this job
        current_time += job.page_count;

        // Print the results for this job
        printf("%-6d | %-5d | %-8d | %-9d | %-15d\n",
               job.job_id,
               job.page_count,
               job.priority,
               wait_time,
               turnaround_time);
    }

    // Calculate and print averages
    double avg_wait_time = total_wait_time / count;
    double avg_turnaround_time = total_turnaround_time / count;

    printf("----------------------------------------------------------\n");
    printf("Average Waiting Time:     %.2f\n", avg_wait_time);
    printf("Average Turnaround Time:  %.2f\n", avg_turnaround_time);
}
