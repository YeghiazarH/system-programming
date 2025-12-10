#include <iostream>
#include <vector>
#include <algorithm>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 12345
#define BUFFER_SIZE 1024

struct Client {
    int sock;
    std::string name;
};

std::vector<Client> clients;
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

// Broadcast message to all clients
void broadcast(const std::string& message, int sender_sock) {
    pthread_mutex_lock(&clients_mutex);
    for (auto &c : clients) {
        if (c.sock != sender_sock) {
            std::string msg_with_newline = message;
            if (msg_with_newline.back() != '\n') {
                msg_with_newline += "\n";
            }
            send(c.sock, msg_with_newline.c_str(), msg_with_newline.size(), 0);
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

// Function to handle each client
void* handle_client(void* arg) {
    int client_sock = *(int*)arg;
    char buffer[BUFFER_SIZE];
    std::string name;

    // Receive name
    int bytes_received = recv(client_sock, buffer, BUFFER_SIZE, 0);
    if (bytes_received <= 0) {
        close(client_sock);
        return nullptr;
    }
    buffer[bytes_received] = '\0';
    name = buffer;

    // Add client
    pthread_mutex_lock(&clients_mutex);
    clients.push_back({client_sock, name});
    pthread_mutex_unlock(&clients_mutex);

    std::string join_msg = name + " joined the chat.\n";
    broadcast(join_msg, client_sock);
    std::cout << join_msg;

    while (true) {
        bytes_received = recv(client_sock, buffer, BUFFER_SIZE, 0);
        if (bytes_received <= 0) break;
        buffer[bytes_received] = '\0';
        std::string msg(buffer);

        if (msg == "/exit") {
            break;
        } else if (msg == "/list") {
            pthread_mutex_lock(&clients_mutex);
            std::string user_list = "Online users: ";
            for (auto &c : clients) user_list += c.name + " ";
            user_list += "\n";
            send(client_sock, user_list.c_str(), user_list.size(), 0);
            pthread_mutex_unlock(&clients_mutex);
        } else {
            std::string full_msg = name + ": " + msg + "\n";
            broadcast(full_msg, client_sock);
            std::cout << full_msg;
        }
    }

    // Remove client
    pthread_mutex_lock(&clients_mutex);
    clients.erase(std::remove_if(clients.begin(), clients.end(),
        [client_sock](const Client& c){ return c.sock == client_sock; }),
        clients.end());
    pthread_mutex_unlock(&clients_mutex);

    std::string leave_msg = name + " left the chat.\n";
    broadcast(leave_msg, client_sock);
    std::cout << leave_msg;

    close(client_sock);
    return nullptr;
}

int main() {
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == -1) {
        std::cerr << "Socket creation failed\n";
        return 1;
    }

    int opt = 1;
    if (setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        std::cerr << "Set socket option failed\n";
        return 1;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_sock, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Bind failed\n";
        return 1;
    }

    if (listen(server_sock, 10) < 0) {
        std::cerr << "Listen failed\n";
        return 1;
    }

    std::cout << "Server started on port " << PORT << "\n";

    while (true) {
        sockaddr_in client_addr;
        socklen_t client_size = sizeof(client_addr);
        int client_sock = accept(server_sock, (sockaddr*)&client_addr, &client_size);
        if (client_sock < 0) {
            std::cerr << "Accept failed\n";
            continue;
        }

        pthread_t thread_id;
        int* pclient = new int;
        *pclient = client_sock;
        pthread_create(&thread_id, nullptr, handle_client, pclient);
        pthread_detach(thread_id);
    }

    close(server_sock);
    return 0;
}
