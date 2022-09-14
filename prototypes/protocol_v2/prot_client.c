#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "prot_client.h"
#include "packet.h"
#include "error.h"
#include "debug.h"

// uint c_index = 0;

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

        // while (opt.index == c_index)
        // {
        //     TRY(packet_recv(sockfd, buf, &opt));
        // }
        TRY(packet_recv(sockfd, buf, &opt));

    } while (opt.type == NACK); // timeout?

    // c_index++; // se funcionou

    if (opt.type == ERROR || opt.type != OK)
    {
        // debug((uint)opt.type);
        return RETURN_ERROR;
    }

    return RETURN_SUCCESS;
}

int protc_ls(int sockfd, char *arg)
{
    packet_options_t opt;
    char buf[PACKET_DATA_MAX_SIZE];
    uint c_index = 0;

    // debug(c_index);
    do
    {
        opt.index = c_index;
        opt.size = strlen(arg) + 1;
        opt.type = LS;
        TRY(packet_send(sockfd, arg, opt));

        // printf("AQUI 0\n");

        packet_reset(&opt);

        while (opt.type == EMPTY)
        {
            TRY(packet_recv(sockfd, buf, &opt));
            // printf("\nMensagem recebida: %s", buf);
            // printf("\n", buf);
            // printf("Tamanho: %u\nTipo: %u \nIndex = %u\n", opt.size, opt.type, opt.index);
            c_index = opt.index;
            // printf("index: %d\n", c_index);
        }
        // TRY(packet_recv(sockfd, buf, &opt));
    } while (opt.type == NACK); // timeout

    c_index++;
    // printf("index: %d\n", c_index);
    // debug(c_index);

    // printf("AQUI 1\n");

    while (1)
    {
        // printf("AQUI 2\n");
        // packet_recv(sockfd, buf, &opt);
        // while (packet_recv(sockfd, buf, &opt) == 666) // ????????
        //     TRY(packet_nack(sockfd, 0));

        // if(algo) // sequencia, paridade, comando nao existente
        //     packet_nack(sockfd, 0);

        // if (opt.type == ACK)
        // {
        //     // printf("pegou o proprio\n");
        //     TRY(packet_recv(sockfd, buf, &opt));
        //     continue;
        // }
        // printf("TIPO: %u\n", opt.type);
        if (opt.type == EMPTY)
        {
            TRY(packet_recv(sockfd, buf, &opt));
            continue;
        }

        if (opt.type == ERROR)
        {
            // Tratar ?? Erro enviado propositalmente pelo server
            return RETURN_ERROR;
        }

        // if (opt.type != SHOW && opt.type != ENDTX)
        // {
        //     // Bizarro, panico
        //     printf("Bizarro\n");
        //     debug((uint)opt.type);
        //     return RETURN_ERROR;
        // }

        // printf("AQUI 3\n");

        c_index = opt.index;
        // printf("Mensagem recebida: %s\n", buf);
        // printf("Tamanho: %u\nTipo: %u \nIndex = %u\n", opt.size, opt.type, opt.index);
        // printf("index: %d\n", c_index);

        // printf("tipo: %d\n", opt.type);

        if (opt.type == ENDTX)
            break;

        // printf("AQUI 4\n");

        // if (opt.type != SHOW)
        //     return RETURN_ERROR;

        if (opt.type == SHOW)
        {
            // printf("SHOW: ");
            printf("%s", buf); // opt.type == SHOW
            TRY(packet_ack(sockfd, c_index));
        }
        // c_index++;
        // printf("index: %d\n", c_index);
        // debug(c_index);

        else
        {
            TRY(packet_nack(sockfd, c_index));
        }

        packet_reset(&opt);

        // while (packet_recv(sockfd, buf, &opt) != -1){printf("ali\n");}
        TRY(packet_recv(sockfd, buf, &opt));
        c_index = opt.index;
        // debug(c_index);

        // TRY(packet_ok(sockfd, 0));

        // printf("AQUI 5\n");
    }

    packet_reset(&opt);
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

    // c_index++;

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

        // printf(" ================================== \n");
        TRY(packet_recv(sockfd, buf, &opt));
        // printf(" ================================== \n");

    } while (opt.type == NACK);

    // printf("FOI 1\n");

    // server envia ou não ok antes?
    // if (opt.type == ERROR || opt.type != OK)
    //     return RETURN_ERROR;

    // TRY(packet_recv(sockfd, buf, &opt));
    if (opt.type != FDESC)
        return RETURN_ERROR;

    // testar se conteudo de FDESC está ok. Enviar ERROR caso não.

    FILE *file = fopen(filename, "wb");
    if (!file)
        return RETURN_ERROR;

    TRY(packet_ok(sockfd, 0));
    // printf("AQUI 1\n");

    while (1)
    {
        do
        {
            // printf("pau\n");
            // nao sei se precisa disso
            TRY(packet_recv(sockfd, buf, &opt));

            while (opt.type == EMPTY)
                TRY(packet_recv(sockfd, buf, &opt));
            // printf("AQUI 2\n");
            if (opt.type != DATA && opt.type != ENDTX)
            {
                debug((uint)opt.type);
                TRY(packet_nack(sockfd, 0));
            }
        } while (opt.type != DATA && opt.type != ENDTX);

        // TRY(packet_recv(sockfd, buf, &opt));

        // if (opt.type != DATA || opt.type != ENDTX)
        //     return RETURN_ERROR;

        if (opt.type == ENDTX)
            break;

        fwrite(buf, sizeof(char), opt.size, file);

        TRY(packet_ack(sockfd, 0));
    }

    fclose(file);

    //     if (opt.type == ERROR)
    //         return -1;

    return RETURN_SUCCESS;
}

int protc_put(int sockfd, char *filename)
{
    packet_options_t opt, data_opt;
    char buf[PACKET_DATA_MAX_SIZE], data_buf[PACKET_DATA_MAX_SIZE];

    FILE *file = fopen(filename, "rb");
    if (!file)
        return RETURN_ERROR;

    fseek(file, 0, SEEK_END);      // seek to end of file
    size_t filesize = ftell(file); // get current file pointer
    fseek(file, 0, SEEK_SET);      // seek back to beginning of file

    // Envia PUT
    do
    {
        opt.index = 0;
        opt.size = strlen(filename) + 1;
        opt.type = PUT;
        TRY(packet_send(sockfd, filename, opt));

        // printf(" ================================== \n");
        TRY(packet_recv(sockfd, buf, &opt));
        // printf(" ================================== \n");

    } while (opt.type == NACK);

    if (opt.type == ERROR)
        return RETURN_ERROR;

    // envia FDESC
    do
    {
        opt.index = 0;
        opt.size = sizeof(size_t);
        opt.type = FDESC;
        TRY(packet_send(sockfd, &filesize, opt));

        // printf(" ================================== \n");
        TRY(packet_recv(sockfd, buf, &opt));
        // printf(" ================================== \n");

    } while (opt.type == NACK);

    if (opt.type == ERROR)
        return RETURN_ERROR;

    data_opt.size = 1; // gambiarra

    // envia dados
    while (1)
    {
        while (opt.type == EMPTY)
        {
            TRY(packet_recv(sockfd, buf, &opt));
            continue;
        }

        while (opt.type == NACK)
        {
            TRY(packet_send(sockfd, data_buf, data_opt));
            // printf(" ================================== \n");
            TRY(packet_recv(sockfd, buf, &opt));
        }

        if (opt.type == ERROR)
            return RETURN_ERROR;

        if (!data_opt.size)
            break;

        // memcpy(last_buf, buf, opt.size);
        // last_opt = opt;

        data_opt.size = fread(data_buf, sizeof(char), PACKET_DATA_MAX_SIZE - 5, file);
        data_opt.index = 0;
        data_opt.type = DATA;
        TRY(packet_send(sockfd, data_buf, data_opt));

        TRY(packet_recv(sockfd, buf, &opt));
    }

    // envia ENDTX
    do
    {
        TRY(packet_end(sockfd, 0));

        // printf(" ================================== \n");
        TRY(packet_recv(sockfd, buf, &opt));
        // printf(" ================================== \n");

    } while (opt.type == NACK);

    if (opt.type == ERROR)
        return RETURN_ERROR;

    fclose(file);

    return RETURN_SUCCESS;
}

int local_ls(char *flag) // talvez devesse passar separado o comando e as opcoes/argumentos?
{
    // checar permissao

    // enunciado: -a ou -l (XOR)
    if (!strcmp(flag, "-a"))
        TRY(system("ls -a"));
    else if (!strcmp(flag, "-l"))
        TRY(system("ls -l"));
    else if (!strcmp(flag, "\0")) // sem opcoes
        TRY(system("ls"));
    else
    {
        // printf("ERRO: flag '%s' nao reconhecida\n", flag);
        return RETURN_ERROR;
    }

    return RETURN_SUCCESS;
}

int local_cd(char *dirname)
{
    TRY(chdir(dirname));
    return RETURN_SUCCESS;
}

int local_mkdir(char *dirname)
{
    char mkdir[64] = "mkdir ";
    strcat(mkdir, dirname);
    TRY(system(mkdir));

    return RETURN_SUCCESS;
}