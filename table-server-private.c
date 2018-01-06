#include "table-server-private.h"

void *atualizar_backup(void *args) {

	struct back_input *myargs = (struct back_input *) args;

	char s = 's'; // Indica que o pedido é feito pelo servidor primario

	int var; // Armazena o resultado da operacao de atualizacao (numero de bytes escritos)

	char *msg_buf = NULL;

	int size_buf = message_to_buffer(myargs->msg_client, &msg_buf);

	int auxi = htonl(size_buf);

	// Envia caratere ao servidor secundario
	if ((var = write(myargs->sock_sec, &s, sizeof(char))) != sizeof(char)) {
		perror("Erro ao enviar dados ao servidor secundario (caratere)");
		free(msg_buf);
		close(myargs->sock_sec);
		myargs->sock_sec = -1;
		return (void *) -1;
	}

	// Envia inteiro indicador do tamanho do buffer ao servidor secundario
	if ((var = write(myargs->sock_sec, &auxi, sizeof(int))) != sizeof(int)) {
		perror("Erro ao enviar dados ao servidor secundario (inteiro)");
		free(msg_buf);
		close(myargs->sock_sec);
		myargs->sock_sec = -1;
		return (void *) -1;
	}

	// Envia buffer ao servidor secundario
	if ((var = write_all(myargs->sock_sec, msg_buf, size_buf)) != size_buf) {
		perror("Erro ao enviar dados ao servidor secundario (buffer)");
		free(msg_buf);
		close(myargs->sock_sec);
		myargs->sock_sec = -1;
		return (void *) -1;
	}

	free(msg_buf);

	return (void *) var;
}

int ligar_backup(char *ip, char *port) {

	int sockfd = -1;

	// Armazena info sobre servidor secundario
	struct hostent *hostptr;

	// Armazena IP do secundario
	struct in_addr *addrptr;

	//
	struct sockaddr_in server;

	if ((hostptr = gethostbyname(ip)) == NULL) {
		printf("gethostbyname error: %s\n", ip);
		return -1;
	}

	addrptr = (struct in_addr*) hostptr->h_addr;

	// Cria socket TCP
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Erro ao criar socket TCP");
		return -1;
	}

	// Preenche estrutura server para estabelecer conexão
	server.sin_family = AF_INET;
	server.sin_port = htons(atoi(port));
	server.sin_addr = *addrptr;

	//limpar o sin_zero
	memset(server.sin_zero, 0, sizeof(server.sin_zero));

	// Estabelece conexão com o servidor secundario
	if (connect(sockfd, (struct sockaddr *) &server, sizeof(server)) < 0) {
		perror("Erro ao conectar-se ao servidor secundario");
		return -1;
	}

	return sockfd;
}
