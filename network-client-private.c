#include "network-client-private.h"

int reconnect(struct server_t *Server, int target) {

	int sockfd;
	char *copy =
			(target == 1) ?
					strdup(Server->address_port) :
					strdup(Server->address_port_sec);
	char *host = strtok(copy, ":");

	if (host == NULL) {
		printf("Formato incorreto!\n");
		free(copy);
		return -1;
	}

	char *port = strtok(NULL, " \n\t");
	if (port == NULL) {
		printf("Nao especificou o porto.\n");
		free(copy);
		return -1;
	}

	//criar socket TCP e alterar a estrutura Server
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Erro ao criar socket TCP");
		free(copy);
		return -1;
	}

	Server->sock_serv.sin_family = AF_INET;
	Server->sock_serv.sin_port = htons(atoi(port));

	//verifica e converte uma string para um endereco IP em formato rede
	if (inet_pton(AF_INET, host, &Server->sock_serv.sin_addr) < 1) {
		printf("Erro ao converter IP");
		free(copy);
		close(sockfd);
		return -1;
	}

	memset(Server->sock_serv.sin_zero, 0, sizeof(Server->sock_serv.sin_zero));

	//estabelece conexao com o servidor definido em Server
	if (connect(sockfd, (struct sockaddr *) &Server->sock_serv,
			sizeof(Server->sock_serv)) < 0) {
		perror("Erro ao conectar-se ao servidor");
		free(copy);
		close(sockfd);
		return -1;
	}

	Server->socket = sockfd;
	free(copy);

	return 0;
}

int write_all(int sock, char *buf, int len) {
	int bufsize = len;

	while (len > 0) {
		int res = write(sock, buf, len);

		if (res < 0) {
			if (errno == EINTR)
				continue;
			perror("write failed:");
			return res;
		}

		buf += res;
		len -= res;
	}

	return bufsize;
}

int read_all(int sock, char *buf, int len) {

	int bufsize = len;

	while (len > 0) {
		int res = read(sock, buf, len);
		if (res < 0) {
			if (errno == EINTR)
				continue;
			perror("read failed:");
			return res;
		}

		buf += res;
		len -= res;
	}
	return bufsize;
}

int palavras(const char *str) {

	if (str == NULL)
		return -1;

	char* teste = (char*) malloc(sizeof(char) * (strlen(str) + 1));
	strcpy(teste, str);
	char* aux = strtok(teste, " \n\t");
	int contador = 0;

	while (aux != NULL) {
		contador++;
		aux = strtok(NULL, " \n\t");
	}

	free(teste);

	return contador;
}

char **parsing_cput(char *aux, int nPalavras) {

	int tam = 0, i = 1;

	char **myinput = (char**) malloc(2 * sizeof(char*));
	aux = strtok(NULL, " \n\t");

	while (aux != NULL && i <= nPalavras) {

		//primeira palavra do <data>
		if (i == 1) {
			tam = sizeof(char) * (strlen(aux) + 2);
			myinput[0] = (char *) malloc(tam);
			strcpy(myinput[0], aux);
			strcat(myinput[0], " ");
		}

		//restantes palavras do <data>
		else if (i < nPalavras - 1) {
			tam += sizeof(char) * (strlen(aux) + 2);
			myinput[0] = realloc(myinput[0], tam);
			strcat(myinput[0], aux);
			strcat(myinput[0], " ");
		}

		//<key>
		else {
			myinput[1] = (char *) malloc((sizeof(char) * (strlen(aux) + 1)));
			strcpy(myinput[1], aux);
		}

		//avancar para o proximo token
		i++;
		aux = strtok(NULL, " \n\t");
	}

	//tirar o espaco da ultima palavra de <data>
	tam = strlen(myinput[0]);
	myinput[0][tam - 1] = '\0';

	return myinput;
}

int inputVerify(char *aux, int nPalavras) {

	//se o utilizador nao inseriu nada
	if (nPalavras == 0)
		return -1;

	/* ao inserir um comando existente, podem existir duas situacoes:
	 * 1) utilizacao incorreta do comando - retorna erro (-1)
	 * 2) utilizacao correta do comando - retorna o respetivo opcode
	 *
	 * a utilizacao de comandos inexistentes tambem retorna erro
	 */
	if (strcmp(aux, "put") == 0 || strcmp(aux, "cput") == 0) {

		if (nPalavras < 3) {
			printf("Use: %s <data> <key>\n", aux);
			printf("Exemplo: %s teenage mutant ninja turtles chave\n", aux);
			return -1;
		}

		else
			return (strcmp(aux, "put") == 0) ? OC_PUT : OC_COND_PUT;
	}

	else if (strcmp(aux, "get") == 0 || strcmp(aux, "del") == 0) {

		if (nPalavras != 2) {
			printf("Use: %s <key>\n", aux);
			printf("Exemplo: %s Klefki\n", aux);
			return -1;
		} else
			return (strcmp(aux, "get") == 0) ? OC_GET : OC_DEL;
	}

	else if (strcmp(aux, "size") == 0 || strcmp(aux, "getkeys") == 0) {

		if (nPalavras != 1) {
			printf("Use: %s\n", aux);
			return -1;
		}

		else
			return (strcmp(aux, "size") == 0) ? OC_SIZE : OC_GET_KEYS;
	}

	else if (strcmp(aux, "help") == 0) {
		printf("\nEsta disponivel a seguinte lista de comandos:\n\n");
		printf(
				"\t- put <data> <key>\n\t- cput <data> <key>\n\t- get <key>\n\t- del <key>\n\t- size\n\t- getkeys\n\t- quit\n\n");
		return -1;
	}

	//utilizador inseriu um comando inexistente
	printf("%s: Comando invalido. Para ajuda insira \"help\".\n", aux);
	return -1;
}
