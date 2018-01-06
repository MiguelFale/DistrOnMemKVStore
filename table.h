#ifndef _TABLE_H
#define _TABLE_H

#include "list-private.h"

struct table_t;
/* A definir pelo grupo em table-private.h */

/* Funcao para criar/inicializar uma nova tabela hash, com n
 * linhas(n = modulo da funcao hash)
 */
struct table_t *table_create(int n);

/* Eliminar/desalocar toda a memoria
 */
void table_destroy(struct table_t *table);

/* Funcao para adicionar um elemento na tabela.
 * A funcao vai copiar a key (string) e os dados num
 * espaco de memoria alocado por malloc().
 * Se a key ja existe, vai substituir essa entrada 
 * pelos novos dados.
 * Devolve 0 (ok) ou -1 (out of memory)
 */
int table_put(struct table_t *table, char *key, struct data_t *data);

/* Funcao para adicionar um elemento na tabela caso a chave
 * associada ao elemento nao exista na tabela. Caso a chave
 * exista, retorna erro.
 * A funcao vai copiar a key (string) e os dados num
 * espaco de memoria alocado por malloc().
 * Devolve 0 (ok) ou -1 (chave existe ou out of memory)
 */
int table_conditional_put(struct table_t *table, char *key, struct data_t *data);

/* Funcao para obter um elemento da tabela.
 * O argumento key indica a chave da entrada da tabela. 
 * A funcao aloca memoria para armazenar uma *COPIA*
 * dos dados da tabela e retorna este endereco. 
 * O programa a usar essa funcao deve
 * desalocar (utilizando free()) esta memoria.
 * Em caso de erro, devolve NULL
 */
struct data_t *table_get(struct table_t *table, char *key);

/* Funcao para remover um elemento da tabela. Vai desalocar
 * toda a memoria alocada na respetiva operacao table_put().
 * Devolve: 0 (ok), -1 (key not found)
 */
int table_del(struct table_t *table, char *key);

/* Devolve o numero de elementos da tabela.
 */
int table_size(struct table_t *table);

/* Devolve um array de char * com a copia de todas as 
 * keys da tabela, e um ultimo elemento a NULL.
 */
char **table_get_keys(struct table_t *table);

/* Desaloca a memoria alocada por table_get_keys()
 */
void table_free_keys(char **keys);

#endif
