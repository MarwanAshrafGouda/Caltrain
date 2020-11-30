#include <pthread.h>
#include "caltrain.h"
#include <stdio.h>

void station_init(struct station *station) {
    station->waiting_passengers = 0;
    station->boarding_passengers = 0;
    station->train_seats = 0;
    pthread_mutex_init(&station->lock, NULL);
    pthread_cond_init(&station->train_arrived, NULL);
    pthread_cond_init(&station->ready_to_leave, NULL);
}

void station_load_train(struct station *station, int count) {
    station->train_seats = count;
    pthread_cond_broadcast(&station->train_arrived);
    fflush(stdout);
    while (station->train_seats && station->waiting_passengers) {
        pthread_cond_wait(&station->ready_to_leave, &station->lock);
    }
    station->train_seats = 0;
}

void station_wait_for_train(struct station *station) {
    pthread_mutex_lock(&station->lock);
    station->waiting_passengers++;
    pthread_mutex_unlock(&station->lock);
    printf("seats %d, boarding %d\n", station->train_seats, station->boarding_passengers);
    fflush(stdout);
    while (1) {
        while (!station->train_seats && station->boarding_passengers <= station->train_seats) {
            pthread_cond_wait(&station->train_arrived, &station->lock);
        }
        printf("passenger getting ready to board\n");
        fflush(stdout);
        pthread_mutex_lock(&station->lock);
        printf("testing lock\n");
        station->boarding_passengers++;
        if (station->boarding_passengers <= station->train_seats) {
            pthread_mutex_unlock(&station->lock);
            return;
        }
        pthread_mutex_unlock(&station->lock);
    }
}

void station_on_board(struct station *station) {
    pthread_mutex_lock(&station->lock);
    station->waiting_passengers--;
    station->train_seats--;
    printf("%d waiting passengers, %d seats left\n", station->waiting_passengers, station->train_seats);
    fflush(stdout);
    if (!(station->waiting_passengers && station->train_seats)) {
        pthread_mutex_unlock(&station->lock);
        pthread_cond_signal(&station->ready_to_leave);
        return;
    }
    pthread_mutex_unlock(&station->lock);
}