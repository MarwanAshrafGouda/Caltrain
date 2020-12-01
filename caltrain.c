#include <pthread.h>
#include "caltrain.h"
#include <stdio.h>

void station_init(struct station *station) {
    station->waiting_passengers = 0;
    station->train_seats = 0;
    station->boarding_counter = 0;
    pthread_mutex_init(&station->lock, NULL);
    pthread_cond_init(&station->train_arrived, NULL);
    pthread_cond_init(&station->ready_to_leave, NULL);
}

void station_load_train(struct station *station, int count) {
    station->train_seats = count;
    station->reserved = count;
    if (count && station->waiting_passengers)
        pthread_cond_broadcast(&station->train_arrived);
    while (station->boarding_counter < station->reserved && station->waiting_passengers) {
        pthread_cond_wait(&station->ready_to_leave, &station->lock);
    }
    station->train_seats = 0;
    station->boarding_counter = 0;
}

void station_wait_for_train(struct station *station) {
    pthread_mutex_lock(&station->lock);
    station->waiting_passengers++;
    while (!station->train_seats && station->waiting_passengers) {
        pthread_cond_wait(&station->train_arrived, &station->lock);
    }
    if (station->train_seats > 0) {
        station->train_seats--;
    }
    pthread_mutex_unlock(&station->lock);
}

void station_on_board(struct station *station) {
    pthread_mutex_lock(&station->lock);
    station->waiting_passengers--;
    station->boarding_counter++;
    if (station->reserved == station->boarding_counter || !station->waiting_passengers) {
        pthread_cond_broadcast(&station->ready_to_leave);
    }
    pthread_mutex_unlock(&station->lock);
}