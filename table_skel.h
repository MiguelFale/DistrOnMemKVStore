#ifndef _TABLE_SKEL_H 
#define _TABLE_SKEL_H

#include "message.h"

/* Inicia o skeleton da tabela.
 * O main() do servidor deve chamar esta funcao antes de usar a
 * funcao invoke(). O parametro n_lists define o numero de listas a 
 * serem usadas pela tabela mantida no servidor.
 * Retorna 0 (OK) ou -1 (erro, por exemplo OUT OF MEMORY)
 */
int table_skel_init(int n_lists);

/* Libertar toda a memoria e recursos alocados pela funcao anterior. */
int table_skel_destroy();

/* Executa uma operacao (indicada pelo opcode na msg) e retorna o
 * resultado na propria struct msg.
 * Retorna 0 (OK) ou -1 (erro, por exemplo, tabela nao incializada) */
int invoke(struct message_t *msg);

#endif
