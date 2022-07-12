#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "ConexaoRawSocket.h"

int main(int argc, char const *argv[])
{
    int teste = ConexaoRawSocket("lo", 2);
    char *buf[1024];

    if (write(teste, buf, 1024 * sizeof(char)) == 1)
    {
        perror("Write Error");
        exit(-1);
    }

    close(teste);
    return 0;
}
