#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ConexaoRawSocket.h"

int main(int argc, char const *argv[])
{
    int teste = ConexaoRawSocket("lo");
    char buf[1024];

    memcpy(buf, "Teste de envio de mensagem bem simples", 39);

    if (write(teste, buf, 1024 * sizeof(char)) == -1)
    {
        perror("Write Error");
        exit(-1);
    }

    close(teste);
    return 0;
}
