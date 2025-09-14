#include <iostream>

int main(int argc, char* argv[]) {
    if(argc != 4) {
        std::cout << "Usage: ./a.out a b c\n";
        return 1;
    }

    int a = std::stoi(argv[1]);
    int b = std::stoi(argv[2]);
    int c = std::stoi(argv[3]);

    int min = a;
    if(b < min) min = b;
    if(c < min) min = c;

    int max = a;
    if(b > max) max = b;
    if(c > max) max = c;

    std::cout << "min-" << min << ", max-" << max << "\n";

    return 0;
}

