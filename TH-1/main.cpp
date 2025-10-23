
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <random>
#include <cstdlib>

// Function to sum a portion of the array
void sum_portion(const std::vector<int>& arr, size_t start, size_t end, long long& result) {
    long long local_sum = 0;
    for (size_t i = start; i < end; ++i) {
        local_sum += arr[i];
    }
    result = local_sum;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <N> <M>" << std::endl;
        std::cerr << "  N: Number of elements in array" << std::endl;
        std::cerr << "  M: Number of threads" << std::endl;
        return 1;
    }

    size_t N = std::atoll(argv[1]);
    int M = std::atoi(argv[2]);

    if (N <= 1000000) {
        std::cerr << "Error: N must be greater than 1000000" << std::endl;
        return 1;
    }

    if (M <= 0) {
        std::cerr << "Error: M must be greater than 0" << std::endl;
        return 1;
    }


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

    std::vector<std::thread> threads;
    std::vector<long long> partial_sums(M, 0);

    size_t chunk_size = N / M;

    for (int i = 0; i < M; ++i) {
        size_t start = i * chunk_size;
        size_t end;

        // Last thread gets the remaining elements
        if (i == M - 1) {
            end = N;
        } else {
            end = start + chunk_size;
        }

        threads.emplace_back(sum_portion, std::cref(arr), start, end, std::ref(partial_sums[i]));
    }

    // Wait for threads
    for (auto& thread : threads) {
        thread.join();
    }

    long long sum_multi = 0;
    for (int i = 0; i < M; ++i) {
        sum_multi += partial_sums[i];
    }

    end_time = std::chrono::high_resolution_clock::now();
    auto duration_multi = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

    std::cout << "Time spent without threads: " << duration_single.count() << " microseconds" << std::endl;
    std::cout << "Time spent with " << M << " threads: " << duration_multi.count() << " microseconds" << std::endl;

    if (sum_single != sum_multi) {
        std::cerr << "Warning: Sums don't match! Single: " << sum_single << ", Multi: " << sum_multi << std::endl;
    }

    return 0;
}