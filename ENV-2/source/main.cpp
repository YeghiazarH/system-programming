#include <iostream>
#include <vector>
#include "complex.h"
#include "sort.h"

using namespace std;

// demonstration of basic complex number operations
void demonstrateComplexOperations() {
    cout << "\nComplex Number Operations" << endl;

    Complex c1(3.0, 4.0);
    Complex c2(1.0, 2.0);
    Complex c3(5.0, -3.0);

    cout << "c1 = " << c1 << endl;
    cout << "c2 = " << c2 << endl;
    cout << "c3 = " << c3 << endl;

    cout << "\nBasic Operations:" << endl;
    cout << "c1 + c2 = " << (c1 + c2) << endl;
    cout << "c1 - c2 = " << (c1 - c2) << endl;
    cout << "c1 * 2.5 = " << (c1 * 2.5) << endl;

    cout << "\nAbsolute Values:" << endl;
    cout << "|c1| = " << c1.abs() << endl;
    cout << "|c2| = " << c2.abs() << endl;
    cout << "|c3| = " << c3.abs() << endl;

    cout << "\nComparisons:" << endl;
    cout << "c1 < c2: " << (c1 < c2 ? "true" : "false") << endl;
    cout << "c1 > c3: " << (c1 > c3 ? "true" : "false") << endl;
    cout << "c2 == c3: " << (c2 == c3 ? "true" : "false") << endl;
}

// demonstration of sort
void demonstrateSorting() {
    cout << "\nSorting" << endl;

    vector<Complex> numbers = {
        Complex(1.0, 1.0),    // |z| = √2 ≈ 1.414
        Complex(3.0, 4.0),    // |z| = 5.0
        Complex(0.0, 2.0),    // |z| = 2.0
        Complex(5.0, 0.0),    // |z| = 5.0
        Complex(-1.0, -1.0),  // |z| = √2 ≈ 1.414
        Complex(2.0, -1.0),   // |z| = √5 ≈ 2.236
        Complex(-3.0, 0.0),   // |z| = 3.0
        Complex(0.0, -4.0)    // |z| = 4.0
    };

    Sorting::printArray(numbers, "Original array");
    vector<Complex> mergeSortArray = numbers;
    Sorting::mergeSort(mergeSortArray);
    Sorting::printArray(mergeSortArray, "Sorted array");
}

int main() {
    demonstrateComplexOperations();
    demonstrateSorting();

    return 0;
}