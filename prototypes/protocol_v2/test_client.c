#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

#include "raw_socket.h"
#include "packet.h"
#include "prot_client.h"

int socket;
packet_options_t opt;
char readbuf[PACKET_DATA_MAX_SIZE];

// checa se deu erro
void check(int err, const char *msg)
{
    if (err == -1)
    {
        fprintf(stderr, "[ERROR] %s: %s\n", __FUNCTION__, msg);
        exit(1);
    }
}

// checa se processo child retornou codigo esperado
void check_child(int expected, const char *msg)
{
    int status;
    wait(&status);
    if (WEXITSTATUS(status) != expected)
    {
        fprintf(stderr, "[ERROR] %s: %s\n", __FUNCTION__, msg);
        exit(1);
    }
}

void test_protc_cd()
{
    void fork_protc_cd()
    {
        if (fork() == 0) // child
        {
            int err = protc_cd(socket, "panzerkampfwagen");
            exit(err);
        }
    }

    // teste com server OK
    fork_protc_cd();
    usleep(10000);
    check(packet_recv(socket, readbuf, &opt), "cliente nao enviou CD");
    check(packet_ok(socket, 0), "nao foi possivel enviar OK");
    check_child(0, "protc_cd terminou com erro");

    // teste com server ERROR
    fork_protc_cd();
    usleep(10000);
    check(packet_recv(socket, readbuf, &opt), "cliente nao enviou CD");
    check(packet_error(socket, "baka", 0), "nao foi possivel enviar ERROR");
    check_child(-1, "protc_cd terminou com sucesso ao receber ERROR");

    // teste com server NACK
    fork_protc_cd();
    usleep(10000);
    for (int i = 0; i < 1000; i++) // envia NACK 1000 vezes
    {
        check(packet_recv(socket, readbuf, &opt), "cliente nao enviou CD");
        check(packet_nack(socket, 0), "nao foi possivel enviar NACK");
    }
    check(packet_ok(socket, 0), "nao foi possivel enviar OK");
    check_child(0, "protc_cd terminou com erro");

    // teste com lixo
    fork_protc_cd();
    usleep(10000);
    check(packet_recv(socket, readbuf, &opt), "cliente nao enviou CD");
    opt.index = 0;
    opt.size = 16;
    opt.type = 0b111111;
    check(packet_send(socket, "jkshfglaisghasf", opt), "nao foi possivel enviar lixo");

    // teste com silencio
    fork_protc_cd();
    usleep(10000);
    check(packet_recv(socket, readbuf, &opt), "cliente nao enviou CD");
    check_child(0, "protc_cd recebeu coisas do alem");
}

int main(int argc, char const *argv[])
{
    socket = rs_socket("lo");
    rs_set_timeout(socket, (uint)5e6);

    test_protc_cd();
    printf("protc_cd OK!\n");

    return 0;
}
