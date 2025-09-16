#include "sort.h"
#include <iostream>
#include <string>

namespace Sorting {

    void merge(std::vector<Complex>& arr, int left, int mid, int right) {
        int n1 = mid - left + 1;
        int n2 = right - mid;
        
        //temp arrays
        std::vector<Complex> leftArr(n1);
        std::vector<Complex> rightArr(n2);
        
        for (int i = 0; i < n1; i++)
            leftArr[i] = arr[left + i];
        for (int j = 0; j < n2; j++)
            rightArr[j] = arr[mid + 1 + j];
        
        // Merge back
        int i = 0, j = 0, k = left;
        while (i < n1 && j < n2) {
            if (leftArr[i] <= rightArr[j]) {
                arr[k] = leftArr[i];
                i++;
            } else {
                arr[k] = rightArr[j];
                j++;
            }
            k++;
        }
        
        //elements that left after merge
        while (i < n1) {
            arr[k] = leftArr[i];
            i++;
            k++;
        }
        
        while (j < n2) {
            arr[k] = rightArr[j];
            j++;
            k++;
        }
    }

    void mergeSort(std::vector<Complex>& arr, int left, int right) {
        if (left < right) {
            int mid = left + (right - left) / 2;

            mergeSort(arr, left, mid);
            mergeSort(arr, mid + 1, right);
            merge(arr, left, mid, right);
        }
    }

    void mergeSort(std::vector<Complex>& arr) {
        if (!arr.empty()) {
            mergeSort(arr, 0, arr.size() - 1);
        }
    }
    
    //print func
    void printArray(const std::vector<Complex>& arr, const std::string& title) {
        std::cout << title << ": [";
        for (size_t i = 0; i < arr.size(); i++) {
            std::cout << arr[i];
            if (i < arr.size() - 1) {
                std::cout << ", ";
            }
        }
        std::cout << "]" << std::endl;
    }
    
    //func to check if array is sorted
    bool isSorted(const std::vector<Complex>& arr) {
        for (size_t i = 1; i < arr.size(); i++) {
            if (arr[i] < arr[i - 1]) {
                return false;
            }
        } return true;
    }
}