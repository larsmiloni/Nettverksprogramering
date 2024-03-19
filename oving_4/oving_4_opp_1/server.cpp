#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include <sstream>
#include <iostream>
#include <regex>

int calculate(const std::string& equation, bool& isError) {
    int num1, num2;
    char op;
    std::istringstream iss(equation);127.0.0.1
    std::smatch match;
    std::regex valid_expr("^\\s*(-?\\d+)\\s*([+-])\\s*(-?\\d+)\\s*$"); // Regex to validate expression

    isError = true; // Assume error by default

    // Check if the equation matches the valid expression format
    if (std::regex_match(equation, match, valid_expr)) {
        // Extract numbers and operator
        num1 = std::stoi(match.str(1));
        op = match.str(2)[0];
        num2 = std::stoi(match.str(3));

        // Reset error flag as the expression is valid
        isError = false;

        // Perform calculation based on the operator
        switch (op) {
            case '+': return num1 + num2;
            case '-': return num1 - num2;
        }
    }

    // If we reach here, the expression didn't match the expected format
    return 0; // Return value is irrelevant when isError is true
}


void handle_client(int server_fd) {
    char buffer[1024] = {0};
    struct sockaddr_in cli_addr;
    socklen_t cli_len = sizeof(cli_addr);

    while (true) {
        memset(buffer, 0, sizeof(buffer));
        ssize_t valread = recvfrom(server_fd, buffer, 1024, 0, (struct sockaddr*)&cli_addr, &cli_len);
        if (valread <= 0) continue; // Error in receiving data

        std::string equation(buffer);
        std::cout << "Equation received: " << equation << std::endl;

        bool isError;
        int result = calculate(equation, isError);
        std::string response = isError ? "Error: Invalid equation" : std::to_string(result);

        sendto(server_fd, response.c_str(), response.length(), 0, (struct sockaddr*)&cli_addr, cli_len);
        std::cout << "Response sent back to client\n";
    }
}

int main() {
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;

    server_fd = socket(AF_INET, SOCK_DGRAM, 0);
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

    std::cout << "Waiting for equation..." << std::endl; // Print message indicating server is waiting for input

    handle_client(server_fd); // In UDP, no listen or accept calls are needed

    close(server_fd);

    return 0;
}

