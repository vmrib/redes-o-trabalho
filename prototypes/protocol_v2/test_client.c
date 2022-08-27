#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>

#include "raw_socket.h"
#include "packet.h"
#include "prot_client.h"
#include "debug.h"

#define TIMEOUT (uint)1e6

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
    void fork_protc_cd() // cria fork pra interação com protc
    {
        if (fork() == 0) // child
        {
            int childsock = rs_socket("lo");
            rs_set_timeout(childsock, TIMEOUT);
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
    printf("%s: %s\n", __FUNCTION__, "passou no teste OK\n");

    // teste com server ERROR
    fork_protc_cd();
    CHECK(packet_recv(sock, readbuf, &opt), "cliente nao enviou CD");
    CHECK_OPT(opt, CD);
    CHECK(packet_error(sock, "baka", 0), "nao foi possivel enviar ERROR");
    CHECK_CHILD(-1, "protc_cd terminou com sucesso ao receber ERROR");
    printf("%s: %s\n", __FUNCTION__, "passou no teste ERROR\n");

    // teste com lixo
    fork_protc_cd();
    CHECK(packet_recv(sock, readbuf, &opt), "cliente nao enviou CD");
    CHECK_OPT(opt, CD);
    opt.index = 0;
    opt.size = 16;
    opt.type = 0b111111;
    CHECK(packet_send(sock, "jkshfglaisghasf", opt), "nao foi possivel enviar lixo");
    CHECK_CHILD(-1, "protc_cd terminou com sucesso ao receber lixo");
    printf("%s: %s\n", __FUNCTION__, "passou no teste lixo\n");

    // teste com silencio
    fork_protc_cd();
    CHECK(packet_recv(sock, readbuf, &opt), "cliente nao enviou CD");
    CHECK_OPT(opt, CD);
    CHECK_CHILD(-1, "protc_cd recebeu coisas do alem");
    printf("%s: %s\n", __FUNCTION__, "passou no teste silencio\n");

    // teste com server NACK
    fork_protc_cd();
    for (int i = 0; i < 100; i++) // envia NACK 10 vezes
    {
        CHECK(packet_recv(sock, readbuf, &opt), "cliente nao enviou CD");
        CHECK_OPT(opt, CD);
        CHECK(packet_nack(sock, 0), "nao foi possivel enviar NACK");
    }
    CHECK(packet_recv(sock, readbuf, &opt), "cliente nao enviou CD");
    CHECK(packet_ok(sock, 0), "nao foi possivel enviar OK");
    CHECK_CHILD(0, "protc_cd terminou com erro");
    printf("%s: %s\n", __FUNCTION__, "passou no teste NACK\n");
}

void test_protc_mkdir()
{
    void fork_protc_mkdir()
    {
        if (fork() == 0) // child
        {
            int childsock = rs_socket("lo");
            rs_set_timeout(childsock, TIMEOUT);
            int err = protc_mkdir(childsock, "panzerkampfwagen");
            exit(err);
        }
    }

    // teste com server OK
    fork_protc_mkdir();
    CHECK(packet_recv(sock, readbuf, &opt), "cliente nao enviou MKDIR");
    CHECK_OPT(opt, MKDIR);
    CHECK(packet_ok(sock, 0), "nao foi possivel enviar OK");
    CHECK_CHILD(0, "protc_cd terminou com erro");
    printf("%s: %s\n", __FUNCTION__, "passou no teste OK\n");

    // teste com server ERROR
    fork_protc_mkdir();
    CHECK(packet_recv(sock, readbuf, &opt), "cliente nao enviou MKDIR");
    CHECK_OPT(opt, MKDIR);
    CHECK(packet_error(sock, "baka", 0), "nao foi possivel enviar ERROR");
    CHECK_CHILD(-1, "protc_cd terminou com sucesso ao receber ERROR");
    printf("%s: %s\n", __FUNCTION__, "passou no teste ERROR\n");

    // teste com lixo
    fork_protc_mkdir();
    CHECK(packet_recv(sock, readbuf, &opt), "cliente nao enviou MKDIR");
    CHECK_OPT(opt, MKDIR);
    opt.index = 0;
    opt.size = 16;
    opt.type = 0b111111;
    CHECK(packet_send(sock, "jkshfglaisghasf", opt), "nao foi possivel enviar lixo");
    CHECK_CHILD(-1, "protc_cd terminou com sucesso ao receber lixo");
    printf("%s: %s\n", __FUNCTION__, "passou no teste lixo\n");

    // teste com silencio
    fork_protc_mkdir();
    CHECK(packet_recv(sock, readbuf, &opt), "cliente nao enviou MKDIR");
    CHECK_OPT(opt, MKDIR);
    CHECK_CHILD(-1, "protc_cd recebeu coisas do alem");
    printf("%s: %s\n", __FUNCTION__, "passou no teste silencio\n");

    // teste com server NACK
    fork_protc_mkdir();
    for (int i = 0; i < 100; i++) // envia NACK 10 vezes
    {
        CHECK(packet_recv(sock, readbuf, &opt), "cliente nao enviou MKDIR");
        CHECK_OPT(opt, MKDIR);
        CHECK(packet_nack(sock, 0), "nao foi possivel enviar NACK");
    }
    CHECK(packet_recv(sock, readbuf, &opt), "cliente nao enviou MKDIR");
    CHECK(packet_ok(sock, 0), "nao foi possivel enviar OK");
    CHECK_CHILD(0, "protc_cd terminou com erro");
    printf("%s: %s\n", __FUNCTION__, "passou no teste NACK\n");
}

void test_protc_ls()
{
    // TODO: testar ls args
    //       testar index
    void fork_protc_ls()
    {
        if (fork() == 0) // child
        {
            int childsock = rs_socket("lo");
            rs_set_timeout(childsock, TIMEOUT);
            int err = protc_ls(childsock, "TODO");
            exit(err);
        }
    }

    const char *dir[] = {
        "homework",
        "trabalho.pdf",
        "impostor.exe",
        "trab1-requisitos.docx",
    };

    // teste com tudo OK
    fork_protc_ls();
    CHECK(packet_recv(sock, readbuf, &opt), "cliente nao enviou LS");
    CHECK_OPT(opt, LS);
    for (int i = 0; i < 4; i++)
    {
        opt.type = SHOW;
        opt.size = strlen(dir[i]) + 1;
        opt.index = i;
        CHECK(packet_send(sock, (void *)dir[i], opt), "nao foi possivel enviar LS");
        CHECK(packet_recv(sock, readbuf, &opt), "cliente nao enviou ACK");
        CHECK_OPT(opt, ACK);
    }
    CHECK(packet_end(sock, 0), "nao foi possivel enviar ENDTX");
    CHECK_CHILD(0, "protc_ls retornou com erro");

    // teste com NACK
    fork_protc_ls();
    for (int i = 0; i < 100; i++)
    {
        CHECK(packet_recv(sock, readbuf, &opt), "cliente nao enviou LS");
        CHECK_OPT(opt, LS);
        CHECK(packet_nack(sock, 0), "nao foi possivel enviar NACK");
    }
    CHECK(packet_recv(sock, readbuf, &opt), "cliente nao enviou LS");
    CHECK(packet_end(sock, 0), "nao foi possivel enviar ENDTX");
    CHECK_CHILD(0, "protc_ls retornou com erro");

    // teste com ERROR
    fork_protc_ls();
    CHECK(packet_recv(sock, readbuf, &opt), "cliente nao enviou LS");
    CHECK_OPT(opt, LS);
    CHECK(packet_error(sock, "baka", 0), "nao foi possivel enviar ERROR");
    CHECK_CHILD(-1, "protc_ls terminou com sucesso. Esperado erro");

    // teste com ERROR 2
    fork_protc_ls();
    CHECK(packet_recv(sock, readbuf, &opt), "cliente nao enviou LS");
    CHECK_OPT(opt, LS);
    for (int i = 0; i < 2; i++)
    {
        opt.type = SHOW;
        opt.size = strlen(dir[i]) + 1;
        opt.index = i;
        CHECK(packet_send(sock, (void *)dir[i], opt), "nao foi possivel enviar LS");
        CHECK(packet_recv(sock, readbuf, &opt), "cliente nao enviou ACK");
        CHECK_OPT(opt, ACK);
    }
    CHECK(packet_error(sock, "baka", 0), "nao foi possivel enviar ERROR");
    CHECK_CHILD(-1, "protc_ls terminou com sucesso. Esperado erro");

    // teste com silencio
    fork_protc_ls();
    CHECK(packet_recv(sock, readbuf, &opt), "cliente nao enviou LS");
    CHECK_OPT(opt, LS);
    CHECK_CHILD(-1, "protc_ls terminou com sucesso. Esperado erro");

    // teste com ENDTX
    fork_protc_ls();
    CHECK(packet_recv(sock, readbuf, &opt), "cliente nao enviou LS");
    CHECK_OPT(opt, LS);
    for (int i = 0; i < 4; i++)
    {
        opt.type = SHOW;
        opt.size = strlen(dir[i]) + 1;
        opt.index = i;
        CHECK(packet_send(sock, (void *)dir[i], opt), "nao foi possivel enviar LS");
        CHECK(packet_recv(sock, readbuf, &opt), "cliente nao enviou ACK");
        CHECK_OPT(opt, ACK);
    }
    CHECK_CHILD(-1, "protc_ls terminou com sucesso. Esperado erro");

    // teste com lixo
    fork_protc_ls();
    CHECK(packet_recv(sock, readbuf, &opt), "cliente nao enviou LS");
    CHECK_OPT(opt, LS);
    for (int i = 0; i < 2; i++)
    {
        opt.type = SHOW;
        opt.size = strlen(dir[i]) + 1;
        opt.index = i;
        CHECK(packet_send(sock, (void *)dir[i], opt), "nao foi possivel enviar LS");
        CHECK(packet_recv(sock, readbuf, &opt), "cliente nao enviou ACK");
        CHECK_OPT(opt, ACK);
    }
    opt.index = 0;
    opt.size = 16;
    opt.type = 0b111111;
    CHECK(packet_send(sock, "jkshfglaisghasf", opt), "nao foi possivel enviar lixo");
    CHECK_CHILD(-1, "protc_ls terminou com sucesso. Esperado erro");
}

int main(int argc, char const *argv[])
{
    sock = rs_socket("lo");
    rs_set_timeout(sock, TIMEOUT);

    printf("========== Testando protc_cd() ==========\n");
    test_protc_cd();
    printf("========== protc_cd() OK! YAY! ==========\n\n");
    printf("======== Testando protc_mkdir() =========\n");
    test_protc_mkdir();
    printf("======== protc_mkdir() OK! YAY! =========\n\n");
    printf("========== Testando protc_ls() ===========\n");
    test_protc_ls();
    printf("========== protc_ls() OK! YAY! ==========\n\n");
    printf("============== TUDO CERTO ===============\n");
    return 0;
}
