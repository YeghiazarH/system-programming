#include <iostream>

int main(int argc, char* argv[]) {
    if(argc < 2) {
        std::cout << "No arguments provided\n";
        return 1;
    }

    std::cout << "Arguments in reverse:\n";
    for(int i = argc - 1; i >= 1; --i) {
        std::cout << argv[i] << " ";
    }
    std::cout << "\n";

    return 0;
}
