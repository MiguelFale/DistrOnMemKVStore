#ifndef _ENTRY_H
#define _ENTRY_H

#include "data.h"

/* Esta estrutura define o par chave-valor para a tabela
 */
struct entry_t {
	char *key; /* String, (char* terminado por '\0') */
	long long timestamp; /* Por agora vai ter valor igual a 0 */
	struct data_t *value; /* Bloco de dados */
};

/* Funcao que cria um novo par chave-valor (isto e, que inicializa
 * a estrutura e aloca a memoria necessaria).
 */
struct entry_t *entry_create(char *key, struct data_t *data);

/* Funcao que destroi um par chave-valor e liberta toda a memoria.
 */
void entry_destroy(struct entry_t *entry);

/* Funcao que duplica um par chave-valor. */
struct entry_t *entry_dup(struct entry_t *entry);

#endif
