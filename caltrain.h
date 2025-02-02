#include <pthread.h>

struct station {
    int waiting_passengers;
    int train_seats;
    int reserved;
    int boarding_counter;
    pthread_mutex_t lock;
    pthread_cond_t train_arrived;
    pthread_cond_t ready_to_leave;
};

void station_init(struct station *station);

void station_load_train(struct station *station, int count);

void station_wait_for_train(struct station *station);

void station_on_board(struct station *station);