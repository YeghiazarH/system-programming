#include <iostream>

bool isValidInteger(const std::string& str) {
    if (str.empty()) return false;
    size_t start = 0;
    if (str[0] == '+' || str[0] == '-') {
        if (str.length() == 1) return false;
        start = 1;
    }
    
    for (size_t i = start; i < str.length(); ++i) {
        if (!std::isdigit(str[i])) {
            return false;
        }
    }
    return true;
}

int main(int argc, char* argv[]) {
    if(argc != 4) {
        std::cout << "Usage: ./a.out a b c\n";
        return 1;
    }

    if (!isValidInteger(argv[1]) || !isValidInteger(argv[2]) || !isValidInteger(argv[3])) {
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
