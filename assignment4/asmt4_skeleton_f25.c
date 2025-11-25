#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

// Thread function prototypes
// Declare mutex and semaphore
// Global variables

// Main procedure
int main() {
	// Declare local variables
	// Prompt for number of students and number of chairs
	// Create thread handle for each student
	// Create dynamic array of students
    // Initialize mutex and semaphore
    // Create the TA thread
    // Create the student threads
    // Wait for all student threads to finish
    // End the TA thread after all students are done
    // Destroy mutex and semaphore
    return 0;
} // end main

// TA thread function
void* THREAD_NAME_FOR_TA(void* param) {
	// Run indefnitely in while loop		
        // Wait for a student to show up
        // Lock the mutex when a student is being helped
        // Check if students waiting
            // "Help" a student by reducing # of waiting students & print message
			// Unlock mutex 
			// Simulate time taken to help a student (delay to make output readable)
		// Else is no students waiting
			// Print message that TA is sleeping
			// Unlock mutex
			// Make TA sleep (delay to make output readable)
} // end thread function
    
// Student thread function
void* THREAD_NAME_FOR_STUDENT(void* num) {
    // typcast param to integer id

	// Run indefnitely in while loop		
        // Simulate time spent programming
        
        // Try to get help from the TA by locking mutex

		// If # waiting students is less than the number of chairs
			// Increment # waiting students
			// Print message that student "id" is waiting & number of waiting students	
			// Unlock mutex
			// Notify TA through semaphore
            // Simulate waiting time (delay to make output readable)
			
		// Else (no available chairs)
			// Print message that hallway is full, student "id" will try later
            // Unlock mutex
            // Delay to make output readable
} // end thread function
