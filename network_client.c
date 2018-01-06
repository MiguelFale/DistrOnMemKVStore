#include "network-client-private.h"
#include "client_stub.h"

struct server_t *network_connect(const char *address_port) {

	int sockfd;

	//cria estrutura para armazenar informacao necessaria para o cliente se ligar
	struct server_t *myServer = (struct server_t *) malloc(
			sizeof(struct server_t));

	if (myServer == NULL) {
		printf("Erro ao criar a estrutura.\n");
		return NULL;
	}

	//verificacao inicial do address_port
	char *copy = strdup(address_port);

	char *host = strtok(copy, ":");
	if (host == NULL) {
		printf("Formato incorreto!\n");
		free(copy);
		free(myServer);
		return NULL;
	}

	char *port = strtok(NULL, " \n\t");
	if (port == NULL) {
		printf("Nao especificou o porto.\n");
		free(copy);
		free(myServer);
		return NULL;
	}

	//criar socket TCP e preenche a estrutura myServer
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Erro ao criar socket TCP! ");
		free(copy);
		free(myServer);
		return NULL;
	}

	myServer->sock_serv.sin_family = AF_INET;
	myServer->sock_serv.sin_port = htons(atoi(port));

	//verifica e converte uma string para um endereco IP em formato rede
	if (inet_pton(AF_INET, host, &myServer->sock_serv.sin_addr) < 1) {
		printf("Erro ao converter IP! ");
		free(copy);
		free(myServer);
		close(sockfd);
		return NULL;
	}

	//limpar o sin_zero
	memset(myServer->sock_serv.sin_zero, 0,
			sizeof(myServer->sock_serv.sin_zero));

	//estabelece conexao com o servidor definido em myServer
	if (connect(sockfd, (struct sockaddr *) &myServer->sock_serv,
			sizeof(myServer->sock_serv)) < 0) {
		perror("Erro ao conectar-se ao servidor");
		free(copy);
		free(myServer);
		close(sockfd);
		return NULL;
	}

	myServer->socket = sockfd;
	free(copy);

	return myServer;
}

struct message_t *network_send_receive(struct server_t *server,
		struct message_t *msg) {

	int sockfd = server->socket; //obtem o descritor do socket
	int nbytes, auxi_c, auxi_s, res;

	//variaveis do cliente
	char *msg_to_buff = NULL;
	int size_pedido = message_to_buffer(msg, &msg_to_buff);

	//variaveis do servidor
	char *buff_to_msg = NULL;
	int size_resposta;
	char c = 'c';

	int resend = 0;

	auxi_c = htonl(size_pedido);

	while (1) {

		//clientes enviam 'c' ao servidor
		if ((nbytes = write(sockfd, &c, sizeof(char))) != sizeof(char)) {
			perror("Erro ao enviar dados ao servidor ('c')");
			close(sockfd);

			if (server->primario == 1) {

				while (resend < 3) {

					printf("O %s falhou; vai ligar ao %s em 5 segundos-> %s\n",
							(server->primario == 1) ? "primario" : "secundario",
							(server->primario == 1) ? "secundario" : "primario",
							(server->primario == 1) ? server->address_port_sec : server->address_port);

					server->primario = (server->primario == 1) ? 2 : 1;
					sleep(5);
					res = reconnect(server, server->primario); //tenta no secundario

					if (res == -1)
						resend++;
					else
						break;
				}
				if (resend >= 3) {
					free(msg_to_buff);
					free(buff_to_msg);
					return NULL;
				}
				continue;
			} else if (server->primario == 2) {

				while (resend < 2) {

					printf("O %s falhou; vai ligar ao %s em 5 segundos-> %s\n",
							(server->primario == 1) ? "primario" : "secundario",
							(server->primario == 1) ? "secundario" : "primario",
							(server->primario == 1) ? server->address_port_sec : server->address_port);

					server->primario = (server->primario == 1) ? 2 : 1;
					sleep(5);
					res = reconnect(server, server->primario); //tenta no secundario

					if (res == -1)
						resend++;
					else
						break;
				}
				if (resend >= 2) {
					free(msg_to_buff);
					free(buff_to_msg);
					return NULL;
				}
				continue;
			}
		}


		//ENVIAR INTEIRO COM TAMANHO DA MENSAGEM PARA O SERVIDOR
		if ((nbytes = write(sockfd, &auxi_c, sizeof(int))) != sizeof(int)) {
			perror("Erro ao enviar dados ao servidor (inteiro)");
			close(sockfd);

			if (server->primario == 1) {

				while (resend < 3) {

					printf("O %s falhou; vai ligar ao %s em 5 segundos-> %s\n",
							(server->primario == 1) ? "primario" : "secundario",
							(server->primario == 1) ? "secundario" : "primario",
							(server->primario == 1) ? server->address_port_sec : server->address_port);

					server->primario = (server->primario == 1) ? 2 : 1;
					sleep(5);
					res = reconnect(server, server->primario); //tenta no secundario

					if (res == -1)
						resend++;
					else
						break;
				}
				if (resend >= 3) {
					free(msg_to_buff);
					free(buff_to_msg);
					return NULL;
				}
				continue;
			} else if (server->primario == 2) {

				while (resend < 2) {

					printf("O %s falhou; vai ligar ao %s em 5 segundos-> %s\n",
							(server->primario == 1) ? "primario" : "secundario",
							(server->primario == 1) ? "secundario" : "primario",
							(server->primario == 1) ? server->address_port_sec : server->address_port);

					server->primario = (server->primario == 1) ? 2 : 1;
					sleep(5);
					res = reconnect(server, server->primario); //tenta no secundario

					if (res == -1)
						resend++;
					else
						break;
				}
				if (resend >= 2) {
					free(msg_to_buff);
					free(buff_to_msg);
					return NULL;
				}
				continue;
			}
		}

		//Enviar mensagem
		if ((nbytes = write_all(sockfd, msg_to_buff, size_pedido))
				!= size_pedido) {
			perror("Erro ao enviar dados ao servidor (buffer)");
			close(sockfd);

			if (server->primario == 1) {

				while (resend < 3) {

					printf("O %s falhou; vai ligar ao %s em 5 segundos-> %s\n",
							(server->primario == 1) ? "primario" : "secundario",
							(server->primario == 1) ? "secundario" : "primario",
							server->address_port_sec);

					server->primario = (server->primario == 1) ? 2 : 1;
					sleep(5);
					res = reconnect(server, server->primario); //tenta no secundario

					if (res == -1)
						resend++;
					else
						break;
				}
				if (resend >= 3) {
					free(msg_to_buff);
					free(buff_to_msg);
					return NULL;
				}
				continue;
			} else if (server->primario == 2) {

				while (resend < 2) {

					printf("O %s falhou; vai ligar ao %s em 5 segundos-> %s\n",
							(server->primario == 1) ? "primario" : "secundario",
							(server->primario == 1) ? "secundario" : "primario",
							server->address_port_sec);

					server->primario = (server->primario == 1) ? 2 : 1;
					sleep(5);
					res = reconnect(server, server->primario); //tenta no secundario

					if (res == -1)
						resend++;
					else
						break;
				}
				if (resend >= 2) {
					free(msg_to_buff);
					free(buff_to_msg);
					return NULL;
				}
				continue;
			}
		}

		//Ler um inteiro do servidor
		if ((nbytes = read(sockfd, &auxi_s, sizeof(int))) != sizeof(int)) { //
			perror("Erro ao ler dados ao servidor (inteiro)");
			close(sockfd);

			if (server->primario == 1) {

				while (resend < 3) {

					printf("O %s falhou; vai ligar ao %s em 5 segundos-> %s\n",
							(server->primario == 1) ? "primario" : "secundario",
							(server->primario == 1) ? "secundario" : "primario",
							server->address_port_sec);
					server->primario = (server->primario == 1) ? 2 : 1;
					sleep(5);
					res = reconnect(server, server->primario); //tenta no secundario

					if (res == -1)
						resend++;
					else
						break;
				}
				if (resend >= 3) {
					free(msg_to_buff);
					free(buff_to_msg);
					return NULL;
				}
				continue;
			} else if (server->primario == 2) {

				while (resend < 2) {

					printf("O %s falhou; vai ligar ao %s em 5 segundos-> %s\n",
							(server->primario == 1) ? "primario" : "secundario",
							(server->primario == 1) ? "secundario" : "primario",
							server->address_port_sec);

					server->primario = (server->primario == 1) ? 2 : 1;
					sleep(5);
					res = reconnect(server, server->primario); //tenta no secundario

					if (res == -1)
						resend++;
					else
						break;
				}
				if (resend >= 2) {
					free(msg_to_buff);
					free(buff_to_msg);
					return NULL;
				}
				continue;
			}
		}

		size_resposta = ntohl(auxi_s);

		buff_to_msg = (char *) malloc(size_resposta);

		//Ler o buffer do servidor
		if ((nbytes = read_all(sockfd, buff_to_msg, size_resposta))
				!= size_resposta) {
			perror("Erro ao receber dados do servidor (buffer)");
			close(sockfd);
			free(buff_to_msg); //isto serve para se ele errar no read_all ele libertar

			if (server->primario == 1) {

				while (resend < 3) {

					printf("O %s falhou; vai ligar ao %s em 5 segundos-> %s\n",
							(server->primario == 1) ? "primario" : "secundario",
							(server->primario == 1) ? "secundario" : "primario",
							server->address_port_sec);

					server->primario = (server->primario == 1) ? 2 : 1;
					sleep(5);
					res = reconnect(server, server->primario); //tenta no secundario

					if (res == -1)
						resend++;
					else
						break;
				}
				if (resend >= 3) {
					free(msg_to_buff);
					free(buff_to_msg);
					return NULL;
				}
				continue;
			} else if (server->primario == 2) {

				while (resend < 2) {

					printf("O %s falhou; vai ligar ao %s em 5 segundos-> %s\n",
							(server->primario == 1) ? "primario" : "secundario",
							(server->primario == 1) ? "secundario" : "primario",
							server->address_port_sec);

					server->primario = (server->primario == 1) ? 2 : 1;
					sleep(5);
					res = reconnect(server, server->primario); //tenta no secundario

					if (res == -1)
						resend++;
					else
						break;
				}
				if (resend >= 2) {
					free(msg_to_buff);
					free(buff_to_msg);
					return NULL;
				}
				continue;
			}
		}
		break;
	}

	free(msg_to_buff);

	//transformar buffer em mensagem, pronta para ser lida pelo cliente
	struct message_t * myMessage = buffer_to_message(buff_to_msg,
			size_resposta);
	free(buff_to_msg);

	return myMessage;

}

int network_close(struct server_t *server) {

	if (close(server->socket) < 0) {
		perror("Erro ao fechar socket");
		free(server);
		return -1;
	}

	free(server);

	return 0;
}
