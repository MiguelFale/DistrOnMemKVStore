#include "table-private.h"
#include "network-client-private.h"
#include "table-server-private.h"
#include "table_skel.h"
#include <poll.h>

#define NUM_CLIENTES 5

int main(int argc, char *argv[]) {

	// Verificacao basica dos args
	if ((argc != 3) && (argc != 5)) {
		printf(
				"Uso: ./table-server <nlists> <port_primary> <ip_backup> <port_backup>\n");
		printf("Exemplo de uso (sem replicacao): ./table_server 12 12345\n");
		printf(
				"Exemplo de uso (com replicacao): ./table_server 12 12345 10.101.200.30 12346\n");
		return 0;
	}

	int res, nbytes; //nbytes nao utilizado
	int lst_sock; //listening socket
	int con_sock; //socket de conexao auxiliar
	int sock_sec; //socket do servidor secundario
	int n_lists = atoi(argv[1]); //numero de listas da tabela hash
	char ip[INET_ADDRSTRLEN]; // auxiliar para enderecos IP
	int backup = 0; //indica se este servidor eh replicado
	int ok_backup = 0; //indica se uma actualizacao foi bem sucedida

	char caratere; /* 'c' ou 's' dependendo de quem envia o pedido
	 * (c = cliente, s = servidor)
	 */

	//variaveis cliente
	struct sockaddr_in client;
	socklen_t size_client;
	int intCliente;
	char *buff_to_msg = NULL;

	//variaveis servidor
	int intServidor;
	char *msg_to_buff = NULL;
	struct server_t *myServer = (struct server_t *) malloc(
			sizeof(struct server_t));
	struct message_t *msg_client;
	struct message_t *content = (struct message_t *) malloc(
			sizeof(struct message_t));
	//criacao da tabela hash
	table_skel_init(n_lists);

	pthread_t back;
	struct back_input *mystruct;

	//ignorar sinais do tipo SIGPIPE
	signal(SIGPIPE, SIG_IGN);

	myServer->primario = 2;

	// Se for com replicacao liga-se ao backup
	if (argc == 5) {
		strcpy(ip, argv[3]);
		strcat(ip, ":");
		strcat(ip, argv[4]);
		myServer->address_port_sec = ip;
		myServer->primario = 1;
		sock_sec = ligar_backup(argv[3], argv[4]);

		if (sock_sec > 0)
			backup = 1;
	}

	//fazer conexao para o server
	// Cria socket TCP
	if ((lst_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Erro ao criar socket");
		return -1;
	}

	// Preenche estrutura server para bind
	myServer->sock_serv.sin_family = AF_INET;
	myServer->sock_serv.sin_addr.s_addr = htonl(INADDR_ANY);
	myServer->sock_serv.sin_port = htons(atoi(argv[2]));
	memset(myServer->sock_serv.sin_zero, 0,
			sizeof(myServer->sock_serv.sin_zero));

	int sim;

	//Permite reutilização do socket
	sim = 1;

	if (setsockopt(lst_sock, SOL_SOCKET, SO_REUSEADDR, (int*) &sim, sizeof(sim))
			< 0) {
		perror("SO_REUSEADDR setsockopt error");
	}

	// Faz bind
	if (bind(lst_sock, (struct sockaddr *) &myServer->sock_serv,
			sizeof(myServer->sock_serv)) < 0) {
		perror("Erro ao fazer bind");
		close(lst_sock);
		return -1;
	}

	// Faz listen
	if (listen(lst_sock, 0) < 0) {
		perror("Erro ao executar listen");
		close(lst_sock);
		return -1;
	}

	printf("\nServidor 'a espera de dados\n\n");

	struct pollfd my_pfds[NUM_CLIENTES + 2]; //listening socket + stdin + NUM_CLIENTES
	memset(my_pfds, 0, sizeof(struct pollfd) * (NUM_CLIENTES + 2));

	int i;

	//preenche listening socket + stdin + NUM_CLIENTES
	for (i = 0; i < NUM_CLIENTES + 2; i++) {
		if (i == 0) {
			my_pfds[i].fd = lst_sock;
			my_pfds[i].events = POLLIN;
			continue;
		} else if (i == 1) {
			my_pfds[i].fd = fileno(stdin);
			my_pfds[i].events = POLLIN;
			continue;
		}
		my_pfds[i].fd = -1;
	}

	int auxic, auxis;
	char in[100];

	//ciclo infinito
	//fazendo com que o servidor fique à espera de novas ligacoes dos clientes
	while (1) {

		if (poll(my_pfds, NUM_CLIENTES + 2, -1) > 0) {

			//se for uma ligacao nova
			if (my_pfds[0].revents & POLLIN) {

				//inicializa a estrutura a 0 por precaução
				memset(&client, 0, sizeof(client));
				size_client = sizeof(client);

				//aceita a ligacao
				con_sock = accept(lst_sock, (struct sockaddr *) &client,
						&size_client);

				//informacoes do cliente
				if (inet_ntop(AF_INET, &(client.sin_addr), ip, INET_ADDRSTRLEN)
						== NULL) {
					printf("Erro ao converter IP\n");
					continue;
				}

				printf("---> (%s, %d) ligou-se.\n", ip, ntohs(client.sin_port));

				//registar o tipo de evento de interesse
				for (i = 2; i < NUM_CLIENTES + 2; i++) {
					if (my_pfds[i].fd == -1) {
						my_pfds[i].fd = con_sock;
						my_pfds[i].events = POLLIN;
						break;
					}
				}

			} //if my_pfds[0].revents & POLLIN

			if (my_pfds[1].revents & POLLIN) {
				scanf(" %s", in);
				if (strncmp(in, "print", 5) == 0) {

					if (myServer->primario == 1)
						printf("O servidor eh primario\n");
					else
						printf("O servidor eh secundario\n");

					//construir mensagem
					content->opcode = OC_GET_KEYS;
					res = invoke(content);

					if (content->content.result != -1) {

						char **mykeys;
						int mysize;

						for (i = 0; content->content.keys[i] != NULL; i++)
							; //TENHO O SIZE
						mysize = i;

						mykeys = (char **) malloc(sizeof(char *) * (i + 1));

						for (i = 0; content->content.keys[i] != NULL; i++) { //METO AS KEYS PARA UM POINTER
							mykeys[i] = malloc(
									sizeof(char)
											* (strlen(content->content.keys[i])
													+ 1));
							strcpy(mykeys[i], content->content.keys[i]);
						}

						table_free_keys(content->content.keys);

						for (i = 0; i < mysize; i++) { //METO
							content->opcode = OC_GET;
							content->content.key = strdup(mykeys[i]);
							res = invoke(content);
							printf(" - data: %s key: %s\n",
									(char *) content->content.value->data,
									mykeys[i]);
						}

					} else
						printf("Nao existem keys!!\n");
				}

			} //if my_pfds[1].revents & POLLIN

			for (i = 2; i < NUM_CLIENTES + 2; i++) {

				if (my_pfds[i].fd < 0)
					continue;

				if (my_pfds[i].revents & POLLIN) { // dados disponiveis no cliente i

					// Lê o caratere indicando se eh cliente ou servidor
					if ((nbytes = read(my_pfds[i].fd, &caratere, sizeof(char)))
							!= sizeof(char)) {
						perror("Erro ao receber caratere");
						close(my_pfds[i].fd);
						my_pfds[i].fd = -1;
						continue;
					}

					//inicializa a estrutura a 0 por precaução
					memset(&client, 0, sizeof(client));
					size_client = sizeof(client);

					if (getpeername(my_pfds[i].fd, (struct sockaddr *) &client,
							&size_client) == -1) {
						perror("Erro ao identificar quem enviou o pedido");
						close(my_pfds[i].fd);
						my_pfds[i].fd = -1;
						continue;
					}

					if (inet_ntop(AF_INET, &(client.sin_addr), ip,
							INET_ADDRSTRLEN) == NULL) {
						printf("Erro ao converter IP\n");
						close(my_pfds[i].fd);
						my_pfds[i].fd = -1;
						continue;
					}

					printf("Servidor %s recebeu pedido de %s (%s, %d)\n",
							myServer->primario == 1 ? "primario" : "secundario",
							caratere == 'c' ? "cliente" : "servidor primario",
							ip, ntohs(client.sin_port));

					// Lê o intCliente enviado pelo cliente do socket referente a conexao
					if ((nbytes = read(my_pfds[i].fd, &auxic, sizeof(int)))
							!= sizeof(int)) {
						printf("Erro ao receber numero do %s",
								(caratere == 'c') ? "cliente" : "servidor");
						perror("Erro ao receber numero do cliente");
						close(my_pfds[i].fd);
						my_pfds[i].fd = -1;
						continue;
					}

					//converte do formato de rede que recebeu do read
					intCliente = ntohl(auxic);

					//reserva memória para buff_to_msg size do size que o cliente mandou
					buff_to_msg = (char*) malloc(intCliente);

					// Lê string enviada pelo cliente do socket referente a conexao
					if ((nbytes = read_all(my_pfds[i].fd, buff_to_msg,
							intCliente)) != intCliente) {
						perror("Erro ao receber dados do cliente (buffer).");
						printf("Erro ao receber dados do %s (buffer)",
								(caratere == 'c') ? "cliente" : "servidor");
						free(buff_to_msg);
						close(my_pfds[i].fd);
						my_pfds[i].fd = -1;
						continue;
					}

					//transformar buffer em mensagem
					msg_client = buffer_to_message(buff_to_msg, intCliente);

					if (backup) {

						// so tem logica atualizar o secundario se cliente inseriu:
						switch (msg_client->opcode) {

						case OC_PUT:
						case OC_DEL:
						case OC_COND_PUT:

							mystruct = (struct back_input *) malloc(
									sizeof(struct back_input));

							mystruct->sock_sec = sock_sec;
							mystruct->msg_client = msg_client;

							pthread_create(&back, NULL,
									(void *) atualizar_backup,
									(void *) mystruct);

							pthread_join(back, (void *) &ok_backup);

							free(mystruct);

							if (ok_backup < 0) {
								printf(
										"O servidor secundario morreu. Vou atuar sozinho...\n");
								backup = 0;
								sock_sec = -1;
							}

							break;
						}
					}

					//chama invoke para fazer as operações consoante a mensagem do cliente
					//retorna o resultado na variável res
					res = invoke(msg_client);

					free(buff_to_msg);

					if (res == -1) {
						close(my_pfds[i].fd);
						my_pfds[i].fd = -1;
						break;
					}

					//com msg_client alterado pelo invoke,
					//traduz struct message_t para um buffer
					//e guarda-se o size da mensagem a enviar em intServidor
					intServidor = message_to_buffer(msg_client, &msg_to_buff);

					free_message(msg_client);

					auxis = htonl(intServidor);

					//envia o inteiro
					if ((nbytes = write(my_pfds[i].fd, &auxis, sizeof(int)))
							!= sizeof(int)) {
						perror("Erro ao enviar dados ao cliente (inteiro)\n");
						printf("Erro ao enviar dados ao %s (inteiro)",
								(caratere == 'c') ? "cliente" : "servidor");
						free(msg_to_buff);
						close(my_pfds[i].fd);
						my_pfds[i].fd = -1;
						continue;
					}

					//envia o buffer
					if ((nbytes = write_all(my_pfds[i].fd, msg_to_buff,
							intServidor)) != intServidor) {
						perror("Erro ao enviar dados ao cliente (buffer)\n");
						printf("Erro ao enviar dados ao %s (buffer)",
								(caratere == 'c') ? "cliente" : "servidor");
						free(msg_to_buff);
						close(my_pfds[i].fd);
						my_pfds[i].fd = -1;
						continue;
					}

					free(msg_to_buff);

				} // if my_pfds[i].revents & POLLIN

				else if (my_pfds[i].revents & (POLLHUP | POLLERR)) {
					close(my_pfds[i].fd);
					my_pfds[i].fd = -1;
				}
			} //for i = 2; i < NUM_CLIENTES + 2; i++
		} //if do poll(my_pfds, NUM_CLIENTES + 2, -1)
	}

	table_skel_destroy();
	close(lst_sock);
	return 0;
}

