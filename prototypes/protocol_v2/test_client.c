#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

#include "raw_socket.h"
#include "packet.h"
#include "prot_client.h"
#include "debug.h"

int sock;
packet_options_t opt;
char readbuf[PACKET_DATA_MAX_SIZE];

// checa se deu erro
#define CHECK(err, msg)                                                          \
    do                                                                           \
    {                                                                            \
        if (err == -1)                                                           \
        {                                                                        \
            fprintf(stderr, "[ERROR] %s:%d: %s\n", __FUNCTION__, __LINE__, msg); \
            exit(1);                                                             \
        }                                                                        \
    } while (0)

// checa se processo child retornou codigo esperado
#define CHECK_CHILD(expected, msg)                                                 \
    do                                                                             \
    {                                                                              \
        int status;                                                                \
        wait(&status);                                                             \
        if (WEXITSTATUS(status) != (u_int8_t)expected)                             \
        {                                                                          \
            fprintf(stderr, "[ERROR] %s:%d -> %s\n", __FUNCTION__, __LINE__, msg); \
            exit(1);                                                               \
        }                                                                          \
    } while (0)

#define CHECK_OPT(opt, expected)                                                                    \
    do                                                                                              \
    {                                                                                               \
        if (opt.type != expected)                                                                   \
        {                                                                                           \
            fprintf(stderr, "[ERROR] %s:%d -> opt.type == %d\n", __FUNCTION__, __LINE__, opt.type); \
            exit(1);                                                                                \
        }                                                                                           \
    } while (0)

void test_protc_cd()
{
    void fork_protc_cd()
    {
        if (fork() == 0) // child
        {
            int childsock = rs_socket("lo");
            rs_set_timeout(childsock, (uint)1e6);
            int err = protc_cd(childsock, "panzerkampfwagen");
            exit(err);
        }
    }

    // teste com server OK
    fork_protc_cd();
    CHECK(packet_recv(sock, readbuf, &opt), "cliente nao enviou CD");
    CHECK_OPT(opt, CD);
    CHECK(packet_ok(sock, 0), "nao foi possivel enviar OK");
    CHECK_CHILD(0, "protc_cd terminou com erro");
    puts("passou no teste OK\n");

    // teste com server ERROR
    fork_protc_cd();
    CHECK(packet_recv(sock, readbuf, &opt), "cliente nao enviou CD");
    CHECK_OPT(opt, CD);
    CHECK(packet_error(sock, "baka", 0), "nao foi possivel enviar ERROR");
    CHECK_CHILD(-1, "protc_cd terminou com sucesso ao receber ERROR");
    puts("passou no teste ERROR\n");

    // teste com lixo
    fork_protc_cd();
    CHECK(packet_recv(sock, readbuf, &opt), "cliente nao enviou CD");
    CHECK_OPT(opt, CD);
    opt.index = 0;
    opt.size = 16;
    opt.type = 0b111111;
    CHECK(packet_send(sock, "jkshfglaisghasf", opt), "nao foi possivel enviar lixo");
    CHECK_CHILD(-1, "protc_cd terminou com sucesso ao receber lixo");
    puts("passou no teste lixo\n");

    // teste com silencio
    fork_protc_cd();
    CHECK(packet_recv(sock, readbuf, &opt), "cliente nao enviou CD");
    CHECK_OPT(opt, CD);
    CHECK_CHILD(-1, "protc_cd recebeu coisas do alem");
    puts("passou no teste silencio\n");

    // teste com server NACK
    fork_protc_cd();
    for (int i = 0; i < 1000; i++) // envia NACK 10 vezes
    {
        CHECK(packet_recv(sock, readbuf, &opt), "cliente nao enviou CD");
        CHECK_OPT(opt, CD);
        CHECK(packet_nack(sock, 0), "nao foi possivel enviar NACK");
    }
    CHECK(packet_recv(sock, readbuf, &opt), "cliente nao enviou CD");
    CHECK(packet_ok(sock, 0), "nao foi possivel enviar OK");
    CHECK_CHILD(0, "protc_cd terminou com erro");
    puts("passou no teste NACK\n");
}

int main(int argc, char const *argv[])
{
    sock = rs_socket("lo");
    rs_set_timeout(sock, (uint)1e6);

    test_protc_cd();
    printf("protc_cd OK!\n");

    return 0;
}
