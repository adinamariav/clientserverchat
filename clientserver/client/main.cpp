#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_BUFFER_SIZE 1024

int main() {
    int clientSocket;
    struct sockaddr_in serverAddr;
    char buffer[MAX_BUFFER_SIZE];

    // Create a socket
    if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set up the server address struct
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Server IP address
    serverAddr.sin_port = htons(8080); // Server port

    // Connect to the server
    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    // Send a message to the server
    char message[] = "Hello, server!";
    send(clientSocket, message, strlen(message), 0);
    printf("Message sent to the server: %s\n", message);

    // Receive the echoed message from the server
    int bytesRead = read(clientSocket, buffer, sizeof(buffer));
    if (bytesRead < 0) {
        perror("Read error");
        exit(EXIT_FAILURE);
    }

    buffer[bytesRead] = '\0';
    printf("Message received from the server: %s\n", buffer);

    // Close the socket
    close(clientSocket);

    return 0;
}