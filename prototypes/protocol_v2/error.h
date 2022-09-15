#pragma once

#include <errno.h>

// Erros para setar errno.
// Protocolo define alguns caracteres para erros.
// Convém criar erros cujo código seja igual aos caracteres
// de erro do protocolo.
// Nomenclatura dos erros tentando seguir padrão do linux
// https://www.thegeekstuff.com/2010/10/linux-error-codes/
#define ENODIR 'A'      // Diretório não existe
#define EPERMISSION 'B' // Sem permissão
#define EALREADYDIR 'C' // Diretório já existe
#define ENOFILE 'D'     // Arquivo não existe

// Extensões
// #define ETIMEOUT 'F'   // Tempo de limite excedido --> JÁ DEFINIDO POR PADRÃO COMO ETIMEDOUT
#define EINTEGRITY '?' // Erro de integridade de dados (paridade, marcador, etc.)

// Tipos de retorno de funções
#define RETURN_SUCCESS 0
#define RETURN_ERROR -1

// Checa se 'expr' retornou erro. Faz com que a função atual também retorne erro.
#define TRY(expr)         \
    do                    \
    {                     \
        if ((expr) == -1) \
            return -1;    \
    } while (0)

// Igual a TRY(), mas também seta errno para errnoval
#define TRYERR(expr, errnoval) \
    do                         \
    {                          \
        if ((expr) == -1)      \
        {                      \
            errno = errnoval;  \
            return -1;         \
        }                      \
    } while (0)

// Printa mensagem de erro padrão para os erros definidos
void print_erro(unsigned int errcode);