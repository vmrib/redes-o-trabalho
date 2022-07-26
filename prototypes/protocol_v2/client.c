#include "raw_socket.h"
#include "packet.h"
#include <string.h>
#include <stdio.h>

int main(int argc, char const *argv[])
{
    int socket = rs_socket("lo");
    char *buf = "teste";

    packet_options_t c = {
        .size = strlen(buf) + 1,
        .type = DATA,
        .index = 0,
    };
    if (packet_send(socket, buf, c) == -1)
    {
        perror("Error");
        return 1;
    }

    rs_close(socket);

    return 0;
}
