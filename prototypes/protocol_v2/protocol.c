#include <string.h>

#include "error.h"
#include "protocol.h"
#include "raw_socket.h"

#define PACKET_START_MARKER 0b01111110
#define PACKET_DATA_MAX_SIZE 64

typedef struct envelope_t
{
    uint start_marker : 8;
    // uint size : 6;
    // uint sequence : 4;
    // uint type : 6;
    packet_config_t config;
    uint parity : 8;
} envelope_t;

const size_t PACKET_MAX_SIZE = sizeof(envelope_t) + PACKET_DATA_MAX_SIZE;

uint calc_parity(void *data, size_t size)
{
    return 0;
}

int packet_send(int sockfd, void *data, packet_config_t config)
{
    const size_t buf_size = config.size + sizeof(envelope_t) < 14 ? 14 : config.size + sizeof(envelope_t);
    char buf[buf_size];

    envelope_t env;
    env.start_marker = PACKET_START_MARKER;
    env.config = config;
    env.parity = calc_parity(data, config.size);

    memcpy(buf, &env, sizeof(envelope_t) - 1);
    memcpy(buf + sizeof(envelope_t) - 1, data, config.size);
    buf[sizeof(envelope_t) - 1 + config.size] = env.parity;

    TRY(rs_send(sockfd, buf, buf_size));

    return 0;
}

int packet_recv(int sockfd, void *data, packet_config_t *config)
{
    return 0;
}