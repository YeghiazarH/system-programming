#include "shared_array.h"
#include <iostream>
#include <unistd.h>
#include <pthread.h>

int main(){
    shared_array arr("array-example", 10);
    sem_t *sem = arr.get_semaphore();

    while(1){
        sem_wait(sem);

        std::cout << "[second] read: ";
        for(size_t i=0;i<arr.size();++i){
            int v = arr[i];
            std::cout << v << " ";
            arr[i] = v + 100;
        }
        std::cout << "\n";

        sem_post(sem);

        sleep(2);
    }
    return 0;
}
