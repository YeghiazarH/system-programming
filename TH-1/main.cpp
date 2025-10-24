#include <iostream>
#include <vector>
#include <pthread.h>
#include <chrono>
#include <random>
#include <cstdlib>
#include <cerrno>
#include <cstring>

struct ThreadData {
    const std::vector<int>* arr;
    size_t start;
    size_t end;
    long long result;
};

void* sum_portion(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    long long local_sum = 0;
    for (size_t i = data->start; i < data->end; ++i)
        local_sum += (*data->arr)[i];
    data->result = local_sum;
    pthread_exit(nullptr);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <N> <M>" << std::endl;
        return 1;
    }

    char* endptr;
    errno = 0;
    long long N_val = strtoll(argv[1], &endptr, 10);
    if (errno || *endptr || N_val < 1000000) {
        std::cerr << "Error: N must be > 1000000" << std::endl;
        return 1;
    }
    size_t N = (size_t)N_val;

    errno = 0;
    long M_val = strtol(argv[2], &endptr, 10);
    if (errno || *endptr || M_val <= 0) {
        std::cerr << "Error: M must be > 0" << std::endl;
        return 1;
    }
    int M = (int)M_val;

    std::vector<int> arr(N);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(1, 100);
    for (size_t i = 0; i < N; ++i)
        arr[i] = dis(gen);

    auto start_time = std::chrono::high_resolution_clock::now();
    long long sum_single = 0;
    for (size_t i = 0; i < N; ++i)
        sum_single += arr[i];
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration_single = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

    start_time = std::chrono::high_resolution_clock::now();
    std::vector<pthread_t> threads(M);
    std::vector<ThreadData> thread_data(M);
    std::vector<bool> created(M, false);

    size_t chunk = N / M;
    bool fail = false;
    int created_count = 0;

    for (int i = 0; i < M; ++i) {
        // You can safely add a break (as thread creation fail) here, it will be handled
        // Example:
        //if (i == 2) break;

        thread_data[i].arr = &arr;
        thread_data[i].start = i * chunk;
        thread_data[i].end = (i == M - 1) ? N : (i + 1) * chunk;
        thread_data[i].result = 0;

        int ret = pthread_create(&threads[i], nullptr, sum_portion, &thread_data[i]);
        if (ret != 0) {
            std::cerr << "Error: Failed to create thread " << i << ": " << strerror(ret) << std::endl;
            fail = true;
            break;
        }
        created[i] = true;
        created_count++;
    }

    // Detect if loop exited early (via break)
    if (created_count < M)
        fail = true;

    long long sum_multi = 0;
    if (fail) {
        std::cerr << "Thread creation failed. Falling back to main thread only." << std::endl;
        // cancel any created threads
        for (int j = 0; j < created_count; ++j)
            if (created[j]) pthread_cancel(threads[j]);
        // join them to clean up
        for (int j = 0; j < created_count; ++j)
            if (created[j]) pthread_join(threads[j], nullptr);
        // compute entire sum sequentially
        for (size_t i = 0; i < N; ++i)
            sum_multi += arr[i];
    } else {
        for (int i = 0; i < M; ++i)
            pthread_join(threads[i], nullptr);
        for (int i = 0; i < M; ++i)
            sum_multi += thread_data[i].result;
    }

    end_time = std::chrono::high_resolution_clock::now();
    auto duration_multi = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

    std::cout << "Time without threads: " << duration_single.count() << " us" << std::endl;
    if (!fail) std::cout << "Time with " << (fail ? 1 : M) << " thread(s): " << duration_multi.count() << " us" << std::endl;
    std::cout << "Sum: " << sum_multi << std::endl;

    if (sum_single != sum_multi)
        std::cerr << "Warning: Sums don't match! Single: " << sum_single << " Multi: " << sum_multi << std::endl;

    return fail ? 1 : 0;
}
