// dining_philosophers_semaphore.c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define NUM_PHILOSOPHERS 5

// Array of semaphores, each representing a chopstick
// Initialized to 1, so that each chopstick is available
sem_t chopsticks[NUM_PHILOSOPHERS];

// Mutex to synchronize access to console output
pthread_mutex_t output_mutex;

void* philosopher(void* num) {
    // Each philosopher is assigned a unique id (0 to 4)
    int id = *(int*)num;
    int left_chopstick = id;
    // Ensure circular indexing 
    int right_chopstick = (id + 1) % NUM_PHILOSOPHERS;

    while (1) {
        // When the philosopher is thinking
        pthread_mutex_lock(&output_mutex);
        printf("P#%d THINKING.\n", id);
        pthread_mutex_unlock(&output_mutex);
        // Simulates thinking time (1 to 3 seconds)
        sleep(rand() % 3 + 1);

        // When the philosopher is picking up chopsticks
        if (id % 2 == 0) {
            // If philosopher is even pick up right chopstick first
            pthread_mutex_lock(&output_mutex);
            printf("P#%d picked up right chopstick.\n", id);
            pthread_mutex_unlock(&output_mutex);
            // sem_wait(&chopsticks[right_chopstick]):
            // Decrerment the semaphore. If semaphore value is > 0, 
            // decrement and proceed. If 0, philosopher blocks until 
            // the semaphore becomes available
            sem_wait(&chopsticks[right_chopstick]);

            pthread_mutex_lock(&output_mutex);
            printf("P#%d picked up left chopstick.\n", id);
            pthread_mutex_unlock(&output_mutex);
            sem_wait(&chopsticks[left_chopstick]);
        } else {
            // If phiosopher is odd pick up left chopstick first
            pthread_mutex_lock(&output_mutex);
            printf("P#%d picked up left chopstick.\n", id);
            pthread_mutex_unlock(&output_mutex);
            sem_wait(&chopsticks[left_chopstick]);

            pthread_mutex_lock(&output_mutex);
            printf("P#%d picked up right chopstick.\n", id);
            pthread_mutex_unlock(&output_mutex);
            sem_wait(&chopsticks[right_chopstick]);
        }

        // When the philosopher is eating
        pthread_mutex_lock(&output_mutex);
        printf("P#%d EATING.\n", id);
        pthread_mutex_unlock(&output_mutex);
        // Simulates eating time (1 to 2 seconds)
        sleep(rand() % 2 + 1);

        // When the philosopher is putting down chopsticks
        // sem_post($chopsticks[side]):
        // Increments the semaphore, signaling the chopstick 
        // is now available for others
        sem_post(&chopsticks[left_chopstick]);
        sem_post(&chopsticks[right_chopstick]);

        pthread_mutex_lock(&output_mutex);
        printf("P#%d put down left chopstick.\n", id);
        printf("P#%d put down right chopstick.\n", id);
        printf("P#%d finished eating and is thinking again.\n", id);
        pthread_mutex_unlock(&output_mutex);
    }
    return NULL;
}

int main() {
    // Array of thread identifiers for each philosopher
    pthread_t philosophers[NUM_PHILOSOPHERS];
    // Array storing each philosopher's unique ID from 0 to 4
    int ids[NUM_PHILOSOPHERS];

    for(int i = 0; i < NUM_PHILOSOPHERS; i++) {
        sem_init(&chopsticks[i], 0, 1);
        ids[i] = i;
    }
    pthread_mutex_init(&output_mutex, NULL);

    for(int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_create(&philosophers[i], NULL, philosopher, &ids[i]);
    }

    for(int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_join(philosophers[i], NULL);
    }

    for(int i = 0; i < NUM_PHILOSOPHERS; i++) {
        sem_destroy(&chopsticks[i]);
    }
    pthread_mutex_destroy(&output_mutex);

    return 0;
}