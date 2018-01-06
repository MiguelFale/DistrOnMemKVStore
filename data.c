#include <stdlib.h>
#include <string.h>
#include "data.h"

struct data_t *data_create(int size) {

	//reserva memoria para UM bloco de dados e verificacao do erro
	struct data_t *mydata = (struct data_t *) malloc(sizeof(struct data_t));
	if (mydata == NULL)
		return NULL;

	mydata->datasize = size;

	//reserva espaco para os dados arbitrarios
	mydata->data = (void *) malloc(size);

	return mydata;
}

struct data_t *data_create2(int size, void *data) {

	//reserva memoria para UM bloco de dados e verificacao do erro
	struct data_t *mystruct = (struct data_t *) malloc(sizeof(struct data_t));
	if (mystruct == NULL)
		return NULL;

	mystruct->datasize = size;

	//liga o campo data da nova estrutura a um espaco previamente reservado
	mystruct->data = data;

	return mystruct;
}

void data_destroy(struct data_t *data) {

	//libertar conteudo arbitrario e depois a propria estrutura
	free(data->data);
	free(data);
}

struct data_t *data_dup(struct data_t *data) {

	//criar novo bloco de dados de tamanho igual ao original
	struct data_t *copia = data_create(data->datasize);

	//copiar o conteudo do bloco original para o novo bloco
	memcpy(copia->data, data->data, data->datasize);

	return copia;
}
