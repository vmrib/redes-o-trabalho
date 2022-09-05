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
    char parity = 0;
    char *string = (char *)data;
    for (int i = 0; i < size; i++)
    {
        parity ^= string[i];
    }
    // printf("%c\n", parity);
    return parity;
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

    // for (size_t i = 0; i < buf_size; i++)
    // {
    //     printf("buf[%lu]: 0x%X\n", i, buf[i]);
    // }

    TRY(rs_send(sockfd, buf, buf_size));

    return RETURN_SUCCESS;
}

int packet_recv(int sockfd, void *data, packet_options_t *options)
{
    envelope_t env;
    char buf[PACKET_DATA_MAX_SIZE];
    // printf("buf: %s\n", buf);
#ifndef NDEBUG
    rs_recv(sockfd, buf, PACKET_DATA_MAX_SIZE); // descarta echo do loopback
    // printf("buf: %s\n", buf);
#endif

    rs_recv(sockfd, buf, PACKET_DATA_MAX_SIZE);
    // printf("buf: %s\n", buf);

    // for (size_t i = 0; i < PACKET_DATA_MAX_SIZE; i++)
    // {
    //     printf("buf[%lu]: 0x%X\n", i, buf[i]);
    // }

    memcpy(&env, buf, sizeof(envelope_t) - 1);
    if (env.start_marker != PACKET_START_MARKER)
    {
        // debug((uint)env.start_marker);
        // printf("IF 1\n");
        return RETURN_ERROR;
    }

    env.parity = buf[env.size + sizeof(envelope_t) - 1];
    if (env.parity != calc_parity(buf + sizeof(envelope_t) - 1, env.size))
    {
        // debug((uint)env.parity);
        // printf("IF 2\n");
        return RETURN_ERROR;
    }

    memcpy(data, buf + sizeof(envelope_t) - 1, env.size);
    options->size = env.size;
    options->type = env.type;
    options->index = env.sequence;

    return RETURN_SUCCESS;
}

int packet_ack(int sockfd, uint index)
{
    packet_options_t opt = {
        .index = index,
        .size = 0,
        .type = ACK,
    };
    TRY(packet_send(sockfd, NULL, opt));

    return RETURN_SUCCESS;
}

int packet_nack(int sockfd, uint index)
{
    packet_options_t opt = {
        .index = index,
        .size = 0,
        .type = NACK,
    };
    TRY(packet_send(sockfd, NULL, opt));

    return RETURN_SUCCESS;
}

int packet_ok(int sockfd, uint index)
{
    packet_options_t opt = {
        .index = index,
        .size = 0,
        .type = OK,
    };
    TRY(packet_send(sockfd, NULL, opt));

    return RETURN_SUCCESS;
}

int packet_error(int sockfd, char *description, uint index)
{
    packet_options_t opt = {
        .index = index,
        .size = strlen(description) + 1,
        .type = ERROR,
    };
    TRY(packet_send(sockfd, description, opt));

    return RETURN_SUCCESS;
}

int packet_end(int sockfd, uint index)
{
    packet_options_t opt = {
        .index = index,
        .size = 0,
        .type = ENDTX,
    };
    TRY(packet_send(sockfd, NULL, opt));

    return RETURN_SUCCESS;
}

int packet_reset(packet_options_t *opt)
{
    opt->type = EMPTY;
    return 0;
}