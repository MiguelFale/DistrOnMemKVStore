#include "network-client-private.h"
#include "client_stub.h"
#include "client_stub-private.h"

#define MAXSIZE 1024

int main(int argc, char *argv[]) {

	if (argc < 2 && argc > 3) {
		printf("Faltam argumentos!\n");
		return 0;
	}

	//ignorar sinais do tipo SIGPIPE
	signal(SIGPIPE, SIG_IGN);

	struct data_t *blocoDados;
	char mystring[MAXSIZE];
	char **myinput, **resKeys;
	char *aux, *copia;
	int i, tam, nPalavras, retorno, guarda = 1, retry;
	struct rtable_t *remTable = NULL;

	printf("\n\t\tBEM-VINDO AO PROJETO 5 DE APLICACOES DISTRIBUIDAS!\n\n"
			"Em caso de duvida, insira o comando \"help\".\n\n");
	printf("prompt$ ");
	fgets(mystring, MAXSIZE, stdin);

	//CICLO PRINCIPAL: parsing, envio e rececao de mensagens
	while (strncmp(mystring, "quit", 4)) {

		//obter o numero de palavras do comando e a primeira palavra do mesmo
		nPalavras = palavras(mystring);

		copia = strdup(mystring); //usada em inputVerify
		aux = strtok(copia, " \n\t");

		/* certos inputs provocam a repeticao do ciclo principal
		 *
		 * CASOS:
		 * 1) Utilizador nao introduz nada.
		 * 2) Comando existente usado de forma incorreta.
		 * 3) Comando inexistente.
		 * 4) Comando help.
		 */
		if ((i = inputVerify(aux, nPalavras)) < 0) {
			free(copia);
			printf("prompt$ ");
			fgets(mystring, MAXSIZE, stdin);
			continue;
		}

		free(copia);

		//ligar pela primeira vez ao servidor
		if (guarda) {

			retry = 0;

			while (1) {

				remTable = rtable_bind(argv[1]);

				if (remTable != NULL) {
					remTable->myServer->primario = 1;
					remTable->myServer->address_port_sec = argv[2];
					guarda = 0;
					break;
				} else {
					printf(
							"O primario falhou; vai ligar ao secundário em 5 segundos-> %s\n",
							argv[2]);
					sleep(5);
					remTable = rtable_bind(argv[2]);
					retry++;

					if (remTable == NULL) {

						if (retry == 3) {
							printf("Abortar . . .\n");
							return 0;
						}

						printf(
								"O secundario falhou; vai ligar ao primario em 5 segundos-> %s\n",
								argv[1]);
						sleep(5);
						retry++;
						continue;
					}

					remTable->myServer->primario = 2;
					guarda = 0;
					break;
				}
			}
		}

		aux = strtok(mystring, " \n\t");

		switch (i) {

		case OC_PUT:
		case OC_COND_PUT:
			//retorno == 0 OK
			//retorno == -1 ERRO

			/**
			 * myinput[0] = < data >
			 * myinput[1] = < key >
			 */
			myinput = parsing_cput(aux, nPalavras);

			tam = strlen(myinput[0]) + 1; //tamanho de < data >
			blocoDados = data_create2(tam, myinput[0]);

			//chamar a funcao adequada
			retorno =
					(i == OC_PUT) ?
							rtable_put(remTable, myinput[1], blocoDados) :
							rtable_conditional_put(remTable, myinput[1],
									blocoDados);

			//O campo abort em remTable sinaliza se o servidor crashou
			//Se tal for o caso, deve-se libertar toda a memoria reservada
			if (remTable->abort == YES) {
				free(myinput);
				free(remTable->myServer);
				free(remTable);
				printf("Abortar . . .\n");
				return 0;
			}

			printf("Operacao %s %s\n", (i == OC_PUT) ? "put" : "cput",
					(retorno == 0) ? "bem sucedida!" : "falhou!");

			free(myinput); //libertar memoria

			break;

		case OC_GET:
			//blocoDados != NULL OK
			//blocoDados == NULL ERRO

			aux = strtok(NULL, " \n\t");
			blocoDados = rtable_get(remTable, strdup(aux));

			if (remTable->abort == YES) {
				free(remTable->myServer);
				free(remTable);
				printf("Abortar . . .\n");
				return 0;
			}

			printf("Operacao get %s",
					(blocoDados != NULL) ? "bem sucedida! " : "falhou!\n");

			if (blocoDados != NULL) {
				printf("Os dados de \"%s\" sao:\n - \"%s\"\n", aux,
						(char *) blocoDados->data);
				data_destroy(blocoDados); //libertar memoria
			}
			break;

		case OC_DEL:
			//retorno == 0 OK
			//retorno == -1 ERRO

			aux = strtok(NULL, " \n\t");
			retorno = rtable_del(remTable, strdup(aux));

			if (remTable->abort == YES) {
				free(remTable->myServer);
				free(remTable);
				printf("Abortar . . .\n");
				return 0;
			}

			printf("Operacao del %s\n",
					(retorno != -1) ? "bem sucedida!" : "falhou!");

			break;

		case OC_SIZE:

			retorno = rtable_size(remTable);

			if (remTable->abort == YES) {
				free(remTable->myServer);
				free(remTable);
				printf("Abortar . . .\n");
				return 0;
			}

			printf("Operacao size bem sucedida! Tamanho = %d.\n", retorno);

			break;

		case OC_GET_KEYS:
			//resKeys != NULL OK
			//resKeys == NULL ERRO

			resKeys = rtable_get_keys(remTable);

			if (remTable->abort == YES) {
				free(remTable->myServer);
				free(remTable);
				printf("Abortar . . .\n");
				return 0;
			}

			if (resKeys != NULL) {
				printf("Operacao getkeys foi bem sucedida! Keys obtidas:\n");

				for (i = 0; resKeys[i] != NULL; i++)
					printf(" - \"%s\"\n", resKeys[i]);

				rtable_free_keys(resKeys); //libertar memoria
			}

			else
				printf("Não existem chaves!!!\n");

			break;
		}

		//proximo comando......
		printf("prompt$ ");
		fgets(mystring, MAXSIZE, stdin);
	}

	if (remTable != NULL) {
		if (rtable_unbind(remTable) == -1)
			printf("Erro inesperado ao fazer unbind...\n");
	}

	printf("Ligacao fechada!\n");
	return 0;
}

