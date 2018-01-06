#ifndef _TABLE_SERVER_PRIVATE_H
#define _TABLE_SERVER_PRIVATE_H

#include "network-client-private.h"

/* Usada para passar informacao ah funcao pthread_create como parametro */
struct back_input {
	int sock_sec; // socket do secundario
	struct message_t *msg_client;
};

/* Funcao responsavel pela atualizacao do servidor secundario, pelo que eh
 * apenas chamada se o servidor primario receber um pedido de escrita.
 *
 * @param args argumento a passar ah funcao
 * @return -1 em caso de erro ( secundario foi abaixo )
 *         qualquer outro valor em caso de sucesso
 */
void *atualizar_backup(void * args);

/* Funcao responsavel por fazer a ligacao entre os servidores primario e
 * secundario
 *
 * @param ip endereco do servidor secundario
 * @param port porto do servidor secundario
 * @return -1 em caso de erro ( nao foi possivel ligar ao secundario )
 *         qualquer outro valor em caso de sucesso
 */
int ligar_backup(char *ip, char *port);

#endif
