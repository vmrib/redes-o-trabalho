#include <string.h>
#include "protocol.h"
#include "raw_socket.h"
#include "debug.h"

#define PACKET_START_MARKER 0b01111110
#define PACKET_DATA_MAX_SIZE 64

typedef struct packet_envelope_t
{
    uint start_marker : 8;
    uint size : 6;
    uint sequence : 4;
    uint type : 6;
    uint parity : 8;
} packet_envelope_t;

// typedef struct packet_t
// {
//     packet_header_t header;
//     void *data;
//     packet_footer_t footer;
// } packet_t;

const size_t PACKET_MAX_SIZE = sizeof(packet_envelope_t) + PACKET_DATA_MAX_SIZE;

uint calc_parity(void *data, size_t size)
{
    return 0;
}

static packet_envelope_t make_envelope(void *data, size_t size, uint sequence, packet_type_t type)
{
    packet_envelope_t env;

    env.start_marker = PACKET_START_MARKER;
    env.size = size;
    env.sequence = sequence;
    env.type = type;
    env.parity = calc_parity(data, size);

    return env;
}

int protocol_send(int sockfd, void *data, size_t size, packet_type_t type)
{
    if (size > PACKET_DATA_MAX_SIZE)
        return -1;

    const size_t buf_size = size + sizeof(packet_envelope_t) < 14 ? 14 : size + sizeof(packet_envelope_t);

    char buf[buf_size];

    packet_envelope_t env = make_envelope(data, size, 0, type);

    // debug(sizeof(packet_header_t));
    // debug(sizeof(packet_footer_t));
    debug(sizeof(packet_envelope_t));

    // debug((uint)packet.header.start_marker, (uint)packet.header.size, (uint)packet.header.sequence, (uint)packet.header.type);

    memcpy(buf, &env, sizeof(packet_envelope_t) - 1);
    memcpy(buf + sizeof(packet_envelope_t) - 1, data, size);
    buf[buf_size - 1] = env.parity;

    for (size_t i = 0; i < size + sizeof(packet_envelope_t); i++)
    {
        printf("buf[%lu]: 0x%X\n", i, buf[i]);
    }

    debug(buf_size);

    rs_send(sockfd, buf, buf_size);

    return 0;
}

int protocol_recv(int sockfd, void *data, size_t size)
{
    packet_envelope_t env;
    char buf[PACKET_DATA_MAX_SIZE];

    // debug("packet header:", (uint)packet.header.start_marker, (uint)packet.header.size, (uint)packet.header.sequence);

    rs_recv(sockfd, buf, PACKET_DATA_MAX_SIZE);

    memcpy(&env, buf, sizeof(packet_envelope_t) - 1);
    if (env.start_marker != PACKET_START_MARKER)
        return -1;

    env.parity = buf[env.size + sizeof(packet_envelope_t) - 1];
    memcpy(data, buf + sizeof(packet_envelope_t) - 1, env.size);

    if (env.parity != calc_parity(data, size))
        return -1;

    return 0;

    // memcpy(data, buf, packet.header.size);
}
