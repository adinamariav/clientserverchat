#include "CConnection.h"

CConnection::CConnection() {
    // Create a socket
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    int reuse = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse)) < 0) {
        perror("Setsockopt failed");
        exit(EXIT_FAILURE);
    }

    // Set up the server address struct
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(8080);

    // Bind the socket to the specified address and port
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }
}

void CConnection::acceptConnections() {
    if (listen(serverSocket, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    // Create an epoll instance
    if ((epoll_fd = epoll_create1(0)) < 0) {
        perror("Epoll creation failed");
        exit(EXIT_FAILURE);
    }

    // Add the server socket to the epoll set
    event.events = EPOLLIN;
    event.data.fd = serverSocket;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, serverSocket, &event) < 0) {
        perror("Epoll control error");
        exit(EXIT_FAILURE);
    }

    printf("Waiting for connections...\n");

    while (1) {
        // Wait for events on the epoll instance
        nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (nfds < 0) {
            perror("Epoll wait error");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < nfds; i++) {
            // Check for events on the server socket
            if (events[i].data.fd == serverSocket) {
                int newSocket;
                struct sockaddr_in clientAddr;
                socklen_t clientLen = sizeof(clientAddr);

                // Accept the incoming connection
                newSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientLen);
                if (newSocket < 0) {
                    perror("Accept failed");
                    continue;
                }

                pthread_t tid;
                if (pthread_create(&tid, nullptr, &CConnection::handleClient, (void *)&newSocket) != 0) {
                    perror("Failed to create thread");
                    close(newSocket);
                    continue;
                }

                // Detach the thread to allow it to run independently
                pthread_detach(tid);

                printf("New connection, socket fd is %d, ip is : %s, port : %d\n",
                       newSocket, inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
            }
        }
    }
}

void* CConnection::handleClient(void *arg) {
    int clientSocket = *((int *)arg);
    char buffer[MAX_BUFFER_SIZE];
    int bytesRead;

    // Handle the client connection
    while ((bytesRead = read(clientSocket, buffer, sizeof(buffer))) > 0) {
        buffer[bytesRead] = '\0';
        printf("Message received from client %d: %s\n", clientSocket, buffer);

        // Echo back the message to the client
        send(clientSocket, buffer, bytesRead, 0);
    }

    // Connection closed or error
    close(clientSocket);
    printf("Client %d disconnected\n", clientSocket);

    // Exit the thread
    pthread_exit(NULL);
}
