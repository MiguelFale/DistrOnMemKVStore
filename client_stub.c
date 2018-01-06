#include "network-client-private.h"   
#include "client_stub.h"
#include "client_stub-private.h"

struct rtable_t *rtable_bind(const char *address_port) {

	//criacao da estrutura usada para a ligacao
	struct rtable_t *myTable = (struct rtable_t *) malloc(
			sizeof(struct rtable_t));

	if (myTable == NULL) {
		printf("Erro ao criar estrutura myTable!\n");
		return NULL;
	}

	//ligar ao servidor
	myTable->abort = NO;
	myTable->myServer = network_connect(address_port);

	if (myTable->myServer == NULL) {
		free(myTable);
		return NULL;
	}

	//preencher com o par <hostname>:<port> associado
	myTable->myServer->address_port = address_port;

	return myTable;

}

int rtable_unbind(struct rtable_t *rtable) {

	if (rtable->myServer == NULL) {
		free(rtable);
		return -1;
	}

	//fechar ligacao e eliminar a estrutura myServer
	if (network_close(rtable->myServer) == -1) {
		free(rtable);
		return -1;
	}

	//eliminar a estrutura rtable
	free(rtable);
	return 0;

}

int rtable_put(struct rtable_t *rtable, char *key, struct data_t *data) {

	struct message_t *mymsg;
	mymsg = (struct message_t*) malloc(sizeof(struct message_t));
	mymsg->opcode = OC_PUT;
	mymsg->c_type = CT_ENTRY;
	mymsg->content.entry = entry_create(key, data);

	struct message_t *resposta = network_send_receive(rtable->myServer, mymsg);

	free_message(mymsg);

	//verifica se o servidor foi abaixo (feito para cada pedido)
	if (resposta == NULL) {
		rtable->abort = YES;
		return -1;
	}

	int res = resposta->opcode;

	free_message(resposta);

	if (res == OC_RT_ERROR)
		return -1; //put falhou (out of memory)
	else
		return 0; //put bem sucedido

}

int rtable_conditional_put(struct rtable_t *rtable, char *key,
		struct data_t *data) {

	struct message_t *mymsg;
	mymsg = (struct message_t*) malloc(sizeof(struct message_t));
	mymsg->opcode = OC_COND_PUT;
	mymsg->c_type = CT_ENTRY;
	mymsg->content.entry = entry_create(key, data);

	struct message_t *resposta = network_send_receive(rtable->myServer, mymsg);

	free_message(mymsg);

	if (resposta == NULL) {
		rtable->abort = YES;
		return -1;
	}

	int res = resposta->opcode;

	free_message(resposta);

	if (res == OC_RT_ERROR)
		return -1; //cput falhou (ja existe ou out of memory)
	else
		return 0; //cput bem sucedido

}

struct data_t *rtable_get(struct rtable_t *rtable, char *key) {

	struct message_t *mymsg;
	mymsg = (struct message_t*) malloc(sizeof(struct message_t));
	mymsg->opcode = OC_GET;
	mymsg->c_type = CT_KEY;

	mymsg->content.key = key;

	struct message_t *resposta = network_send_receive(rtable->myServer, mymsg);

	free_message(mymsg);

	if (resposta == NULL) {
		rtable->abort = YES;
		return NULL;
	}

	//dados devem ser duplicados para que se possa apagar a resposta
	struct data_t *myData = data_dup(resposta->content.value);

	free_message(resposta);

	if (myData->datasize != 0 && myData->data != NULL)
		return myData; //get bem sucedido; copia dos dados vai ser retornada

	else {
		data_destroy(myData);
		return NULL; //get falhou; chave nao encontrada
	}

}

int rtable_del(struct rtable_t *rtable, char *key) {

	struct message_t *mymsg;
	mymsg = (struct message_t*) malloc(sizeof(struct message_t));
	mymsg->opcode = OC_DEL;
	mymsg->c_type = CT_KEY;

	mymsg->content.key = key;

	struct message_t *resposta = network_send_receive(rtable->myServer, mymsg);

	free_message(mymsg);

	if (resposta == NULL) {
		rtable->abort = YES;
		return -1;
	}

	int res = resposta->opcode;

	free_message(resposta);

	if (res == OC_RT_ERROR)
		return -1; //del falhou; chave nao encontrada
	else
		return 0; //del bem sucedido; chave apagada

}

int rtable_size(struct rtable_t *rtable) {

	struct message_t *mymsg;
	mymsg = (struct message_t*) malloc(sizeof(struct message_t));
	mymsg->opcode = OC_SIZE;
	mymsg->c_type = CT_RESULT;
	mymsg->content.result = 1;

	struct message_t *resposta = network_send_receive(rtable->myServer, mymsg);

	free_message(mymsg);

	if (resposta == NULL) {
		rtable->abort = YES;
		return -1;
	}

	int res = resposta->content.result;

	free_message(resposta);

	return res; //res eh o tamanho da tabela hash do servidor (total de nos)

}

char **rtable_get_keys(struct rtable_t *rtable) {

	struct message_t *mymsg;
	mymsg = (struct message_t*) malloc(sizeof(struct message_t));
	mymsg->opcode = OC_GET_KEYS;
	mymsg->c_type = CT_RESULT;
	mymsg->content.result = 1;

	struct message_t *resposta = network_send_receive(rtable->myServer, mymsg);

	free_message(mymsg);

	if (resposta == NULL) {
		rtable->abort = YES;
		return NULL;
	}

	char **myKeys;

	if (resposta->opcode == OC_RT_ERROR) {
		free_message(resposta);
		return NULL; //getkeys falhou; nao existem chaves
	}

	else {
		int i;

		// contar quantas keys existem
		for (i = 0; resposta->content.keys[i] != NULL; i++)
			;

		myKeys = (char**) malloc((i + 1) * sizeof(char*));

		// copiar
		for (i = 0; resposta->content.keys[i] != NULL; i++) {
			myKeys[i] = strdup(resposta->content.keys[i]);
		}

		myKeys[i] = NULL;

		free_message(resposta);
		return myKeys; //getkeys bem sucedido; copia das chaves vai ser retornada
	}
}

void rtable_free_keys(char **keys) {

	return list_free_keys(keys);
}

