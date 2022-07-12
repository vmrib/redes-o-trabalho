#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "ConexaoRawSocket.h"

int main(int argc, char const *argv[])
{
    int teste = ConexaoRawSocket("lo", 2);
    char buf[1024];

    if (read(teste, buf, 1024 * sizeof(char)) == -1)
    {
        perror("Read Error");
        exit(-1);
    }

    printf("Mensagem recebida: %s\n", buf);

    close(teste);
    return 0;
}
