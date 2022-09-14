#pragma once

#include <errno.h>

// Erros para setar errno.
// Protocolo define caracteres para erros.
// Convém criar erros cujo código seja igual aos caracteres
// de erro do protocolo.
// Nomenclatura dos erros tentando seguir padrão do linux
// https://www.thegeekstuff.com/2010/10/linux-error-codes/
#define ENODIR 'A'      // Diretório não existe
#define EPERMISSION 'B' // Sem permissão
#define EALREADYDIR 'C' // Diretório já existe
#define ENOFILE 'D'     // Arquivo não existe

#define RETURN_SUCCESS 0
#define RETURN_ERROR -1

#define TRY(expr)         \
    do                    \
    {                     \
        if ((expr) == -1) \
            return -1;    \
    } while (0)
