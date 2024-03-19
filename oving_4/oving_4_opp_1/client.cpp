#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 8080

int main() {
    int client_fd;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    // Create socket file descriptor for UDP
    client_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_fd < 0) {
        std::cerr << "\n Socket creation error \n";
        return -1;
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = INADDR_ANY; // If server is on the same machine

    // Convert IPv4 addresses from text to binary form
    if (inet_pton(AF_INET, "192.168.0.162", &serv_addr.sin_addr) <= 0) {
        std::cerr << "\nInvalid address/ Address not supported \n";
        return -1;
    }

    while (true) {
        std::string equation;
        std::cout << "Enter a math equation (addition or subtraction) or type 'exit' to quit: ";
        std::getline(std::cin, equation); // Get user input

        if (equation == "exit") {
            break; // Exit the loop if user wants to quit
        }

        // Send the equation to the server using sendto
        sendto(client_fd, equation.c_str(), equation.length(), 0, (const struct sockaddr*)&serv_addr, sizeof(serv_addr));
        std::cout << "Equation sent to server\n";

        // Receive the result from the server using recvfrom
        socklen_t len = sizeof(serv_addr);
        ssize_t valread = recvfrom(client_fd, buffer, 1024, 0, (struct sockaddr*)&serv_addr, &len);
        buffer[valread] = '\0'; // Ensure null-termination
        std::cout << "Answer received from server: " << buffer << std::endl;

        memset(buffer, 0, sizeof(buffer)); // Clear the buffer for the next message
    }

    // Close the socket
    close(client_fd);

    return 0;
}
