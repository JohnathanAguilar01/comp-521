#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define HELP_CYCLES 3   // How many times each student will seek help

// GLOBALS
pthread_mutex_t mutex;       // for protecting shared variables
sem_t ta_sem;                // TA waits on this until a student wakes them

int waiting_students = 0;    // number of students waiting in chairs
int num_chairs;              // total chairs
int total_students;          // total student threads

int students_done = 0;       // how many students finished all their help cycles

// THREAD PROTOTYPES
void* ta_thread(void* param);
void* student_thread(void* num);

// MAIN
int main() {
    printf("Enter number of students: ");
    scanf("%d", &total_students);

    printf("Enter number of chairs: ");
    scanf("%d", &num_chairs);

    pthread_t ta;
    pthread_t* students = malloc(sizeof(pthread_t) * total_students);

    // Initialize mutex & semaphore
    pthread_mutex_init(&mutex, NULL);
    sem_init(&ta_sem, 0, 0);

    // Create TA thread
    pthread_create(&ta, NULL, ta_thread, NULL);

    // Create student threads
    for (int i = 0; i < total_students; i++) {
        int* id = malloc(sizeof(int));
        *id = i + 1; 
        pthread_create(&students[i], NULL, student_thread, id);
    }

    // Wait for all students to finish
    for (int i = 0; i < total_students; i++)
        pthread_join(students[i], NULL);

    // End the TA thread after all students are done
    sem_post(&ta_sem);
    pthread_join(ta, NULL);

    // Destroy mutex and semaphore
    pthread_mutex_destroy(&mutex);
    sem_destroy(&ta_sem);
    free(students);

    printf("\nAll students finished. TA is done for the day.\n");
    return 0;
}


// TA THREAD
void* ta_thread(void* param) {
	  // Run indefnitely in while loop		
    while (1) {
        // Wait for a student to show up
        sem_wait(&ta_sem);

        // Lock the mutex when a student is being helped
        pthread_mutex_lock(&mutex);

        // Check if students waiting
        if (waiting_students > 0) {
            // "Help" a student by reducing # of waiting students & print message
            waiting_students--;
            printf("TA is helping a student. Students waiting: %d\n", waiting_students);
			      // Unlock mutex 
            pthread_mutex_unlock(&mutex);

			      // Simulate time taken to help a student (delay to make output readable)
            int min = 2;
            int max = 4;
            sleep(rand() % (max - min + 1) + min);
        } 
        else { // Else is no students waiting
            // No students waiting
            printf("TA is sleeping...\n");
			      // Unlock mutex
            pthread_mutex_unlock(&mutex);
			      // Make TA sleep (delay to make output readable)
            int min = 2;
            int max = 3;
            sleep(rand() % (max - min + 1) + min);
        }

        // end thread function
        if (students_done == total_students) {
            pthread_mutex_unlock(&mutex);
            pthread_exit(NULL);
        }
    }
}


// STUDENT THREAD
void* student_thread(void* num) {
    // typcast param to integer id
    int id = *(int*)num;
    free(num);

	  // Run indefnitely in while loop		
    for (int i = 0; i < HELP_CYCLES; i++) {

        // Simulate time spent programming
        sleep(rand() % 5 + 1);

        // Try to get help from the TA by locking mutex
        pthread_mutex_lock(&mutex);

		    // If # waiting students is less than the number of chairs
        if (waiting_students < num_chairs) {
			      // Increment # waiting students
            waiting_students++;
			      // Print message that student "id" is waiting & number of waiting students	
            printf("Student %d is waiting. Total waiting: %d\n", id, waiting_students);

			      // Unlock mutex
            pthread_mutex_unlock(&mutex);

			      // Notify TA through semaphore
            sem_post(&ta_sem);

            // Simulate waiting time (delay to make output readable)
            int min = 2;
            int max = 3;
            sleep(rand() % (max - min + 1) + min);
        } else { // Else (no available chairs)
			      // Print message that hallway is full, student "id" will try later
            printf("Student %d will return later. Hallway full.\n", id);
            // Unlock mutex
            pthread_mutex_unlock(&mutex);
            // Delay to make output readable
            sleep(2);
            i--;  // try again for this help cycle
        }
    }

    pthread_mutex_lock(&mutex);
    students_done++;
    pthread_mutex_unlock(&mutex);

    return NULL;
}
