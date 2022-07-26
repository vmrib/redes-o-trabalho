#include <string.h>

#include "prot_client.h"
#include "packet.h"
#include "error.h"

int protc_cd(int sockfd, const char *dirname)
{
    packet_options_t opt;
    opt.index = 0;
    opt.size = strlen(dirname) + 1;
    opt.type = CD;
    TRY(packet_send(sockfd, dirname, opt));

    char buf[PACKET_DATA_MAX_SIZE];
    do
    {
        TRY(packet_recv(sockfd, buf, &opt));
    } while (opt.type != NACK);

    if (opt.type == ERROR || opt.type != OK)
        return RETURN_ERROR;

    return RETURN_SUCCESS;
}

int protc_ls(int sockfd, const char *arg)
{
    packet_options_t opt;
    opt.index = 0;
    opt.size = strlen(arg) + 1;
    opt.type = LS;
    TRY(packet_send(sockfd, arg, opt));

    char buf[PACKET_DATA_MAX_SIZE];
    do
    {
        TRY(packet_recv(sockfd, buf, &opt));
    } while (opt.type != NACK);

    if (opt.type == ERROR || opt.type != OK)
        return RETURN_ERROR;

    while (1)
    {
        while (packet_recv(sockfd, buf, &opt) == 666)
        {
            opt.index = 0;
            opt.size = 0;
            opt.type = NACK;
            TRY(packet_send(sockfd, NULL, opt));
        }

        if (opt.type = ENDTX)
            break;

        if (opt.type != SHOW)
            return -1;

        printf("%s\n", buf);

        opt.index = 0;
        opt.size = 0;
        opt.type = OK;
        TRY(packet_send(sockfd, NULL, opt));
    }

    return RETURN_SUCCESS;
}
