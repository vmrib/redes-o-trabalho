#include "raw_socket.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char const *argv[])
{
    int socket = rs_socket("lo");
    u_char buf[1024];

    rs_set_timeout(socket, (uint)2e6);
    rs_recv(socket, buf, sizeof(buf));

    printf("Mensagem recebida: [");
    for (size_t i = 0; i < 4; i++)
    {
        printf("0x%x, ");
    }
    printf("\n");

    rs_close(socket);

    return 0;
}
