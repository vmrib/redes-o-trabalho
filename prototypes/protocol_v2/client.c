#include "raw_socket.h"
#include "packet.h"
#include "prot_client.h"
#include <string.h>
#include <stdio.h>

int main(int argc, char const *argv[])
{
    packet_options_t opt;
    int socket = rs_socket("lo");
    char *dirname = "teste";

    rs_set_timeout(socket, (uint)5e6);

    // opt.index = 0;
    // opt.size = strlen(dirname) + 1;
    // opt.type = CD;
    // packet_send(socket, dirname, opt);

    if (protc_cd(socket, dirname) == -1)
    {
        perror("Error");
        return 1;
    }

    rs_close(socket);

    return 0;
}
