#include "table-private.h"
#include "network-client-private.h"

struct table_t *myTable;

int table_skel_init(int n_lists) {

	//criacao da tabela hash
	myTable = table_create(n_lists);

	if (myTable != NULL)
		return 0;
	else
		return -1;

}

int table_skel_destroy() {

	table_destroy(myTable);

	return 0;

}

int invoke(struct message_t *msg) {

	struct data_t *data;
	int res;
	char **keySet;

	switch (msg->opcode) {
	case OC_PUT:
	case OC_COND_PUT:
		//res == 0 OK
		//res == -1 ERRO

		if (msg->opcode == OC_PUT)
			res = table_put(myTable, msg->content.entry->key,
					msg->content.entry->value);
		else
			res = table_conditional_put(myTable, msg->content.entry->key,
					msg->content.entry->value);

		//limpar e preencher a mensagem
		entry_destroy(msg->content.entry);

		msg->opcode = (res == 0) ? msg->opcode : OC_RT_ERROR;
		msg->c_type = CT_RESULT;
		msg->content.result = res;
		break;

	case OC_GET:
		// data != NULL OK
		// data == NULL ERRO

		data = table_get(myTable, msg->content.key);

		//limpar e preencher a mensagem
		free(msg->content.key);

		msg->opcode = OC_GET;
		msg->c_type = CT_VALUE;
		msg->content.value = (data != NULL) ? data : data_create(0);
		break;

	case OC_DEL:
		//res == 0 OK
		//res == -1 ERRO

		res = table_del(myTable, msg->content.key);

		//limpar e preencher a mensagem
		free(msg->content.key);

		msg->opcode = (res == 0) ? OC_DEL : OC_RT_ERROR;
		msg->c_type = CT_RESULT;
		msg->content.result = res;
		break;

	case OC_SIZE:

		//preencher a mensagem
		msg->opcode = OC_SIZE;
		msg->c_type = CT_RESULT;
		msg->content.result = table_size(myTable);
		break;

	case OC_GET_KEYS:
		//keySet[0] != NULL OK
		//keySet[0] == NULL ERRO

		keySet = table_get_keys(myTable);

		//preencher a mensagem
		msg->c_type = keySet[0] != NULL ? CT_KEYS : CT_RESULT;
		msg->opcode = keySet[0] != NULL ? OC_GET_KEYS : OC_RT_ERROR;

		if (keySet[0] == NULL) {
			table_free_keys(keySet);
			msg->content.result = -1;
		} else
			msg->content.keys = keySet;

		break;

	default:
		return -1;
	}

	return 0;

}
