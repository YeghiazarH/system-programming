#include "shared_array.h"
#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

volatile sig_atomic_t stopflag = 0;
void onint(int){ stopflag = 1; }

int main(){
    signal(SIGINT, onint);

    shared_array arr("array-example", 10);
    sem_t *sem = arr.get_semaphore();

    size_t idx = 0;
    int counter = 0;

    while(!stopflag){
        sem_wait(sem);
        arr[idx] = counter;
        std::cout << "[first] arr[" << idx << "]=" << counter << "\n";
        sem_post(sem);

        idx = (idx + 1) % arr.size();
        counter++;
        sleep(1);
    }
    return 0;
}
