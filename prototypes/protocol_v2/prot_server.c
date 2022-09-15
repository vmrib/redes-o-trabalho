#include "prot_server.h"
#include "packet.h"
#include "error.h"
#include <unistd.h> // chdir
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>

uint s_index = 0;

int prots_cd(int sockfd, const char *dirname) // retorna OK, NACK ou ERRO
{
    packet_options_t opt;
    printf("%s\n", dirname);
    int resultado = chdir(dirname);

    if (!resultado)
        packet_ok(sockfd, s_index);
    else
        packet_error(sockfd, "ERRO", s_index); // descrever melhor os erros

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
        printf("Failed to run command\n");
        exit(1);
    }

    /* Read the output a line at a time - output it. */
    while (fgets(path, sizeof(path), fp) != NULL)
    {
        // printf("%ld\n", strlen(path));
        // path[strlen(path)-1] = '    ';
        printf("outro\n");
        s_index++;
        // printf("index: %d\n", s_index);
        do
        {
            printf("mandou\n");
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

            packet_reset(&opt);

            // while (packet_recv(sockfd, buf, &opt) != -1){printf("aqui\n");}
            // while (packet_recv(sockfd, buf, &opt) == -1){}
            while (opt.type != ACK)
            {
                // while (packet_recv(sockfd, buf, &opt) == -1){}
                TRY(packet_recv(sockfd, buf, &opt));
                printf("\nMensagem recebida: %s", buf);
                // printf("Tamanho: %u\nTipo: %u \nIndex = %u\n", opt.size, opt.type, opt.index);
                if (opt.type == EMPTY)
                    printf("Tipo: EMPTY\n");
                else if (opt.type == SHOW)
                    printf("Tipo: SHOW\n");
                else if (opt.type == ACK)
                    printf("Tipo: ACK\n");
                else
                    printf("Tipo: %u\n", opt.type);
                s_index = opt.index;
                // printf("index: %d\n", s_index);
                // if(opt.type == SHOW) continue;
            }
            // s_index++;
        } while (opt.type == NACK); // timeout
    }

    TRY(packet_end(sockfd, s_index));
    s_index++;
    printf("index: %d\n", s_index);

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
        TRY(packet_error(sockfd, "ERRO", s_index));
    }
    else
    {
        TRY(packet_ok(sockfd, 0));
    }
    s_index++;

    return RETURN_SUCCESS;
}

int prots_get(int sockfd)
{
}

int prots_put(int sockfd)
{
}