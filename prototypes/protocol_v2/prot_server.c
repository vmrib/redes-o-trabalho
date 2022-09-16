#include "prot_server.h"
#include "packet.h"
#include "error.h"
#include "debug.h"
#include <unistd.h> // chdir
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>

uint s_index = 0;

int prots_cd(int sockfd, const char *dirname) // retorna OK, NACK ou ERRO
{
    // packet_options_t opt;
    printf("%s\n", dirname);
    int resultado = chdir(dirname);

    if (!resultado)
        packet_ok(sockfd, s_index);
    else
        packet_error(sockfd, ENODIR, s_index); // descrever melhor os erros

    // opt.index = s_index; // receber? global?
    // s_index++;

    // opt.size = strlen(dirname) + 1; // +1 do \0?
    // opt.size = 0; // teste? retorna o q no OK? nada?

    // TRY(packet_send(sockfd, "", opt));

    s_index++;

    return RETURN_SUCCESS;
}

// problema no -l
int prots_ls(int sockfd, char *flag) // retorna NACK, ERRO ou MOSTRA NA TELA
{
    packet_options_t opt;
    char buf[PACKET_DATA_MAX_SIZE];

    FILE *fp;
    char path[1035];
    char ls[64] = "ls ";

    strcat(ls, flag);
    strcat(ls, " .");

    fp = popen(ls, "r");

    if (fp == NULL)
    {
        TRY(packet_error(sockfd, EPERMISSION, s_index));
        printf("Failed to run command\n");
        exit(1);
    }

    /* Read the output a line at a time - output it. */
    // while (fgets(path, sizeof(path), fp) != NULL)
    fgets(path, sizeof(path), fp);
    while (1)
    {
        // printf("%ld\n", strlen(path));
        // path[strlen(path)-1] = '    ';
        // printf("outro\n");
        s_index++;
        // printf("index: %d\n", s_index);
        // printf("mandou\n");
        opt.index = s_index;
        opt.size = strlen(path) + 1;
        opt.type = SHOW;
        TRY(packet_send(sockfd, path, opt));

        printf("\nMensagem enviada: %s", path);
        // printf("Tamanho: %u\nTipo: %u \nIndex = %u\n", opt.size, opt.type, opt.index);
        if (opt.type == EMPTY)
            printf("Tipo: EMPTY\n");
        else if (opt.type == SHOW)
            printf("Tipo: SHOW\n");
        else if (opt.type == ACK)
            printf("Tipo: ACK\n");
        else
            printf("Tipo: %u\n", opt.type);

        if (packet_recv(sockfd, buf, &opt) == -1)
            if (errno == ETIMEDOUT)
                continue;

        if (opt.type == ACK)
            if (fgets(path, sizeof(path), fp) == NULL)
                break;

        packet_reset(&opt);
    }

    TRY(packet_end(sockfd, s_index));
    // printf("FIM DO PACOTE\n");
    s_index++;
    // printf("index: %d\n", s_index);

    packet_reset(&opt);

    /* close */
    pclose(fp);

    return RETURN_SUCCESS;
}

int prots_mkdir(int sockfd, char *dirname) // retorna OK, NACK ou ERRO
{
    char mkdir[64] = "mkdir ";
    strcat(mkdir, dirname);
    if (system(mkdir) != 0)
    {
        TRY(packet_error(sockfd, EALREADYDIR, s_index));
    }
    else
    {
        TRY(packet_ok(sockfd, 0));
    }
    s_index++;

    return RETURN_SUCCESS;
}

int prots_get(int sockfd, char *filename)
{
    packet_options_t opt, data_opt;
    char buf[PACKET_DATA_MAX_SIZE], data_buf[PACKET_DATA_MAX_SIZE];

    FILE *file = fopen(filename, "rb");

    if (file == NULL)
    {
        TRY(packet_error(sockfd, ENOFILE, s_index));
        s_index++;
        printf("get: %s\n", filename);
        return RETURN_SUCCESS;
    }

    printf("get: %s\n", filename);

    // pegar tamanho do arquivo
    fseek(file, 0, SEEK_END);      // seek to end of file
    size_t filesize = ftell(file); // get current file pointer
    fseek(file, 0, SEEK_SET);      // seek back to beginning of file

    printf("tamanho: %lu\n", filesize);

    do
    {
        opt.type = FDESC;
        opt.index = s_index;
        opt.size = sizeof(size_t);
        TRY(packet_send(sockfd, &filesize, opt));
        s_index++;

        printf("Enviado FDESC\n");

        TRY(packet_recv(sockfd, buf, &opt));

        if (opt.type == ERROR) // cliente deu pau, a gente só aceita
        {
            return RETURN_SUCCESS;
        }
    } while (opt.type != OK);

    // data_opt.size = 1; // gambiarra

    while (1)
    {
        data_opt.size = fread(data_buf, sizeof(char), PACKET_DATA_MAX_SIZE - 10, file);
        data_opt.type = DATA;
        data_opt.index = s_index;
        TRY(packet_send(sockfd, data_buf, data_opt));
        s_index++;

        // printf("Enviado DATA com tamanho %u.\n", data_opt.size);
        // printf("==========================\n");
        // write(STDOUT_FILENO, data_buf, data_opt.size);
        // printf("==========================\n");

        // enviou tudo que dava
        // como assim? pq data_opt em outros lugares?
        if (data_opt.size == 0)
            break;

        TRY(packet_recv(sockfd, buf, &opt));
        while (opt.type == NACK)
        {
            TRY(packet_send(sockfd, data_buf, data_opt));
            // printf("data_opt.size: %u", data_opt.size);
            s_index++;
            TRY(packet_recv(sockfd, buf, &opt));
        }

        // if (opt.type == NACK)
        //     continue;
    }

    TRY(packet_end(sockfd, s_index));
    // printf("Enviado ENDTX\n");

    return RETURN_SUCCESS;
}

int prots_put(int sockfd, char *filename)
{
    packet_options_t opt;
    char buf[PACKET_DATA_MAX_SIZE];

    FILE *file = fopen(filename, "wb"); // seta errno
    if (!file)
    {
        // converte erro de acesso da fopen() para erro de acesso do protocolo
        if (errno == EACCES)
            errno = EPERMISSION;

        packet_error(sockfd, EPERMISSION, s_index);
        // tem que enviar o erro, nao retornar
        return RETURN_ERROR;
    }

    TRY(packet_ok(sockfd, s_index));
    s_index++;

    while (1)
    {
        TRY(packet_recv(sockfd, buf, &opt));

        if (opt.type == FDESC)
            break;

        TRY(packet_nack(sockfd, s_index));
        s_index++;
    }

    TRY(packet_ok(sockfd, s_index));
    s_index++;

    while (1)
    {
        do
        {
            // printf("pau\n");
            // nao sei se precisa disso
            packet_recv(sockfd, buf, &opt);

            // acho que não acontece aqui, soh se ler o proprio nack??
            // while (opt.type == EMPTY)
            //     packet_recv(sockfd, buf, &opt);
            // printf("AQUI 2\n");
            // if (opt.type != DATA && opt.type != ENDTX)
            // {
            //     debug((uint)opt.type);
            //     packet_nack(sockfd, 0);
            //     packet_reset(&opt);
            // }
        } while (opt.type != DATA && opt.type != ENDTX);

        // TRY(packet_recv(sockfd, buf, &opt));

        // if (opt.type != DATA || opt.type != ENDTX)
        //     return RETURN_ERROR;

        if (opt.type == ENDTX)
            break;

        // se tipo == DATA
        fwrite(buf, sizeof(char), opt.size, file);

        TRY(packet_ack(sockfd, 0));
        s_index++;

        // reset??
        // packet_reset(&opt);
    }

    fclose(file);

    //     if (opt.type == ERROR)
    //         return -1;

    return RETURN_SUCCESS;
}