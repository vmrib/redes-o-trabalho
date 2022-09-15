#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>

#include "raw_socket.h"
#include "packet.h"
#include "prot_client.h"
#include "debug.h"
#include "error.h"

#define TIMEOUT (uint)1e6

int sock;
packet_options_t opt;
char buf[PACKET_DATA_MAX_SIZE];

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
    CHECK(packet_recv(sock, buf, &opt), "cliente nao enviou CD");
    CHECK_OPT(opt, CD);
    CHECK(packet_ok(sock, 0), "nao foi possivel enviar OK");
    CHECK_CHILD(0, "protc_cd terminou com erro");
    printf("%s: %s\n", __FUNCTION__, "passou no teste OK\n");

    // teste com server ERROR
    fork_protc_cd();
    CHECK(packet_recv(sock, buf, &opt), "cliente nao enviou CD");
    CHECK_OPT(opt, CD);
    CHECK(packet_error(sock, ENODIR, 0), "nao foi possivel enviar ERROR");
    CHECK_CHILD(-1, "protc_cd terminou com sucesso ao receber ERROR");
    printf("%s: %s\n", __FUNCTION__, "passou no teste ERROR\n");

    // teste com lixo
    // fork_protc_cd();
    // CHECK(packet_recv(sock, buf, &opt), "cliente nao enviou CD");
    // CHECK_OPT(opt, CD);
    // opt.index = 0;
    // opt.size = 16;
    // opt.type = 0b111111;
    // CHECK(packet_send(sock, "jkshfglaisghasf", opt), "nao foi possivel enviar lixo");
    // CHECK_CHILD(-1, "protc_cd terminou com sucesso ao receber lixo");
    // printf("%s: %s\n", __FUNCTION__, "passou no teste lixo\n");

    // teste com silencio
    // fork_protc_cd();
    // CHECK(packet_recv(sock, buf, &opt), "cliente nao enviou CD");
    // CHECK_OPT(opt, CD);
    // CHECK_CHILD(-1, "protc_cd recebeu coisas do alem");
    // printf("%s: %s\n", __FUNCTION__, "passou no teste silencio\n");

    // teste com server NACK
    fork_protc_cd();
    for (int i = 0; i < 100; i++) // envia NACK 10 vezes
    {
        CHECK(packet_recv(sock, buf, &opt), "cliente nao enviou CD");
        CHECK_OPT(opt, CD);
        CHECK(packet_nack(sock, 0), "nao foi possivel enviar NACK");
    }
    CHECK(packet_recv(sock, buf, &opt), "cliente nao enviou CD");
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
    CHECK(packet_recv(sock, buf, &opt), "cliente nao enviou MKDIR");
    CHECK_OPT(opt, MKDIR);
    CHECK(packet_ok(sock, 0), "nao foi possivel enviar OK");
    CHECK_CHILD(0, "protc_cd terminou com erro");
    printf("%s: %s\n", __FUNCTION__, "passou no teste OK\n");

    // teste com server ERROR
    fork_protc_mkdir();
    CHECK(packet_recv(sock, buf, &opt), "cliente nao enviou MKDIR");
    CHECK_OPT(opt, MKDIR);
    CHECK(packet_error(sock, EALREADYDIR, 0), "nao foi possivel enviar ERROR");
    CHECK_CHILD(-1, "protc_cd terminou com sucesso ao receber ERROR");
    printf("%s: %s\n", __FUNCTION__, "passou no teste ERROR\n");

    // teste com lixo
    // fork_protc_mkdir();
    // CHECK(packet_recv(sock, buf, &opt), "cliente nao enviou MKDIR");
    // CHECK_OPT(opt, MKDIR);
    // opt.index = 0;
    // opt.size = 16;
    // opt.type = 0b111111;
    // CHECK(packet_send(sock, "jkshfglaisghasf", opt), "nao foi possivel enviar lixo");
    // CHECK_CHILD(-1, "protc_cd terminou com sucesso ao receber lixo");
    // printf("%s: %s\n", __FUNCTION__, "passou no teste lixo\n");

    // teste com silencio
    // fork_protc_mkdir();
    // CHECK(packet_recv(sock, buf, &opt), "cliente nao enviou MKDIR");
    // CHECK_OPT(opt, MKDIR);
    // CHECK_CHILD(-1, "protc_cd recebeu coisas do alem");
    // printf("%s: %s\n", __FUNCTION__, "passou no teste silencio\n");

    // teste com server NACK
    fork_protc_mkdir();
    for (int i = 0; i < 100; i++) // envia NACK 10 vezes
    {
        CHECK(packet_recv(sock, buf, &opt), "cliente nao enviou MKDIR");
        CHECK_OPT(opt, MKDIR);
        CHECK(packet_nack(sock, 0), "nao foi possivel enviar NACK");
    }
    CHECK(packet_recv(sock, buf, &opt), "cliente nao enviou MKDIR");
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
            int err = protc_ls(childsock, "TODO...");
            exit(err);
        }
    }

    const char *dir[] = {
        "homework\n",
        "trabalho.pdf\n",
        "impostor.exe\n",
        "trab1-requisitos.docx\n",
    };

    // teste com tudo OK
    fork_protc_ls();
    CHECK(packet_recv(sock, buf, &opt), "cliente nao enviou LS");
    CHECK_OPT(opt, LS);
    for (int i = 0; i < 4; i++)
    {
        opt.type = SHOW;
        opt.size = strlen(dir[i]) + 1;
        opt.index = i;
        CHECK(packet_send(sock, (void *)dir[i], opt), "nao foi possivel enviar LS");
        CHECK(packet_recv(sock, buf, &opt), "cliente nao enviou ACK");
        CHECK_OPT(opt, ACK);
    }
    CHECK(packet_end(sock, 0), "nao foi possivel enviar ENDTX");
    CHECK_CHILD(0, "protc_ls retornou com erro");
    printf("%s: %s\n", __FUNCTION__, "passou no teste OK\n");

    // teste com NACK
    fork_protc_ls();
    for (int i = 0; i < 100; i++)
    {
        CHECK(packet_recv(sock, buf, &opt), "cliente nao enviou LS");
        CHECK_OPT(opt, LS);
        CHECK(packet_nack(sock, 0), "nao foi possivel enviar NACK");
    }
    CHECK(packet_recv(sock, buf, &opt), "cliente nao enviou LS");
    CHECK(packet_end(sock, 0), "nao foi possivel enviar ENDTX");
    CHECK_CHILD(0, "protc_ls retornou com erro");
    printf("%s: %s\n", __FUNCTION__, "passou no teste NACK\n");

    // teste com ERROR
    fork_protc_ls();
    CHECK(packet_recv(sock, buf, &opt), "cliente nao enviou LS");
    CHECK_OPT(opt, LS);
    CHECK(packet_error(sock, EPERMISSION, 0), "nao foi possivel enviar ERROR");
    CHECK_CHILD(-1, "protc_ls terminou com sucesso. Esperado erro");
    printf("%s: %s\n", __FUNCTION__, "passou no teste ERROR (1)\n");

    // teste com ERROR 2
    fork_protc_ls();
    CHECK(packet_recv(sock, buf, &opt), "cliente nao enviou LS");
    CHECK_OPT(opt, LS);
    for (int i = 0; i < 2; i++)
    {
        opt.type = SHOW;
        opt.size = strlen(dir[i]) + 1;
        opt.index = i;
        CHECK(packet_send(sock, (void *)dir[i], opt), "nao foi possivel enviar LS");
        CHECK(packet_recv(sock, buf, &opt), "cliente nao enviou ACK");
        CHECK_OPT(opt, ACK);
    }
    CHECK(packet_error(sock, ENODIR, 0), "nao foi possivel enviar ERROR");
    CHECK_CHILD(-1, "protc_ls terminou com sucesso. Esperado erro");
    printf("%s: %s\n", __FUNCTION__, "passou no teste ERROR (2)\n");

    // teste com silencio
    // fork_protc_ls();
    // CHECK(packet_recv(sock, buf, &opt), "cliente nao enviou LS");

    // CHECK_OPT(opt, LS);
    // CHECK_CHILD(-1, "protc_ls terminou com sucesso. Esperado erro");
    // printf("%s: %s\n", __FUNCTION__, "passou no teste silencio\n");

    // teste com ENDTX
    // fork_protc_ls();
    // CHECK(packet_recv(sock, buf, &opt), "cliente nao enviou LS");
    // CHECK_OPT(opt, LS);
    // for (int i = 0; i < 4; i++)
    // {
    //     opt.type = SHOW;
    //     opt.size = strlen(dir[i]) + 1;
    //     opt.index = i;
    //     CHECK(packet_send(sock, (void *)dir[i], opt), "nao foi possivel enviar LS");
    //     CHECK(packet_recv(sock, buf, &opt), "cliente nao enviou ACK");
    //     CHECK_OPT(opt, ACK);
    // }
    // CHECK_CHILD(-1, "protc_ls terminou com sucesso. Esperado erro");
    // printf("%s: %s\n", __FUNCTION__, "passou no teste ENDTX\n");

    // teste com lixo
    // fork_protc_ls();
    // CHECK(packet_recv(sock, buf, &opt), "cliente nao enviou LS");
    // CHECK_OPT(opt, LS);
    // for (int i = 0; i < 2; i++)
    // {
    //     opt.type = SHOW;
    //     opt.size = strlen(dir[i]) + 1;
    //     opt.index = i;
    //     CHECK(packet_send(sock, (void *)dir[i], opt), "nao foi possivel enviar LS");
    //     CHECK(packet_recv(sock, buf, &opt), "cliente nao enviou ACK");
    //     CHECK_OPT(opt, ACK);
    // }
    // opt.index = 0;
    // opt.size = 16;
    // opt.type = 0b101010;
    // CHECK(packet_send(sock, "jkshfglaisghasf", opt), "nao foi possivel enviar lixo");
    // CHECK_CHILD(-1, "protc_ls terminou com sucesso. Esperado erro");
    // printf("%s: %s\n", __FUNCTION__, "passou no teste lixo\n");
}

void test_protc_get()
{
    void fork_protc_get()
    {
        if (fork() == 0) // child
        {
            int childsock = rs_socket("lo");
            rs_set_timeout(childsock, TIMEOUT);
            int err = protc_get(childsock, "TODO");
            exit(err);
        }
    }

    const char *file =
        "Ela partiu\n"
        "Partiu e nunca mais voltou\n"
        "Ela sumiu, sumiu e nunca mais voltou\n"
        "Se souberem onde ela está\n"
        "Digam-me e vou lá buscá-la\n"
        "Pelo menos telefone em seu nome\n"
        "Me dêumflex uma dica, uma pista, insistaEi! e nunca mais voltou\n"
        "Ela sumiu, sumiu e nunca mais voltou\n"
        "Ela partiu, partiu\n"
        "E nunca mais voltou\n"
        "Se eu soubesse onde ela foi iria atrás\n"
        "Mas não sei mais nem direção\n"
        "Várias noites que eu não durmo um segundo\n"
        "Estou cansado\n"
        "Magoado exausto\n"
        "E nunca mais voltou\n";

    size_t filesize = strlen(file) + 1;
    // teste tudo ok
    fork_protc_get();
    CHECK(packet_recv(sock, buf, &opt), "cliente nao enviou GET");
    CHECK_OPT(opt, GET);

    // opt.type = FDESC;
    // opt.index = 0;
    // opt.size = sizeof(filename);
    // strcpy(readbuf, filename, sizeof(filename));
    opt.type = FDESC;
    opt.index = 0;
    opt.size = sizeof(size_t);
    memcpy(buf, &filesize, sizeof(size_t));
    // printf("------------------------\n");
    CHECK(packet_send(sock, buf, opt), "nao foi possivel enviar FDESC");
    // printf("------------------------\n");

    CHECK(packet_recv(sock, buf, &opt), "cliente nao enviou OK");
    CHECK_OPT(opt, OK);

    size_t i, j;
    for (i = 0, j = 0; i <= filesize - (PACKET_DATA_MAX_SIZE - 5); i += (PACKET_DATA_MAX_SIZE - 5), j++)
    {
        opt.type = DATA;
        opt.index = j;
        opt.size = PACKET_DATA_MAX_SIZE - 5;
        memcpy(buf, file + i, opt.size);
        printf("========================\n");
        write(STDOUT_FILENO, buf, opt.size);
        printf("\n========================\n");
        CHECK(packet_send(sock, buf, opt), "nao foi possivel enviar DATA");
        CHECK(packet_recv(sock, buf, &opt), "cliente nao enviou ACK");
        CHECK_OPT(opt, ACK);
    }
    // i -= PACKET_DATA_MAX_SIZE - 5;
    opt.type = DATA;
    opt.index = j;
    opt.size = filesize - i - 1;
    memcpy(buf, file + i, opt.size);
    printf("========================\n");
    write(STDOUT_FILENO, buf, opt.size);
    printf("\n========================\n");
    CHECK(packet_send(sock, buf, opt), "nao foi possivel enviar DATA");
    CHECK(packet_recv(sock, buf, &opt), "cliente nao enviou ACK");
    CHECK_OPT(opt, ACK);
    CHECK(packet_end(sock, j++), "nao foi possivel enviar ENDTX");
    CHECK_CHILD(0, "protc_get retornou com erro");
    puts("passou do ok? Verificar arquivo");
}

void test_protc_put()
{
    void fork_protc_put()
    {
        if (fork() == 0) // child
        {
            int childsock = rs_socket("lo");
            rs_set_timeout(childsock, TIMEOUT);
            int err = protc_put(childsock, "TODO");
            exit(err);
        }
    }

    fork_protc_put();
    CHECK(packet_recv(sock, buf, &opt), "cliente nao enviou PUT");
    CHECK_OPT(opt, PUT);

    printf("put arquivo %s\n", buf);

    CHECK(packet_ok(sock, 0), "nao foi possivel enviar OK");
    CHECK(packet_recv(sock, buf, &opt), "cliente nao enviou FDESC");
    CHECK_OPT(opt, FDESC);

    size_t filesize;
    memcpy(&filesize, buf, opt.size);
    printf("put tamanho arquivo %lu\n", filesize);

    do
    {
        CHECK(packet_ok(sock, 0), "nao foi possivel enviar OK");
        CHECK(packet_recv(sock, buf, &opt), "cliente nao enviou DATA/ENDTX");

        if (opt.type != DATA && opt.type != ENDTX)
            CHECK_OPT(opt, DATA);

        if (opt.type == DATA)
        {
            write(STDOUT_FILENO, buf, opt.size);
        }

    } while (opt.type != ENDTX);
    CHECK_OPT(opt, ENDTX);

    CHECK_CHILD(0, "protc_get retornou com erro");
    puts("passou do ok? Verificar stdout");
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
    printf("========== Testando protc_get() =========\n");
    test_protc_get();
    printf("========== protc_get() OK! YAY! =========\n\n");
    printf("========== Testando protc_put() =========\n");
    test_protc_put();
    printf("========== protc_put() OK! YAY! =========\n\n");

    printf("============== TUDO CERTO ===============\n");
    return 0;
}
