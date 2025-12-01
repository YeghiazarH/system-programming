#include <iostream>
#include <unistd.h>
#include <limits>

bool isPrime(int n) {
    if(n < 2) return false;
    for(int i = 2; i * i <= n; i++) {
        if(n % i == 0) return false;
    }
    return true;
}

int nthPrime(int m) {
    int count = 0;
    int num = 1;
    while(count < m) {
        num++;
        if(isPrime(num)) count++;
    }
    return num;
}

bool validNumber(const std::string& s){
    for(char c : s){
        if(c < '0' || c > '9') return false;
    }
    return !s.empty();
}

int main(){
    int p2c[2];
    int c2p[2];

    pipe(p2c);
    pipe(c2p);

    pid_t pid = fork();

    if(pid < 0) {
        std::cerr<<"Forking failed";
        return 1;
    }

    if(pid == 0) {
        close(p2c[1]);
        close(c2p[0]);

        while(true) {
            int m;
            int r = read(p2c[0], &m, sizeof(m));
            if(r <= 0) break;
            if(m <= 0) break;

            std::cout << "[Child] Calculating " << m << "-th prime number..." << std::endl;
            int prime = nthPrime(m);
            std::cout << "[Child] Sending calculation result of prime(" << m << ")..." << std::endl;
            write(c2p[1], &prime, sizeof(prime));
        }

        close(p2c[0]);
        close(c2p[1]);
    } 
    else {
        close(p2c[0]);
        close(c2p[1]);

        while(true) {
            std::cout << "[Parent] Please enter the number: ";
            std::string input;
            std::cin >> input;

            if(input == "exit") {
                int stop = 0;
                write(p2c[1], &stop, sizeof(stop));
                break;
            }
            if(!validNumber(input)){
                std::cout<<"Only positive numbers allowed!\n";
                continue;
            }

            int m = std::stoi(input);

            if(m <= 0){
                std::cout<<"Number must be > 0\n";
                continue;
            }

            if(m > 100000){
                std::cout<<"Too big number! Max = 100000\n";
                continue;
            }

            std::cout << "[Parent] Sending " << m << " to the child process..." << std::endl;

            int w = write(p2c[1], &m, sizeof(m));
            if(w <= 0){
                std::cerr<<"Parent: write failed\n";
                break;
            }
            
            std::cout << "[Parent] Waiting for the response from the child process..." << std::endl;

            int result;
            int r = read(c2p[0], &result, sizeof(result));
                if(r <= 0){
                std::cerr<<"Error while reading from child\n";
                break;
            }

            std::cout << "[Parent] Received calculation result of prime(" << m << ") = " << result << "..." << std::endl;
        }
        close(p2c[1]);
        close(c2p[0]);
    }

    return 0;
}

