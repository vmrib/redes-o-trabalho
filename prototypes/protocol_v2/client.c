#include "raw_socket.h"
#include "packet.h"
#include "prot_client.h"
#include <stdio.h>
#include <string.h>

// uint c_index = 0;

int main(int argc, char const *argv[])
{
    packet_options_t p_opt;
    int socket = rs_socket("lo");
    // char *opt1 = "-la";

    rs_set_timeout(socket, (uint)5e6);

    // p_opt.index = 0;
    // p_opt.size = strlen(opt1) + 1;
    // p_opt.type = LS;
    // packet_send(socket, opt1, p_opt);

    // if (protc_cd(socket, dirname) == -1)
    // {
    //     perror("Error");
    //     return 1;
    // }


    // linha: todo o input do usuario (ls -a)
    // comando: apenas o comando na linha (ls)
    // opt: a opcao ou argumento do comando (-a)
    char linha[64], comando[64], opt[10];

    while(1)
    {
        packet_reset(&p_opt);
        // printf("cliente_$ ");
        printf("%c[%d;%dm\ncliente_$ %c[%dm",27,1,32,27,1);
        scanf(" %[^\n]", linha);

        char *token = strtok(linha, " ");
        strcpy(comando, token);

        token = strtok(NULL, " ");
        if(token != NULL)
            strcpy(opt, token);
        else
            strcpy(opt, "\0");


        if(!strcmp(comando, "ls"))
        {
            local_ls(opt);
        }
        else if(!strcmp(comando, "cd"))
        {
            local_cd(opt);
        }
        else if(!strcmp(comando, "mkdir"))
        {
            local_mkdir(opt);
        }
        else if(!strcmp(comando, "smkdir"))
        {
            protc_mkdir(socket, opt);
        }
        else if(!strcmp(comando, "scd"))
        {
            protc_cd(socket, opt);
        }
        else if(!strcmp(comando, "sls"))
        {
            protc_ls(socket, opt);
        }
        else if(!strcmp(comando, "put"))
        {
            continue;
        }
        else if(!strcmp(comando, "get"))
        {
            continue;
        }
    }

    rs_close(socket);

    return 0;
}
