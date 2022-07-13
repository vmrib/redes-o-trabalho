#include "raw_socket.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char const *argv[])
{
    int socket = rs_socket("lo");
    char buf[1024];

    rs_set_timeout(socket, (uint)2e6);
    rs_recv(socket, buf, sizeof(buf));

    printf("Mensagem rebebida: %s\n", buf);

    return 0;
}
