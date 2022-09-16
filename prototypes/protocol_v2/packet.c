#include <string.h>
#include <stdio.h>

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

u_int8_t calc_parity(void *data, size_t size)
{
    char parity = 0;
    char *string = (char *)data;
    for (int i = 0; i < size; i++)
    {
        parity ^= string[i];
        // printf("paridade ^= 0x%X\n", string[i]);
    }
    // printf("paridade = %u\n", parity);
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

    // printf("buf sent: ");
    printf("Mensagem enviada tipo: %u\n", env.type);

    // for (size_t i = 0; i < buf_size; i++)
    // {
    //     printf("%X ", buf[i]);
    // }
    // printf("\n");

    // printf("parity sent: 0x%X\n", env.parity);
    // printf("size sent: 0x%X\n", env.size);

    // tenta enviar até conseguir
    while (rs_send(sockfd, buf, buf_size) == RETURN_ERROR)
    {
    }

    return RETURN_SUCCESS;
}

int packet_recv(int sockfd, void *data, packet_options_t *options)
{
    envelope_t env;
    char buf[PACKET_DATA_MAX_SIZE];
    // printf("buf: %s\n", buf);
    // #ifndef NDEBUG
    //     rs_recv(sockfd, buf, PACKET_DATA_MAX_SIZE); // descarta echo do loopback
    //     // printf("buf: %s\n", buf);
    // #endif

    printf("Aguardando receber...\n");

// tenta receber até c9nseguir
recieve:
    while (rs_recv(sockfd, buf, PACKET_DATA_MAX_SIZE) == RETURN_ERROR)
    {
    }
    // printf("buf read: %s\n", buf);

    // for (size_t i = 0; i < PACKET_DATA_MAX_SIZE; i++)
    // {
    //     printf("buf[%lu]: 0x%X\n", i, buf[i]);
    // }
    // printf("buf read: ");

    // for (size_t i = 0; i < PACKET_DATA_MAX_SIZE; i++)
    // {
    //     printf("%X ", buf[i]);
    // }
    // printf("\n");

    memcpy(&env, buf, sizeof(envelope_t) - 1);
    if (env.start_marker != PACKET_START_MARKER)
    {
        // debug((uint)env.start_marker);
        // printf("IF 1\n");
        // errno = EINTEGRITY;
        // return RETURN_ERROR;
        goto recieve;
    }

    env.parity = buf[env.size + sizeof(envelope_t) - 1];
    // printf("parity read from sent: 0x%X\n", env.parity);
    // printf("size read from sent: 0x%X\n", env.size);
    if (env.parity != calc_parity(buf + sizeof(envelope_t) - 1, env.size))
    {
        // #include <unistd.h>
        // debug((uint)env.parity);
        // debug(calc_parity(buf + sizeof(envelope_t) - 1, env.size));
        printf("env.parity = %u. calc_parity = %u\n", env.parity, calc_parity(buf + sizeof(envelope_t) - 1, env.size));
        for (size_t i = 0; i < PACKET_DATA_MAX_SIZE; i++)
        {
            printf("%X ", buf[i]);
        }
        printf("\n");
        // debug((uint)env.size);
        //         printf("bla %u\n", buf + sizeof(envelope_t) - 1);
        // write(STDOUT_FILENO, buf + sizeof(envelope_t) - 1, env.size);
        packet_nack(sockfd, 0);
        // errno = EINTEGRITY;
        // return RETURN_ERROR;
        goto recieve;
    }

    printf("Mensagem recebida tipo: %u\n", env.type);

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

int packet_error(int sockfd, int errcode, uint index)
{
    packet_options_t opt = {
        .index = index,
        .size = sizeof(int),
        .type = ERROR,
    };
    TRY(packet_send(sockfd, &errcode, opt));

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