#include "table-private.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

struct table_t *table_create(int n) {

	int i = 0;

	//cria uma nova tabela e verifica o erro
	struct table_t *nova = (struct table_t *) malloc(sizeof(struct table_t));
	if (nova == NULL)
		return NULL;

	nova->linhasTotal = n;

	//reserva para um bucket de n slots e verifica o erro
	nova->bucket = (struct list_t **) malloc(sizeof(struct list_t *) * n);
	if (nova->bucket == NULL) {
		table_destroy(nova);
		return NULL;
	}

	//cada slot aponta para uma lista; cada lista esta inicialmente vazia
	for (; i < n; i++) {
		nova->bucket[i] = list_create();
	}

	return nova;
}

void table_destroy(struct table_t * table) {

	int tam = table->linhasTotal;
	int i = 0;

	if (table->bucket != NULL) {
		//percorrer a tabela
		while (i < tam) {
			if (list_destroy(table->bucket[i]) == -1) {
				return; //erro ao destruir lista
			}
			i++;
		}
	}

	//libertar o bucket e por fim a propria estrutura
	free(table->bucket);
	free(table);
}

int table_put(struct table_t *table, char *key, struct data_t *data) {

	//obtem slot correta e cria uma entrada
	int slot = hash(key, table->linhasTotal);
	struct list_t *lpointer = table->bucket[slot];

	//copia da key e dos dados e criacao de uma nova entrada
	char *copiaKey;
	struct data_t *copiaData;
	if ((copiaKey = strdup(key)) == NULL)
		return -1; //erro
	if ((copiaData = data_dup(data)) == NULL) {
		free(copiaKey);
		return -1; //erro
	}

	struct entry_t *entrada = entry_create(copiaKey, copiaData);
	if (entrada == NULL) {
		free(copiaKey);
		data_destroy(copiaData);
		return -1; //erro
	}

	//se a key ja existir, remover o repetido
	if (list_get(lpointer, key) != NULL) {
		list_remove(lpointer, key);
	}

	//adiciona o elemento ah lista
	//retorna um inteiro representante do sucesso
	return list_add(lpointer, entrada);
}

int table_conditional_put(struct table_t *table, char *key, struct data_t *data) {

	//obtem slot correta e cria uma entrada
	int slot = hash(key, table->linhasTotal);
	struct list_t *lpointer = table->bucket[slot];

	//copia da key e dos dados e criacao de uma nova entrada
	char *copiaKey;
	struct data_t *copiaData;
	if ((copiaKey = strdup(key)) == NULL)
		return -1; //erro
	if ((copiaData = data_dup(data)) == NULL) {
		free(copiaKey);
		return -1; //erro
	}

	struct entry_t *entrada = entry_create(copiaKey, copiaData);
	if (entrada == NULL) {
		free(copiaKey);
		data_destroy(copiaData);
		return -1; //erro
	}

	//se a key ja existir, da erro. E ja nao necessita da entrada.
	if (list_get(lpointer, key) != NULL) {
		entry_destroy(entrada);
		return -1;
	}

	//adiciona o elemento ah lista
	//retorna um inteiro representante do sucesso
	return list_add(lpointer, entrada);
}

struct data_t *table_get(struct table_t *table, char *key) {

	//obtem slot correta e procura o elemento na respetiva lista
	int slot = hash(key, table->linhasTotal);
	struct entry_t *aux = list_get(table->bucket[slot], key);

	/* TESTES
	 * 1 - a tabela nao existe?
	 * 2 - a lista nao existe?
	 * 3 - elemento nao encontrado?
	 */
	if (table == NULL || table->bucket[slot] == NULL || aux == NULL) {
		return NULL;
	}

	//retorna um novo bloco de dados
	return data_dup(aux->value);
}

int table_del(struct table_t *table, char *key) {

	//obtem slot correta
	int slot = hash(key, table->linhasTotal);
	struct list_t *ptr = table->bucket[slot];

	//se o elemento existe, remove-lo
	if (list_get(ptr, key) != NULL) {
		list_remove(ptr, key);
		return 0;
	}

	//se o elemento nao existe, da erro
	return -1;
}

int table_size(struct table_t *table) {

	int i = 0;
	int soma = 0;

	//soma de TODOS os elementos (de todas as listas)
	while (i < table->linhasTotal) {
		soma += table->bucket[i]->tamanho;
		i++;
	}

	return soma;
}

char **table_get_keys(struct table_t *table) {

	char **keys;
	int i, j = 0;
	int tableSize = table_size(table);
	struct list_t *temp;
	struct node_t *nodet;

	//reservar espaco; cada posicao de keys vai ser uma string
	//necessario +1 para o ponteiro NULL
	keys = (char **) malloc((tableSize + 1) * sizeof(char*));

	//percorre a tabela
	for (i = 0; i < table->linhasTotal; i++) {

		temp = table->bucket[i];
		nodet = temp->cabeca;

		//percorre a lista, copiando as chaves
		while (nodet != NULL) {
			keys[j] = (char *) malloc(
					sizeof(char) * (strlen(nodet->client->key) + 1));
			strcpy(keys[j], nodet->client->key);
			nodet = nodet->next;
			j++;
		}

	}

	keys[tableSize] = NULL;

	return keys;
}

void table_free_keys(char **keys) {

	//esta funcao tem um mecanismo identico ao pretendido
	list_free_keys(keys);
}

