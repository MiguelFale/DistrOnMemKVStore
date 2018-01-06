#ifndef _MESSAGE_PRIVATE_H
#define _MESSAGE_PRIVATE_H

#include "message.h"
#include "list.h"
#include "list-private.h"

#define OC_RT_ERROR 99
#define OC_QUIT 98
#define CT_QUIT 97

/* Altera a ordem dos bytes em 64 bits (long long)
 * @param number o numero a usar
 */
long long swap_bytes_64(long long number);

#endif
