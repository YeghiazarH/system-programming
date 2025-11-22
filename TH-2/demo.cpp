#include "parallel_scheduler.h"
#include <iostream>
#include <unistd.h>

using namespace std;

void exampleTask(void* arg)
{
    int id = *static_cast<int*>(arg);
    cout << id << "-st task started\n";
    sleep(1);
    cout << id << "-st task finished\n";
}

int main()
{
    parallel_scheduler sch(5);

    int ids[10];
    for (int i = 0; i < 10; i++) {
        ids[i] = i + 1;
        sch.run(exampleTask, &ids[i]);
    }

    sleep(10);
    return 0;
}
