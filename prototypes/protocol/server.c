#include "raw_socket.h"
#include "protocol.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char const *argv[])
{
    int socket = rs_socket("lo");
    char buf[1024];

    rs_set_timeout(socket, (uint)2e6);
    protocol_recv(socket, buf, sizeof(buf));

    printf("Mensagem recebida: %s\n", buf);

    rs_close(socket);

    return 0;
}
