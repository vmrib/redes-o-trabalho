#pragma once

#define TRY(expr)         \
    do                    \
    {                     \
        if ((expr) == -1) \
            return -1;    \
    } while (0)
