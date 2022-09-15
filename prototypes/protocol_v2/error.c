#include <stdio.h>
#include "error.h"

void print_erro(unsigned int errcode)
{
    const char *errstart = "\e[31mErro: ";
    const char *errend = "\e[0m";

    switch (errcode)
    {
    case ETIMEDOUT:
        printf("%sTempo de limite para comunicação com servidor excedido.\n%s", errstart, errend);
        break;

    case EINTEGRITY:
        printf("%sFalha na comunicação com servidor.\n%s", errstart, errend);
        break;

    case ENODIR:
        printf("%sDiretório inexistente.\n%s", errstart, errend);
        break;

    case ENOFILE:
        printf("%sArquivo inexistente.\n%s", errstart, errend);
        break;

    case EPERMISSION:
        printf("%sPermissão negada.\n%s", errstart, errend);
        break;

    case EALREADYDIR:
        printf("%sDiretório já existe.\n%s", errstart, errend);
        break;

    default:
        printf("%sfalha na operação.%s\n", errstart, errend);
        break;
    }
}