#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include "message-private.h"
#include <stdio.h>

int message_to_buffer(struct message_t *msg, char **msg_buf) {

	int size = -1, offset = 0;
	short auxs;
	long long auxl;
	int auxi;
	int str;

	//esta funcao vai executar de acordo com o c_type da mensagem
	switch (msg->c_type) {

	case CT_ENTRY:
		//somar tamanhos para:
		size = sizeof(short) + sizeof(short) + //opcode e c_type
				sizeof(long long) + sizeof(int) + //timestamp e tamanho da key
				strlen(msg->content.entry->key) + //key
				sizeof(int) + //tamanho do data
				msg->content.entry->value->datasize; //data

		//reservar memoria para o buffer
		*msg_buf = (char *) malloc(size);

		//memcpy coloca os dados no buffer
		//opcode
		auxs = htons(msg->opcode);
		memcpy(*msg_buf + offset, &auxs, sizeof(short));
		offset += sizeof(short);

		//c_type
		auxs = htons(msg->c_type);
		memcpy(*msg_buf + offset, &auxs, sizeof(short));
		offset += sizeof(short);

		//timestamp
		auxl = swap_bytes_64(msg->content.entry->timestamp);
		memcpy(*msg_buf + offset, &auxl, sizeof(long long));
		offset += sizeof(long long);

		//tamanho da key
		auxi = htonl(strlen(msg->content.entry->key));
		memcpy(*msg_buf + offset, &auxi, sizeof(int));
		offset += sizeof(int);

		//key
		memcpy(*msg_buf + offset, msg->content.entry->key,
				strlen(msg->content.entry->key));
		offset += strlen(msg->content.entry->key);

		//tamanho do data
		auxi = htonl(msg->content.entry->value->datasize);
		memcpy(*msg_buf + offset, &auxi, sizeof(int));
		offset += sizeof(int);

		//data
		memcpy(*msg_buf + offset, msg->content.entry->value->data,
				msg->content.entry->value->datasize);
		break;

	case CT_KEY:
		//somar tamanhos para:
		size = sizeof(short) + sizeof(short) + //opcode e c_type
				sizeof(int) + strlen(msg->content.key); //tamanho da key e key

		//reservar memoria para o buffer
		*msg_buf = (char *) malloc(size);

		//memcpy coloca os dados no buffer
		auxs = htons(msg->opcode);
		memcpy(*msg_buf + offset, &auxs, sizeof(short));
		offset += sizeof(short);

		auxs = htons(msg->c_type);
		memcpy(*msg_buf + offset, &auxs, sizeof(short));
		offset += sizeof(short);

		//tamanho da key
		auxi = htonl(strlen(msg->content.key));
		memcpy(*msg_buf + offset, &auxi, sizeof(int));
		offset += sizeof(int);

		//key
		memcpy(*msg_buf + offset, msg->content.key, strlen(msg->content.key));
		break;

	case CT_KEYS:
		//somar tamanhos para:
		size = sizeof(short) + sizeof(short) + //opcode e c_type
				sizeof(int); //numero de keys

		int i = 0, n = 0, j, soma = 0;

		for (; msg->content.keys[i] != NULL; i++) {
			n++; //para se saber o numero de keys
			soma += strlen(msg->content.keys[i]); //soma dos tamanhos das keys
		}

		size += soma * sizeof(char) + n * sizeof(int);

		//reservar memoria para o buffer
		*msg_buf = (char *) malloc(size);

		//memcpy coloca os dados no buffer
		auxs = htons(msg->opcode);
		memcpy(*msg_buf + offset, &auxs, sizeof(short));
		offset += sizeof(short);

		auxs = htons(msg->c_type);
		memcpy(*msg_buf + offset, &auxs, sizeof(short));
		offset += sizeof(short);

		//numero de keys
		auxi = htonl(n);
		memcpy(*msg_buf + offset, &auxi, sizeof(int));
		offset += sizeof(int);

		//pares (tamanho da key, key)
		for (j = 0; j < n; j++) {
			str = strlen(msg->content.keys[j]);
			auxi = htonl(str);
			memcpy(*msg_buf + offset, &auxi, sizeof(int));
			offset += sizeof(int);

			memcpy(*msg_buf + offset, msg->content.keys[j], str);
			offset += str;
		}
		break;

	case CT_VALUE:
		//somar tamanhos para:
		size = sizeof(short) + sizeof(short) + //opcode e c_type
				sizeof(int) + msg->content.value->datasize; //datasize e data

		//reservar memoria para o buffer
		*msg_buf = (char *) malloc(size);

		auxs = htons(msg->opcode);
		memcpy(*msg_buf + offset, &auxs, sizeof(short));
		offset += sizeof(short);

		auxs = htons(msg->c_type);
		memcpy(*msg_buf + offset, &auxs, sizeof(short));
		offset += sizeof(short);

		//datasize
		auxi = htonl(msg->content.value->datasize);
		memcpy(*msg_buf + offset, &auxi, sizeof(int));
		offset += sizeof(int);

		//data
		memcpy(*msg_buf + offset, msg->content.value->data,
				msg->content.value->datasize);

		break;

	case CT_RESULT:
		size = sizeof(short) + sizeof(short) + //opcode e c_type
				sizeof(int); //resultado

		//reservar memoria para o buffer
		*msg_buf = (char *) malloc(size);

		auxs = htons(msg->opcode);
		memcpy(*msg_buf + offset, &auxs, sizeof(short));
		offset += sizeof(short);

		auxs = htons(msg->c_type);
		memcpy(*msg_buf + offset, &auxs, sizeof(short));
		offset += sizeof(short);

		//resultado
		auxi = htonl(msg->content.result);
		memcpy(*msg_buf + offset, &auxi, sizeof(int));
		break;

	}

	return size;
}

struct message_t *buffer_to_message(char *msg_buf, int msg_size) {

	int offset = 0;
	int n, j;

	struct message_t *msg = (struct message_t *) malloc(
			sizeof(struct message_t));
	int size_string;
	int sizeCT;
	long long templ;
	char *temps;
	void *tempv;

	//passar primeiro com o memcpy e depois converter
	memcpy(&msg->opcode, msg_buf + offset, sizeof(short)); //OCPODE
	msg->opcode = ntohs(msg->opcode);
	offset += sizeof(short);

	memcpy(&msg->c_type, msg_buf + offset, sizeof(short)); //C_TYPE
	msg->c_type = ntohs(msg->c_type);
	offset += sizeof(short);

	switch (msg->c_type) {

	case CT_ENTRY:
		memcpy(&templ, msg_buf + offset, sizeof(long long)); //TIMESTAMP
		templ = swap_bytes_64(templ);
		offset += sizeof(long long);

		memcpy(&size_string, msg_buf + offset, sizeof(int)); //KEYSIZE
		size_string = ntohl(size_string);
		offset += sizeof(int);

		temps = (char *) malloc(sizeof(char) * (size_string + 1));
		memcpy(temps, msg_buf + offset, size_string); //KEY
		temps[size_string] = '\0';
		offset += size_string;

		memcpy(&(sizeCT), msg_buf + offset, sizeof(int)); //DATASIZE
		sizeCT = ntohl(sizeCT);
		offset += sizeof(int);

		tempv = malloc(sizeCT); //DATA

		msg->content.entry = entry_create(temps,
				data_create2(sizeCT, memcpy(tempv, msg_buf + offset, sizeCT)));

		msg->content.entry->timestamp = templ;
		msg->content.entry->key = temps;
		offset += sizeCT;

		break;

	case CT_KEY:
		memcpy(&size_string, msg_buf + offset, sizeof(int)); //KEYSIZE
		size_string = ntohl(size_string);
		offset += sizeof(int);

		msg->content.key = (char *) malloc(sizeof(char) * (size_string + 1));
		memcpy(msg->content.key, msg_buf + offset, size_string); //KEY
		msg->content.key[size_string] = '\0';
		offset += size_string;
		break;

	case CT_KEYS:
		memcpy(&n, msg_buf + offset, sizeof(int)); //NKEYS
		n = ntohl(n);
		offset += sizeof(int);

		msg->content.keys = (char **) malloc((n + 1) * sizeof(char *));

		for (j = 0; j < n; j++) {
			memcpy(&size_string, msg_buf + offset, sizeof(int)); //KEYSIZE
			size_string = ntohl(size_string);
			offset += sizeof(int);

			msg->content.keys[j] = (char *) malloc(
					sizeof(char) * (size_string + 1));

			memcpy(msg->content.keys[j], msg_buf + offset, size_string);
			msg->content.keys[j][size_string] = '\0'; //KEY
			offset += size_string;

		}
		msg->content.keys[n] = NULL;
		break;

	case CT_VALUE:
		memcpy(&(sizeCT), msg_buf + offset, sizeof(int)); //DATASIZE
		sizeCT = ntohl(sizeCT);
		offset += sizeof(int);

		msg->content.value = data_create(sizeCT);
		memcpy(msg->content.value->data, msg_buf + offset,
				msg->content.value->datasize); //DATA
		offset += sizeCT;
		break;

	case CT_RESULT:
		memcpy(&msg->content.result, msg_buf + offset, sizeof(int)); //RESULT
		msg->content.result = ntohl(msg->content.result);
		offset += sizeof(int);
		break;

	default:
		//nao fazer a mensagem em caso de buffer invalido
		free_message(msg);
		msg = NULL;
		break;
	}

	if (offset != msg_size)
		return NULL;

	return msg;
}

void free_message(struct message_t *message) {

	//liberta o respetivo conteudo
	switch (message->c_type) {

	case CT_ENTRY:
		entry_destroy(message->content.entry);
		break;

	case CT_KEY:
		free(message->content.key);
		break;

	case CT_KEYS:
		//esta funcao tem um mecanismo identico ao pretendido
		list_free_keys(message->content.keys);
		break;

	case CT_VALUE:
		data_destroy(message->content.value);
		break;
	}

	//por fim apagar a estrutura
	free(message);
}
