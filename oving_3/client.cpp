#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 8080

int main(int argc, char const* argv[]) {
    int client_fd, status, valread;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    // Create socket file descriptor
    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0) {
        std::cerr << "\n Socket creation error \n";
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);


    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "78.156.15.31", &serv_addr.sin_addr) <= 0) {
        std::cerr << "\nInvalid address/ Address not supported \n";
        return -1;
    }

    // Connect to the server
    status = connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (status < 0) {
        std::cerr << "\nConnection Failed \n";
        return -1;
    }

    while (true) {
        std::string equation;
        std::cout << "Enter a math equation (addition or subtraction) or type 'exit' to quit: ";
        std::getline(std::cin, equation); // Get user input

        if (equation == "exit") {
            break; // Exit the loop if user wants to quit
        }

        // Send the equation to the server
        send(client_fd, equation.c_str(), equation.length(), 0);
        std::cout << "Equation sent to server\n";

        // Receive the result from the server
        valread = read(client_fd, buffer, 1024 - 1);
        buffer[valread] = '\0'; // Ensure null-termination
        std::cout << "Answer received from server: " << buffer << std::endl;

        memset(buffer, 0, sizeof(buffer)); // Clear the buffer for the next message
    }

    // Close the socket
    close(client_fd);

    return 0;
}