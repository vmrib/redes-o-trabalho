#include "raw_socket.h"
#include "packet.h"
#include "prot_client.h"
#include "error.h"
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

    while (1)
    {
        packet_reset(&p_opt);
        // printf("cliente_$ ");
        printf("%c[%d;%dm\ncliente_$ %c[%dm", 27, 1, 32, 27, 1);
        scanf(" %[^\n]", linha);

        char *token = strtok(linha, " ");
        strcpy(comando, token);

        token = strtok(NULL, " ");
        if (token != NULL)
            strcpy(opt, token);
        else
            strcpy(opt, "\0");

        if (!strcmp(comando, "ls"))
        {
            if (local_ls(opt) == RETURN_ERROR)
                printf("\e[31mErro: flag \'%s\' não reconhecida.\n\e[0m", opt);
        }
        else if (!strcmp(comando, "cd"))
        {
            if (local_cd(opt) == RETURN_ERROR)
                printf("\e[31mErro: não foi possível acessar diretório \'%s\'.\n\e[0m", opt);
        }
        else if (!strcmp(comando, "mkdir"))
        {
            if (local_mkdir(opt) == RETURN_ERROR)
                printf("\e[31mErro: não foi possível criar diretório \'%s\'.\n\e[0m", opt);
        }
        else if (!strcmp(comando, "smkdir"))
        {
            while (protc_mkdir(socket, opt) == RETURN_ERROR)
            {
                // se deu timeout ou os dados perderam integridade reenvia
                if (errno == ETIMEDOUT || errno == EINTEGRITY)
                    continue;

                print_erro(errno);
                break;
            }
        }
        else if (!strcmp(comando, "scd"))
        {
            while (protc_cd(socket, opt) == RETURN_ERROR)
            {
                // se deu timeout ou os dados perderam integridade reenvia
                if (errno == ETIMEDOUT || errno == EINTEGRITY)
                    continue;

                print_erro(errno);
                break;
            }
        }
        else if (!strcmp(comando, "sls"))
        {
            while (protc_ls(socket, opt) == RETURN_ERROR)
            {
                // se deu timeout ou os dados perderam integridade reenvia
                if (errno == ETIMEDOUT || errno == EINTEGRITY)
                    continue;

                print_erro(errno);
                break;
            }
        }
        else if (!strcmp(comando, "put"))
        {
            while (protc_put(socket, opt) == RETURN_ERROR)
            {
                // se deu timeout ou os dados perderam integridade reenvia
                if (errno == ETIMEDOUT || errno == EINTEGRITY)
                    continue;

                print_erro(errno);
                break;
            }
        }
        else if (!strcmp(comando, "get"))
        {
            while (protc_get(socket, opt) == RETURN_ERROR)
            {
                // se deu timeout ou os dados perderam integridade reenvia
                if (errno == ETIMEDOUT || errno == EINTEGRITY)
                    continue;

                print_erro(errno);
                break;
            }
        }
        else if (!strcmp(comando, "exit"))
        {
            break;
        }
        else
        {
            printf("\e[31mErro: comando \'%s\' não reconhecido.\e[0m\n", comando);
        }
    }

    rs_close(socket);

    return 0;
}
