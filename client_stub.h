#ifndef _CLIENT_STUB_H 
#define _CLIENT_STUB_H

#include "data.h"

struct rtable_t;
/* A definir pelo grupo em client_stub-private.h */

/* Funcao para estabelecer uma associacao com uma tabela num servidor. 
 * address_port e’ uma string no formato <hostname>:<port>.
 * retorna NULL em caso de erro
 */
struct rtable_t *rtable_bind(const char *address_port);

/* Fecha a ligacao com o servidor, desaloca toda a memoria local. 
 * Retorna 0 se tudo correr bem e -1 em caso de erro.
 */
int rtable_unbind(struct rtable_t *rtable);

/* Funcao para adicionar um elemento na tabela.
 * Se a key ja existe, vai substituir essa entrada pelos novos dados. 
 * Devolve 0 (ok) ou -1 (problemas).
 */
int rtable_put(struct rtable_t *rtable, char *key, struct data_t *data);

/* Funcao para adicionar um elemento na tabela. 
 * Se a key ja existe, retorna erro.
 * Devolve 0 (ok) ou -1 (problemas).
 */
int rtable_conditional_put(struct rtable_t *rtable, char *key,
		struct data_t *data);

/* Funcao para obter um elemento da tabela. 
 * Em caso de erro, devolve NULL.
 */
struct data_t *rtable_get(struct rtable_t *rtable, char *key);

/* Funcao para remover um elemento da tabela. Vai desalocar
 * toda a memoria alocada na respectiva operacao rtable_put(). 
 * Devolve: 0 (ok), -1 (key not found ou outro problema).
 */
int rtable_del(struct rtable_t *rtable, char *key);

/* Devolve numero de elementos da tabela. */
int rtable_size(struct rtable_t *rtable);

/* Devolve um array de char* com a copia de todas as keys da tabela, 
 * e um ultimo elemento a NULL.
 */
char **rtable_get_keys(struct rtable_t *rtable);

/* Desaloca a memoria alocada por rtable_get_keys(). */
void rtable_free_keys(char **keys);

#endif
