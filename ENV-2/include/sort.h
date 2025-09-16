#ifndef SORT_H
#define SORT_H

#include <vector>
#include "complex.h"

namespace Sorting {
    void mergeSort(std::vector<Complex>& arr, int left, int right);
    void mergeSort(std::vector<Complex>& arr);
    void printArray(const std::vector<Complex>& arr, const std::string& title = "Array");
    bool isSorted(const std::vector<Complex>& arr);
}

#endif // SORT_H