#include "raw_socket.h"
#include <string.h>

int main(int argc, char const *argv[])
{
    int socket = rs_socket("lo");
    char *buf = "Teste com timeout";

    rs_send(socket, buf, strlen(buf) + 1);

    return 0;
}
