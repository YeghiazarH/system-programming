#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 12345
#define BUFFER_SIZE 1024

int sock;

void* receive_messages(void* arg) {
    char buffer[BUFFER_SIZE];
    while (true) {
        int bytes = recv(sock, buffer, BUFFER_SIZE - 1, 0);
        if (bytes <= 0) break;
        buffer[bytes] = '\0';
        std::cout << buffer << "\n";
        std::cout.flush();
    }
    return nullptr;
}

int main() {
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "Socket creation failed\n";
        return 1;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // local server

    if (connect(sock, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Connection failed\n";
        return 1;
    }

    std::string name;
    std::cout << "Enter your username: ";
    std::getline(std::cin, name);
    send(sock, name.c_str(), name.size(), 0);

    pthread_t recv_thread;
    pthread_create(&recv_thread, nullptr, receive_messages, nullptr);
    pthread_detach(recv_thread);

    std::string msg;
    while (true) {
        std::getline(std::cin, msg);
        if (!msg.empty()) {
            send(sock, msg.c_str(), msg.size(), 0);
        }
        if (msg == "/exit") break;
    }

    close(sock);
    return 0;
}
