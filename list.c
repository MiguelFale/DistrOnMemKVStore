#include <stdlib.h>
#include <string.h>
#include "list-private.h"

struct list_t *list_create() {

	//criacao de uma nova lista que no inicio esta vazia
	struct list_t *mylist = (struct list_t *) malloc(sizeof(struct list_t));

	if (mylist == NULL)
		return NULL;

	mylist->tamanho = 0;
	mylist->cabeca = NULL;
	mylist->cauda = NULL;

	return mylist;
}

int list_destroy(struct list_t *list) {

	if (list == NULL) {
		return -1;
	}

	struct node_t *temp = list->cabeca;

	//enquanto existirem nodes
	while (list->cabeca != NULL) {
		entry_destroy(temp->client);
		temp = temp->next; //avanca para o proximo node
		free(list->cabeca); //libertar cabeca
		list->cabeca = temp; //actualiza a cabeca
	}

	//destruir a propria lista
	free(list);
	return 0;
}

int list_add(struct list_t *list, struct entry_t *entry) {

	//cria um novo no para adicionar na lista
	struct node_t *novo = (struct node_t *) malloc(sizeof(struct node_t));

	/* TESTES
	 * 1 - node mal reservado?
	 * 2 - a lista dada nao existe?
	 * 3 - a entrada dada nao existe?
	 */
	if (novo == NULL || list == NULL || entry == NULL)
		return -1;

	//o novo node vai ter no campo client um apontador para a entry
	novo->client = entry;

	struct node_t *no = list->cabeca;
	struct node_t *ante = no;

	//se a lista estiver vazia
	if (list->tamanho == 0) {
		list->cabeca = novo;
		list->cauda = novo;
		novo->next = NULL;
		novo->prev = NULL;
		list->tamanho++;

		return 0;
	}
	//se o elemento a introduzir eh maior que a cabeca
	if (strcmp(entry->key, no->client->key) > 0) {
		list->cabeca = novo;
		novo->next = no;
		novo->prev = NULL;
		no->prev = novo;
		list->tamanho++;
		return 0;
	} else if (strcmp(entry->key, no->client->key) == 0)
		return -1; //erro - foi encontrado um node com key igual!

	//insercao apos a cabeca e antes da cauda
	no = no->next;

	while (no != NULL) {
		if (strcmp(entry->key, no->client->key) > 0) {
			ante->next = novo;
			no->prev = novo;
			novo->prev = ante;
			novo->next = no;
			list->tamanho++;
			return 0;
		} else if (strcmp(entry->key, no->client->key) == 0)
			return -1; //erro

		//avanca na lista
		ante = no;
		no = no->next;
	}

	//insercao na cauda - nao foi encontrado um node com key igual
	ante->next = novo;
	novo->prev = ante;
	novo->next = NULL;
	list->tamanho++;

	return 0;
}

int list_remove(struct list_t *list, char *key) {

	struct node_t *temp = list->cabeca;
	struct node_t *antes = temp;
	struct node_t *seguinte = temp->next;

	/* TESTES
	 * 1 - a lista dada nao existe?
	 * 2 - a key dada nao existe?
	 * 3 - a lista esta vazia?
	 */
	if (list == NULL || key == NULL || list->tamanho == 0)
		return -1;

	//se encontra o node a remover logo de inicio
	if (strcmp(key, temp->client->key) == 0) {

		entry_destroy(list->cabeca->client);
		temp = temp->next; //avanca para o proximo node
		free(list->cabeca); //libertar cabeca
		list->cabeca = temp; //actualizar a cabeca
		list->tamanho--;
		return 0;
	}

	//procurar no meio...
	while (seguinte != NULL) {

		//se encontrar o node
		if (strcmp(key, seguinte->client->key) == 0) {

			//cuidado; se remover o ultimo eh necessario actualizar o campo da cauda
			if (seguinte == list->cauda) {
				list->cauda = antes;
			}

			entry_destroy(seguinte->client);
			antes->next = seguinte->next;
			free(seguinte);
			list->tamanho--;
			return 0;
		}

		//avanca na lista
		antes = seguinte;
		seguinte = seguinte->next;

	}
	return -1; // como nao encontrou nada...
}

struct entry_t *list_get(struct list_t *list, char *key) {

	struct node_t *temp = list->cabeca;

	while (temp != NULL) {

		//se encontrar a chave pretendida
		if (strcmp(key, temp->client->key) == 0)
			return temp->client;

		//avanca na lista
		temp = temp->next;

	}

	return NULL; //chave nao encontrada apos percorrer toda a lista
}

int list_size(struct list_t *list) {

	return (list != NULL ? list->tamanho : -1);

}

char **list_get_keys(struct list_t *list) {

	char **keys;
	int i = 0;
	struct node_t *temp = list->cabeca;

	//reserva do espaco necessario
	//precisa +1 para o apontador NULL
	keys = (char **) malloc(sizeof(char *) * ((list->tamanho) + 1));

	//percorre lista
	while (temp != NULL) {

		//efectuar a copia
		keys[i] = (char *) malloc(
				sizeof(char) * (strlen(temp->client->key) + 1));
		strcpy(keys[i], temp->client->key);

		temp = temp->next;
		i++;
	}

	keys[i] = NULL;

	return keys;
}

void list_free_keys(char **keys) {

	int i = 0;

	//libertar as keys todas
	for (; keys[i] != NULL; i++) {
		free(keys[i]);
	}

	//e por fim o apontador
	free(keys);
}
