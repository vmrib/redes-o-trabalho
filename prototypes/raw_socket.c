#include <unistd.h>
#include "ConexaoRawSocket.h"

int main(int argc, char const *argv[])
{
    int teste = ConexaoRawSocket("lo");
    close(teste);
    return 0;
}
