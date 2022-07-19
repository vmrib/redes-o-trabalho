#include "raw_socket.h"
#include "protocol.h"
#include <string.h>

int main(int argc, char const *argv[])
{
    int socket = rs_socket("lo");
    char *buf = "teste";

    protocol_send(socket, buf, strlen(buf) + 1, OK);

    rs_close(socket);

    return 0;
}
