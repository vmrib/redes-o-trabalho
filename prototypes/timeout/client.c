#include "raw_socket.h"
#include <string.h>
#include <stdlib.h>

int main(int argc, char const *argv[])
{
    char *buf_2 = aligned_alloc(32, 10 * sizeof(char));
    int trool = 2;
    char *buf = "teste com men"; // menos ue 14 bytes da pau
    int socket = rs_socket("lo");

    buf_2[0] = 'a';
    buf_2[1] = '\0';

    rs_send(socket, buf, strlen(buf) + 1);

    rs_close(socket);

    return 0;
}
