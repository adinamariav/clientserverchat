//
// Created by adina on 12/18/23.
//

#ifndef P2P_MESSAGE_H
#define P2P_MESSAGE_H

typedef struct message_header_t {
    char type;
    char error;
    unsigned int room;
    unsigned int payload_length;
} message_header;

typedef struct packet_t {
    struct message_header_t header;
    char payload[1000];
} packet;

#endif //P2P_MESSAGE_H
