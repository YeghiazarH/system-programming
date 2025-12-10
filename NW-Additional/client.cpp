#include <iostream>
#include <fstream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h> //for gethostbyname
#include <unistd.h>

int main() {
    const char* host = "httpforever.com";
    const int port = 80;

    // Get server IP address
    struct hostent* he = gethostbyname(host);
    if (!he) {
        std::cerr << "Can not get host\n";
        return 1;
    }

    char ip[100];
    inet_ntop(AF_INET, he->h_addr_list[0], ip, sizeof(ip)); //ip to str
    std::cout << "IP адрес: " << ip << "\n";

    // Create socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "Can not create socket\n";
        return 1;
    }

    // Configure server address
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &server_addr.sin_addr); //str to binary  

    // Connect to server
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Can not connect to server\n";
        close(sock);
        return 1;
    }

    // Build HTTP request
    const char* request = "GET / HTTP/1.1\r\nHost: httpforever.com\r\nConnection: close\r\n\r\n";
    send(sock, request, strlen(request), 0);

    // Receive response
    char buffer[4096];
    std::ofstream outfile("httpforever.html", std::ios::binary);
    int bytes_received;
    while ((bytes_received = recv(sock, buffer, sizeof(buffer), 0)) > 0) {
        outfile.write(buffer, bytes_received);
    }

    outfile.close();
    close(sock);

    std::cout << "Server response saved to httpforever.html\n";
    return 0;
}
