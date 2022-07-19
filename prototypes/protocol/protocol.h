#pragma once

#include <sys/types.h>

typedef enum packet_type_t
{
    ACK = 0b000011,
    NACK = 0b000010,
    CD = 0b000110,
    OK = 0b000001,
    ERROR = 0b010001,
    SHOW = 0b111111,
    ENDTX = 0b101110,
} packet_type_t;

int protocol_send(int sockfd, void *data, size_t size, packet_type_t type);

// TODO: definir implementação melhor
int protocol_recv(int sockfd, void *data, size_t size);
