#include <string.h>
#include <stdio.h>

#include "prot_client.h"
#include "packet.h"
#include "error.h"
#include "debug.h"

int protc_cd(int sockfd, char *dirname)
{
    packet_options_t opt;
    char buf[PACKET_DATA_MAX_SIZE];

    do
    {
        opt.index = 0;
        opt.size = strlen(dirname) + 1;
        opt.type = CD;
        TRY(packet_send(sockfd, dirname, opt));

        TRY(packet_recv(sockfd, buf, &opt));

    } while (opt.type == NACK);

    if (opt.type == ERROR || opt.type != OK)
    {
        debug((uint)opt.type);
        return RETURN_ERROR;
    }

    return RETURN_SUCCESS;
}

int protc_ls(int sockfd, char *arg)
{
    packet_options_t opt;
    char buf[PACKET_DATA_MAX_SIZE];

    do
    {
        opt.index = 0;
        opt.size = strlen(arg) + 1;
        opt.type = LS;
        TRY(packet_send(sockfd, arg, opt));

        TRY(packet_recv(sockfd, buf, &opt));
    } while (opt.type == NACK);

    if (opt.type == ERROR || opt.type != OK)
        return RETURN_ERROR;

    while (1)
    {
        while (packet_recv(sockfd, buf, &opt) == 666)
            TRY(packet_nack(sockfd, 0));

        if (opt.type == ENDTX)
            break;

        if (opt.type != SHOW)
            return -1;

        printf("%s\n", buf);

        TRY(packet_ok(sockfd, 0));
    }

    return RETURN_SUCCESS;
}

int protc_mkdir(int sockfd, char *dirname)
{
    packet_options_t opt;
    char buf[PACKET_DATA_MAX_SIZE];

    do
    {
        opt.index = 0;
        opt.size = strlen(dirname) + 1;
        opt.type = MKDIR;
        TRY(packet_send(sockfd, dirname, opt));

        TRY(packet_recv(sockfd, buf, &opt));

    } while (opt.type == NACK);

    if (opt.type == ERROR || opt.type != OK)
        return RETURN_ERROR;

    return RETURN_SUCCESS;
}

int protc_get(int sockfd, char *filename)
{
    packet_options_t opt;
    char buf[PACKET_DATA_MAX_SIZE];

    do
    {
        opt.index = 0;
        opt.size = strlen(filename) + 1;
        opt.type = GET;
        TRY(packet_send(sockfd, filename, opt));

        TRY(packet_recv(sockfd, buf, &opt));
    } while (opt.type == NACK);

    if (opt.type == ERROR || opt.type != OK)
        return RETURN_ERROR;

    TRY(packet_recv(sockfd, buf, &opt));
    if (opt.type != FDESC)
        return RETURN_ERROR;

    // testar se conteudo de FDESC está ok. Enviar ERROR caso não.

    FILE *file = fopen(filename, "wb");
    if (!file)
        return RETURN_ERROR;

    TRY(packet_ok(sockfd, 0));

    while (1)
    {
        do
        {
            TRY(packet_recv(sockfd, buf, &opt));
            if (opt.type != DATA || opt.type != ENDTX)
                TRY(packet_nack(sockfd, 0));
        } while (opt.type != DATA || opt.type != ENDTX);

        if (opt.type == ENDTX)
            break;

        fwrite(buf, sizeof(char), opt.size, file);

        TRY(packet_ack(sockfd, 0));
    }

    fclose(file);

    return RETURN_SUCCESS;
}
