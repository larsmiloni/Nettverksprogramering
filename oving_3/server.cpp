#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include <sstream>
#include <iostream>
#include <thread> // Include for threading support

// Updated function to calculate the result of the equation and indicate errors
int calculate(const std::string& equation, bool& isError) {
    int num1, num2;
    char op;
    std::istringstream iss(equation);
    iss >> num1 >> op >> num2;

    isError = false; // Reset error flag

    switch (op) {
        case '+': return num1 + num2;
        case '-': return num1 - num2;
        default:
            isError = true; // Set error flag
            return 0; // Return value is irrelevant when isError is true
    }
}

// Thread function to handle client connection
void handle_client(int client_socket) {
    char buffer[1024] = {0};
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        ssize_t valread = read(client_socket, buffer, 1024 - 1);
        if (valread <= 0) break; // Client closed connection or read error

        std::string equation(buffer);
        std::cout << "Equation received: " << equation << std::endl;

        bool isError;
        int result = calculate(equation, isError);
        std::string response;
        if (isError) {
            response = "Error: Invalid equation";
        } else {
            response = std::to_string(result);
        }

        send(client_socket, response.c_str(), response.length(), 0);
        std::cout << "Response sent back to client\n";
    }
    close(client_socket);
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);
    

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    while (true) {
        std::cout << "Waiting for new connection..." << std::endl;
        new_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen);
        if (new_socket < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        std::thread client_thread(handle_client, new_socket);
        client_thread.detach(); // Detach the thread to allow it to run independently
    }

    // Close the listening socket (optional, as this point will never be reached in the current setup)
    close(server_fd);

    return 0;
}