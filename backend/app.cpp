#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main() {
    char hostname[256];
    gethostname(hostname, sizeof(hostname));
    hostname[255] = '\0';

    // Create socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        std::cerr << "Failed to create socket\n";
        return 1;
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Bind to port 8080
    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "Failed to bind to port 8080\n";
        return 1;
    }

    if (listen(server_fd, 10) < 0) {
        std::cerr << "Failed to listen\n";
        return 1;
    }

    std::cout << "Server listening on port 8080 (hostname: "
              << hostname << ")" << std::endl;

    while (true) {
        int client_fd = accept(server_fd, nullptr, nullptr);
        if (client_fd < 0) {
            continue;
        }

        // -------- HTTP RESPONSE (BROWSER SAFE) --------
        std::string body = "Served by backend: ";
        body += hostname;
        body += "\n";

        std::string response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: " + std::to_string(body.size()) + "\r\n"
            "Connection: close\r\n"
            "\r\n" +
            body;

        send(client_fd, response.c_str(), response.size(), 0);
        close(client_fd);
    }

    return 0;
}
