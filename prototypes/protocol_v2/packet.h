#pragma once

#include <sys/types.h>

#define PACKET_DATA_MAX_SIZE 63

typedef enum
{
    ACK = 0b000011,
    NACK = 0b000010,
    CD = 0b000110,
    LS = 0b000111,
    OK = 0b000001,
    ERROR = 0b010001,
    SHOW = 0b111111,
    ENDTX = 0b101110,
    MKDIR = 0b001000,
    GET = 0b001001,
    PUT = 0b001010,
    DATA = 0b100000,
    FDESC = 0b011000,
    EMPTY = 0b000000,
} packet_t;

typedef struct packet_options_t
{
    uint size : 6;
    packet_t type : 6;
    uint index : 4;
} packet_options_t;

int packet_send(int sockfd, void *data, packet_options_t options);

int packet_recv(int sockfd, void *data, packet_options_t *options);

int packet_ack(int sockfd, uint index);

int packet_nack(int sockfd, uint index);

int packet_ok(int sockfd, uint index);

int packet_error(int sockfd, int errcode, uint index);

int packet_end(int sockfd, uint index);

int packet_reset(packet_options_t *options);