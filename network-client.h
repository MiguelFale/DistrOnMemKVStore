#ifndef _NETWORK_CLIENT_H
#define _NETWORK_CLIENT_H

#include "message-private.h"

struct server_t;
//a definIr pelo grupo em network_client-private.h

/* Esta funcao deve
 * - estabelecer a ligacao com o servidor;
 * - address_port e’ uma string no formato <hostname>:<port>
 *   (exemplo: 10.10.10.10:10000)
 * - retornar toda a informacao necessaria (e.g., descritor do socket)
 *   na estrutura server_t
 */
struct server_t *network_connect(const char *address_port);

/* Esta funcao deve
 * - Obter o descritor da ligacao (socket) da estrutura server_t;
 * - enviar a mensagem msg ao servidor;
 * - receber uma resposta do servidor;
 * - retornar a mensagem obtida como resposta ou NULL em caso de erro.
 */
struct message_t *network_send_receive(struct server_t *server,
		struct message_t *msg);

/* A funcao network_close() deve fechar a ligacao estabelecida por
 * network_connect(). Se network_connect() reservou memoria, a funcao
 * deve libertar essa memoria.
 */
int network_close(struct server_t *server);

#endif
