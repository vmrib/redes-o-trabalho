#include <string.h>

#include "debug.h"
#include "error.h"
#include "packet.h"
#include "raw_socket.h"

#define PACKET_START_MARKER 0b01111110

typedef struct envelope_t
{
    uint start_marker : 8;
    uint size : 6;
    uint sequence : 4;
    uint type : 6;
    // packet_options_t config;
    uint parity : 8;
} envelope_t;

const size_t PACKET_MAX_SIZE = sizeof(envelope_t) + PACKET_DATA_MAX_SIZE;

uint calc_parity(void *data, size_t size)
{
    return 0;
}

int packet_send(int sockfd, void *data, packet_options_t options)
{
    const size_t buf_size = options.size + sizeof(envelope_t) < 14 ? 14 : options.size + sizeof(envelope_t);
    char buf[buf_size];

    envelope_t env;
    env.start_marker = PACKET_START_MARKER;
    env.size = options.size;
    env.sequence = options.index;
    env.type = options.type;
    env.parity = calc_parity(data, options.size);

    memcpy(buf, &env, sizeof(envelope_t) - 1);
    memcpy(buf + sizeof(envelope_t) - 1, data, options.size);
    buf[sizeof(envelope_t) - 1 + options.size] = env.parity;

    TRY(rs_send(sockfd, buf, buf_size));

    return RETURN_SUCCESS;
}

int packet_recv(int sockfd, void *data, packet_options_t *options)
{
    envelope_t env;
    char buf[PACKET_DATA_MAX_SIZE];

    TRY(rs_recv(sockfd, data, PACKET_DATA_MAX_SIZE));

    memcpy(&env, buf, sizeof(envelope_t) - 1);
    if (env.start_marker != PACKET_START_MARKER)
        return RETURN_ERROR;

    env.parity = buf[env.size + sizeof(envelope_t) - 1];
    if (env.parity != calc_parity(buf + sizeof(envelope_t) - 1, env.size))
        return RETURN_ERROR;

    memcpy(data, buf + sizeof(envelope_t) - 1, env.size);

    return RETURN_SUCCESS;
}