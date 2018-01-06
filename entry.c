#include <stdlib.h>
#include <string.h>
#include "entry.h"

struct entry_t *entry_create(char *key, struct data_t *data) {

	//cria uma nova entrada e depois atribui a chave e o valor
	struct entry_t *myentry = (struct entry_t *) malloc(sizeof(struct entry_t));
	if (myentry == NULL)
		return NULL; //verifica erro

	myentry->key = key;
	myentry->timestamp = 0;
	myentry->value = data;

	return myentry;
}

void entry_destroy(struct entry_t *entry) {

	//destroi a chave, o valor e depois a propria entrada
	free(entry->key);
	data_destroy(entry->value);
	free(entry);
}

struct entry_t *entry_dup(struct entry_t *entry) {

	//cria uma nova entrada e reserva espaco para uma chave
	struct entry_t *copia = (struct entry_t *) malloc(sizeof(struct entry_t));
	if (copia == NULL)
		return NULL; //erro

	copia->timestamp = entry->timestamp;

	copia->key = (char *) malloc((strlen(entry->key) + 1) * sizeof(char));
	if (copia->key == NULL)
		return NULL; //erro

	//copia chave da entrada original para a da nova entrada
	strcpy(copia->key, entry->key);

	//duplica o bloco de dados da original
	copia->value = data_dup(entry->value);

	return copia;
}
