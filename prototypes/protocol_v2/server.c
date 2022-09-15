#include "raw_socket.h"
#include "packet.h"
#include "prot_server.h"
#include <stdio.h>
#include <string.h>

// uint s_index = 0;

int main(int argc, char const *argv[])
{
    int socket = rs_socket("enp2s0");
    char buf[1024];
    packet_options_t c;

    rs_set_timeout(socket, (uint)5e6);

    char linha[64], comando[64], opt[10];

    int ultimo = -1;
    while (1)
    {
        packet_reset(&c);

        // if(packet_recv(socket, buf, &c) == -1)
        //     continue;
        // while (packet_recv(socket, buf, &c) == -1){}
        packet_recv(socket, buf, &c);

        // if (c.index == ultimo)
        //     continue;
        // else
        //     ultimo = c.index;

        // printf("\n\nSERVIDOR:\n");
        // printf("Mensagem recebida: %s\n", buf);
        // printf("Tamanho: %u\nTipo: %u \nIndex = %u\n", c.size, c.type, c.index);

        switch (c.type)
        {
        case (LS):
            prots_ls(socket, buf);
            break;

        case (MKDIR):
            prots_mkdir(socket, buf);
            break;

        case (CD):
            prots_cd(socket, buf);
            break;

        case (GET):
            prots_get(socket, buf);
            break;

        default:
            break;
        }
    }

    // // printf("AQUI\n");
    // if (packet_recv(socket, buf, &c) == -1)
    // {
    //     perror("Error");
    //     // return 1;
    // }

    // while (packet_recv(socket, buf, &c) == -1)
    // {
    //     continue;
    //     // return 1;
    // }

    // printf("Mensagem recebida: %s\n", buf);
    // printf("Tamanho: %u\nTipo: %u \nIndex = %u\n", c.size, c.type, c.index);

    // packet_ok(socket, 0);

    rs_close(socket);

    return 0;
}
