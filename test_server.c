#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>

#include "error.h"
#include "packet.h"
#include "prot_server.h"
#include "raw_socket.h"

#define TIMEOUT (uint)1e6

int sock, ret;
packet_options_t opt;
char buf[PACKET_DATA_MAX_SIZE];

// checa se deu erro
#define CHECK(err, msg)                                                          \
    do                                                                           \
    {                                                                            \
        if ((err) == -1)                                                         \
        {                                                                        \
            fprintf(stderr, "[ERROR] %s:%d: %s\n", __FUNCTION__, __LINE__, msg); \
            exit(1);                                                             \
        }                                                                        \
    } while (0)

// checa se processo child retornou codigo esperado
#define CHECK_CHILD(expected, msg)                                                 \
    do                                                                             \
    {                                                                              \
        if (ret != (u_int8_t)(expected))                                           \
        {                                                                          \
            fprintf(stderr, "[ERROR] %s:%d -> %s\n", __FUNCTION__, __LINE__, msg); \
            exit(1);                                                               \
        }                                                                          \
    } while (0)

// checa se processo child retornou codigo esperado
#define CHECK_CHILD2(expected, msg)                                                \
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

int fork_prots_cd(const char *dir)
{
    // if (fork() == 0) // child
    // {
    int childsock = rs_socket("lo");
    rs_set_timeout(childsock, TIMEOUT);
    int err = prots_cd(childsock, dir);
    return err;
    // }
}

int fork_prots_mkdir(const char *dir)
{
    // if (fork() == 0) // child
    // {
    int childsock = rs_socket("lo");
    rs_set_timeout(childsock, TIMEOUT);
    int err = prots_mkdir(childsock, dir);
    return err;
    // }
}

void fork_prots_ls(const char *flag)
{
    if (fork() == 0) // child
    {
        int childsock = rs_socket("lo");
        rs_set_timeout(childsock, TIMEOUT);
        int err = prots_ls(childsock, flag);
        exit(err);
    }
}

void fork_prots_get(const char *dir)
{
    if (fork() == 0) // child
    {
        int childsock = rs_socket("lo");
        rs_set_timeout(childsock, TIMEOUT);
        int err = prots_get(childsock, dir);
        exit(err);
    }
}

void test_prots_cd()
{
    system("pwd");
    ret = fork_prots_cd(".");
    CHECK(packet_recv(sock, buf, &opt), "server não enviou OK");
    CHECK_OPT(opt, OK);
    CHECK_CHILD(RETURN_SUCCESS, "servidor retornou com erro. Esperado sucesso");

    system("pwd");
    ret = fork_prots_cd("..");
    CHECK(packet_recv(sock, buf, &opt), "server não enviou OK");
    CHECK_OPT(opt, OK);
    CHECK_CHILD(RETURN_SUCCESS, "servidor retornou com erro. Esperado sucesso");

    system("pwd");
    ret = fork_prots_cd("protocol_v2");
    CHECK(packet_recv(sock, buf, &opt), "server não enviou OK");
    CHECK_OPT(opt, OK);
    CHECK_CHILD(RETURN_SUCCESS, "servidor retornou com erro. Esperado sucesso");

    system("pwd");
    ret = fork_prots_cd("oi");
    CHECK(packet_recv(sock, buf, &opt), "server não enviou OK");
    CHECK_OPT(opt, OK);
    CHECK_CHILD(RETURN_SUCCESS, "servidor retornou com erro. Esperado sucesso");

    system("pwd");
    ret = fork_prots_cd("..");
    CHECK(packet_recv(sock, buf, &opt), "server não enviou OK");
    CHECK_OPT(opt, OK);
    CHECK_CHILD(RETURN_SUCCESS, "servidor retornou com erro. Esperado sucesso");

    system("pwd");
    ret = fork_prots_cd("askfssdfds");
    CHECK(packet_recv(sock, buf, &opt), "server não enviou ERRO");
    CHECK_OPT(opt, ERROR);
    CHECK_CHILD(RETURN_SUCCESS, "servidor retornou com erro. Esperado sucesso");

    system("pwd");
    ret = fork_prots_cd("..");
    CHECK(packet_recv(sock, buf, &opt), "server não enviou OK");
    CHECK_OPT(opt, OK);
    CHECK_CHILD(RETURN_SUCCESS, "servidor retornou com erro. Esperado sucesso");

    system("pwd");
    ret = fork_prots_cd("oi");
    CHECK(packet_recv(sock, buf, &opt), "server não enviou ERRO");
    CHECK_OPT(opt, ERROR);
    CHECK_CHILD(RETURN_SUCCESS, "servidor retornou com erro. Esperado sucesso");

    system("pwd");
    ret = fork_prots_cd("protocol_v2");
    CHECK(packet_recv(sock, buf, &opt), "server não enviou OK");
    CHECK_OPT(opt, OK);
    CHECK_CHILD(RETURN_SUCCESS, "servidor retornou com erro. Esperado sucesso");
}

void test_prots_mkdir()
{
    ret = fork_prots_mkdir("praxedes");
    CHECK(packet_recv(sock, buf, &opt), "servidor não enviou OK");
    CHECK_OPT(opt, OK);
    CHECK_CHILD(RETURN_SUCCESS, "servidor retornou com erro.");
    CHECK(system("ls praxedes") != 0 ? -1 : 0, "servidor não criou diretório.");
    // system("rm praxedes");

    // sleep(2);

    ret = fork_prots_mkdir("praxedes");
    CHECK(packet_recv(sock, buf, &opt), "servidor não enviou ERRO");
    CHECK_OPT(opt, ERROR);
    CHECK_CHILD(RETURN_SUCCESS, "servidor retornou com erro.");

    system("rm -rf praxedes");
}

void test_prots_ls()
{
    fork_prots_ls("");
    do
    {
        CHECK(packet_recv(sock, buf, &opt), "servidor não enviou SHOW/ENDTX");

        if (opt.type == ENDTX)
            break;

        CHECK_OPT(opt, SHOW);
        printf("%s", buf);
        CHECK(packet_ack(sock, 0), "não foi possível enviar ACK.");

    } while (1);
    
    CHECK_CHILD2(RETURN_SUCCESS, "servidor retornou com erro.");
}

void test_prots_get()
{
    fork_prots_get("TODO");
    CHECK(packet_recv(sock, buf, &opt), "servidor não enviou FDESC");
    CHECK_OPT(opt, FDESC);
    size_t tam;
    memcpy(&tam, buf, sizeof(size_t));
    printf("get: tamanho = %lu\n", tam);
    CHECK(packet_ok(sock, 0), "não foi possível enviar mensagem OK");
    
    while (1)
    {
        CHECK(packet_recv(sock, buf, &opt), "servidor não enviou DATA/ENDTX");
        
        if (opt.type == ENDTX)
            break;
            
        CHECK_OPT(opt, DATA);
        write(STDOUT_FILENO, buf, opt.size);
        CHECK(packet_ack(sock, 0), "não foi possível enviar ACK");
    }
    
    CHECK_CHILD2(RETURN_SUCCESS, "servidor retornou com erro");
}

int main()
{
    sock = rs_socket("lo");
    rs_set_timeout(sock, TIMEOUT);

    // printf("========== Testando prots_cd() ==========\n");
    // test_prots_cd();
    // printf("========== prots_cd() OK! YAY! ==========\n\n");
    // printf("======== Testando prots_mkdir() =========\n");
    // test_prots_mkdir();
    // printf("======== prots_mkdir() OK! YAY! =========\n\n");
    printf("========== Testando prots_ls() ===========\n");
    test_prots_ls();
    printf("========== prots_ls() OK! YAY! ==========\n\n");
    printf("========== Testando prots_get() =========\n");
    test_prots_get();
    printf("========== prots_get() OK! YAY! =========\n\n");
    // printf("========== Testando prots_put() =========\n");
    // test_prots_put();
    // printf("========== prots_put() OK! YAY! =========\n\n");

    printf("============== TUDO CERTO ===============\n");

    return 0;
}