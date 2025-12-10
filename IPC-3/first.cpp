#include "shared_array.h"
#include <iostream>
#include <unistd.h>
#include <pthread.h>

int main(){
    shared_array arr("array-example", 10);
    sem_t *sem = arr.get_semaphore();

    size_t idx = 0;
    int counter = 0;

    while(1){
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
