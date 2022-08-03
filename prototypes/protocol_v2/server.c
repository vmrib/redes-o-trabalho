#include "raw_socket.h"
#include "packet.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char const *argv[])
{
    int socket = rs_socket("lo");
    char buf[1024];
    packet_options_t c;

    rs_set_timeout(socket, (uint)2e6);

    if (packet_recv(socket, buf, &c) == -1)
    {
        perror("Error");
        return 1;
    }

    printf("Mensagem recebida: %s\n", buf);
    printf("Tamanho: %u\n Tipo: %u \n Index = %u\n", c.size, c.type, c.index);

    rs_close(socket);

    return 0;
}
