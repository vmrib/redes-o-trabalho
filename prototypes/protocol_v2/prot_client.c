#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "prot_client.h"
#include "packet.h"
#include "error.h"
#include "debug.h"

uint c_index = 0;

int protc_cd(int sockfd, char *dirname)
{
    packet_options_t opt;
    char buf[PACKET_DATA_MAX_SIZE];

    do
    {
        opt.index = c_index;
        opt.size = strlen(dirname) + 1;
        opt.type = CD;
        TRY(packet_send(sockfd, dirname, opt));

        while(opt.index == c_index){
            TRY(packet_recv(sockfd, buf, &opt));
        }
        // TRY(packet_recv(sockfd, buf, &opt));

    } while (opt.type == NACK); // timeout?

    c_index++; // se funcionou

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
    printf("index: %d\n", c_index);
    do
    {
        opt.index = c_index;
        opt.size = strlen(arg) + 1;
        opt.type = LS;
        TRY(packet_send(sockfd, arg, opt));

        while(opt.type == LS){
            TRY(packet_recv(sockfd, buf, &opt));
            printf("Mensagem recebida: %s\n", buf);
            printf("Tamanho: %u\nTipo: %u \nIndex = %u\n", opt.size, opt.type, opt.index);
            c_index = opt.index;
            printf("index: %d\n", c_index);
        }
        // TRY(packet_recv(sockfd, buf, &opt));
    } while (opt.type == NACK); // timeout

    c_index++;
    printf("index: %d\n", c_index);

    // printf("CLIENTE\n");
    if (opt.type == ERROR) //|| opt.type != OK)
        return RETURN_ERROR;


    while (1)
    {
        // packet_recv(sockfd, buf, &opt);
        // while (packet_recv(sockfd, buf, &opt) == 666) // ????????
        //     TRY(packet_nack(sockfd, 0));

        // if(algo) // sequencia, paridade, comando nao existente
        //     packet_nack(sockfd, 0);

        if (opt.type == ACK)
        {
            // printf("pegou o proprio\n");
            TRY(packet_recv(sockfd, buf, &opt));
            continue;
        }

        c_index = opt.index;
        printf("Mensagem recebida: %s\n", buf);
        printf("Tamanho: %u\nTipo: %u \nIndex = %u\n", opt.size, opt.type, opt.index);
        printf("index: %d\n", c_index);

        // printf("tipo: %d\n", opt.type);

        if (opt.type == ENDTX)
            break;

        if (opt.type != SHOW)
            return -1;

        printf("%s", buf); // opt.type == SHOW
        TRY(packet_ack(sockfd, c_index));
        c_index++;
        printf("index: %d\n", c_index);

        // while (packet_recv(sockfd, buf, &opt) != -1){printf("ali\n");}
        TRY(packet_recv(sockfd, buf, &opt));
        c_index = opt.index;
        printf("index: %d\n", c_index);

        // TRY(packet_ok(sockfd, 0));
    }

    return RETURN_SUCCESS;
}

int protc_mkdir(int sockfd, char *dirname)
{
    packet_options_t opt;
    char buf[PACKET_DATA_MAX_SIZE];

    do
    {
        opt.index = c_index;
        opt.size = strlen(dirname) + 1;
        opt.type = MKDIR;
        TRY(packet_send(sockfd, dirname, opt));

        TRY(packet_recv(sockfd, buf, &opt));

    } while (opt.type == NACK);

    c_index++;

    if (opt.type == ERROR || opt.type != OK)
        return RETURN_ERROR;

    return RETURN_SUCCESS;
}

int protc_get(int sockfd, char *filename)
{
    // packet_options_t opt;
    // char buf[PACKET_DATA_MAX_SIZE];

    // do
    // {
    //     opt.index = 0;
    //     opt.size = strlen(filename) + 1;
    //     opt.type = GET;
    //     TRY(packet_send(sockfd, filename, opt));

    //     TRY(packet_recv(sockfd, buf, &opt));
    // } while (opt.type == NACK);

    // while (opt.type != FDESC)

    //     if (opt.type == ERROR)
    //         return -1;
}

int local_ls(char *flag) // talvez devesse passar separado o comando e as opcoes/argumentos?
{
    // checar permissao

    // enunciado: -a ou -l (XOR)
    if(!strcmp(flag, "-a"))
        system("ls -a");
    else if(!strcmp(flag, "-l"))
        system("ls -l");
    else if(!strcmp(flag, "\0")) // sem opcoes
        system("ls");
    else
        printf("ERRO: flag '%s' nao reconhecida\n", flag);
    
    return 0;

}

int local_cd(char *dirname)
{
    chdir(dirname);
}

int local_mkdir(char *dirname)
{
    char mkdir[64] = "mkdir ";
    strcat(mkdir, dirname);
    system(mkdir);
}