#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <sstream>

#define PORT 8080

void startServer(int server_fd);
std::string processClientRequest(int client_socket);

int main() {
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);

    // Oppretter socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Angi socket alternativer
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    how do i add dialout:x:20:larsmiloni to /etc/group file // Automatisk velge IP
    address.sin_port = htons(PORT);

    // Knytter socket til porten
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Starter å lytte etter klienter
    if (listen(server_fd, 1) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    std::cout << "Serveren kjører og lytter på port " << PORT << std::endl;
    startServer(server_fd);

    return 0;
}

void startServer(int server_fd) {
    int new_socket;
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);
    
    while(true) {
        std::cout << "Venter på ny forbindelse..." << std::endl;
        
        if ((new_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen))<0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        std::string response = processClientRequest(new_socket);
        write(new_socket, response.c_str(), response.length());
        std::cout << "Svar sendt til klient." << std::endl;

        close(new_socket);
    }
}

std::string processClientRequest(int client_socket) {
    char buffer[1024] = {0};
    read(client_socket, buffer, 1024);

    std::stringstream headerStream;
    headerStream << buffer;

    std::string line;
    std::string headersHTML = "<UL>";
    while (getline(headerStream, line) && line != "\r") {
        headersHTML += "<LI>" + line + "</LI>";
    }
    headersHTML += "</UL>";

    std::string httpResponse = 
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html; charset=utf-8\r\n"
        "Connection: close\r\n"
        "<HTML><BODY>"
        "<H1>Hilsen. Du har koblet deg opp til min enkle web-tjener</H1>"
        "Header fra klient er: " + headersHTML +
        "</BODY></HTML>";



    return httpResponse;
}
