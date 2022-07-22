#pragma once

#define RETURN_SUCCESS 0
#define RETURN_ERROR -1

#define TRY(expr)         \
    do                    \
    {                     \
        if ((expr) == -1) \
            return -1;    \
    } while (0)
