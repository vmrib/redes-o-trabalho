#include "raw_socket.h"
#include "packet.h"
#include "prot_client.h"
#include <string.h>
#include <stdio.h>

int main(int argc, char const *argv[])
{
    int socket = rs_socket("lo");
    char *dirname = "teste";

    if (rs_set_timeout(socket, 500000000) == -1)
    {
        return 1;
    }

    if (protc_cd(socket, dirname) == -1)
    {
        perror("Error");
        return 1;
    }

    rs_close(socket);

    return 0;
}
