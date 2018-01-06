#ifndef _NETWORK_CLIENT_PRIVATE_H
#define _NETWORK_CLIENT_PRIVATE_H

#include "network-client.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <netdb.h>
#include <pthread.h>

struct server_t {
	int socket;
	int primario; //1 para primario 2 para secundario
	struct sockaddr_in sock_serv;
	const char *address_port;
	const char *address_port_sec;
};

/* Responsavel por reconexao em caso de falha do servidor. 
 * 
 * @param Server estrutura server a alterar
 * @param target servidor alvo; 1 ou 2
 * @result -1 em caso de erro
 *         0 em caso de sucesso
 */
int reconnect(struct server_t *Server, int target);

/* Envia um array de bytes pela rede
 * @param sock socket
 * @param buf array de bytes a enviar
 * @param len tamanho de buf
 * @result numero de bytes escritos
 */
int write_all(int sock, char *buf, int len);

/* Le um array de bytes da rede
 * @param sock socket
 * @param buf array de bytes a ler
 * @param len tamanho de buf
 * @result numero de bytes lidos
 */
int read_all(int sock, char *buf, int len);

/* Conta o numero de palavras numa frase
 * @param str a frase
 * @return numero de palavras de str
 */
int palavras(const char *str);

/* Faz o parsing dos comandos put e/ou cput
 * @param aux string do comando
 * @param nPalavras numero de palavras
 * @return um buffer de palavras
 */
char** parsing_cput(char *aux, int nPalavras);

/* Verifica o input do utilizador
 * @param aux string do comando
 * @param nPalavras numero de palavras
 * @return
 * -1 se o input nao permitir continuar o fluxo normal de execucao
 * ou entao o respetivo opcode
 */
int inputVerify(char *aux, int nPalavras);

#endif
