#include <iostream>
#include <vector>
#include <pthread.h>
#include <chrono>
#include <random>
#include <cstdlib>
#include <cerrno>
#include <cstring>

// Structure to pass data to thread function
struct ThreadData {
    const std::vector<int>* arr;
    size_t start;
    size_t end;
    long long result;
};

// Thread function to sum a portion of the array
void* sum_portion(void* arg) {
    ThreadData* data = static_cast<ThreadData*>(arg);
    long long local_sum = 0;

    for (size_t i = data->start; i < data->end; ++i) {
        local_sum += (*data->arr)[i];
    }

    data->result = local_sum;
    pthread_exit(nullptr);
}

int main(int argc, char* argv[]) {
    // Check command line arguments
    if (argc != 3) {
        std::cerr << "Error: Invalid number of arguments" << std::endl;
        std::cerr << "Usage: " << argv[0] << " <N> <M>" << std::endl;
        std::cerr << "  N: Number of elements in array (should be > 1000000)" << std::endl;
        std::cerr << "  M: Number of threads (should be > 0)" << std::endl;
        return 1;
    }

    // Validate N
    char* endptr;
    errno = 0;
    long long N_val = strtoll(argv[1], &endptr, 10);

    if (errno != 0 || *endptr != '\0' || N_val < 1000000) {
        std::cerr << "Error: N must be a valid integer greater than 1000000" << std::endl;
        return 1;
    }
    size_t N = static_cast<size_t>(N_val);

    // Validate M
    errno = 0;
    long M_val = strtol(argv[2], &endptr, 10);

    if (errno != 0 || *endptr != '\0' || M_val <= 0) {
        std::cerr << "Error: M must be a valid integer greater than 0" << std::endl;
        return 1;
    }
    int M = static_cast<int>(M_val);

    // Create and initialize array with random values
    std::vector<int> arr(N);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(1, 100);

    for (size_t i = 0; i < N; ++i) {
        arr[i] = dis(gen);
    }

    // Sum without threads
    auto start_time = std::chrono::high_resolution_clock::now();
    long long sum_single = 0;
    for (size_t i = 0; i < N; ++i) {
        sum_single += arr[i];
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration_single = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

    // Sum with threads
    start_time = std::chrono::high_resolution_clock::now();

    std::vector<pthread_t> threads(M);
    std::vector<ThreadData> thread_data(M);
    std::vector<bool> thread_created(M, false);

    size_t chunk_size = N / M;
    bool thread_creation_failed = false;
    int failed_thread_index = -1;

    // Create threads
    for (int i = 0; i < M; ++i) {
        size_t start = i * chunk_size;
        size_t end;

        // Last thread gets the remaining elements
        if (i == M - 1) {
            end = N;
        } else {
            end = start + chunk_size;
        }

        thread_data[i].arr = &arr;
        thread_data[i].start = start;
        thread_data[i].end = end;
        thread_data[i].result = 0;

        int ret = pthread_create(&threads[i], nullptr, sum_portion, &thread_data[i]);

        if (ret != 0) {
            std::cerr << "Error: Failed to create thread " << i << ": " << strerror(ret) << std::endl;
            thread_creation_failed = true;
            failed_thread_index = i;
            break;
        }

        thread_created[i] = true;
    }

    // Join all successfully created threads
    for (int i = 0; i < M; ++i) {
        if (thread_created[i]) {
            int ret = pthread_join(threads[i], nullptr);
            if (ret != 0) {
                std::cerr << "Error: Failed to join thread " << i << ": " << strerror(ret) << std::endl;
            }
        }
    }

    // If thread creation failed, handle the remaining work in main thread
    if (thread_creation_failed) {
        std::cerr << "Warning: Processing remaining work in main thread" << std::endl;

        for (int i = failed_thread_index; i < M; ++i) {
            long long local_sum = 0;
            size_t start = i * chunk_size;
            size_t end = (i == M - 1) ? N : start + chunk_size;

            for (size_t j = start; j < end; ++j) {
                local_sum += arr[j];
            }
            thread_data[i].result = local_sum;
        }
    }

    // Sum the partial results
    long long sum_multi = 0;
    for (int i = 0; i < M; ++i) {
        sum_multi += thread_data[i].result;
    }

    end_time = std::chrono::high_resolution_clock::now();
    auto duration_multi = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

    // Print results
    std::cout << "Time spent without threads: " << duration_single.count() << " microseconds" << std::endl;
    std::cout << "Time spent with " << M << " threads: " << duration_multi.count() << " microseconds" << std::endl;

    // Verify that both methods produce the same result
    if (sum_single != sum_multi) {
        std::cerr << "Warning: Sums don't match! Single: " << sum_single << ", Multi: " << sum_multi << std::endl;
        return 1;
    }

    return thread_creation_failed ? 1 : 0;
}