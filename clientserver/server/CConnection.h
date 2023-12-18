#ifndef CLIENTSERVERCHAT_CCONNECTION_H
#define CLIENTSERVERCHAT_CCONNECTION_H

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <pthread.h>


#define MAX_EVENTS 10
#define MAX_BUFFER_SIZE 1024


class CConnection {
    int serverSocket, epoll_fd, nfds;
    struct epoll_event event, events[MAX_EVENTS];

    struct sockaddr_in serverAddr;
    char buffer[MAX_BUFFER_SIZE];

    static void* handleClient(void *arg);

public:
    CConnection();
    void acceptConnections ();
};


#endif //CLIENTSERVERCHAT_CCONNECTION_H
