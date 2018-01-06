#ifndef _TABLE_PRIVATE_H
#define _TABLE_PRIVATE_H

#include "table.h"

struct table_t {
	struct list_t **bucket;
	int linhasTotal;
};

/* Transforma uma chave num indice de um array onde ficara armazenado o
 * par chave-valor
 *
 * @param string chave que vai ser transformada
 * @param n tamanho do array
 */
int hash(char *string, int n);

#endif
