#ifndef _MESSAGE_H
#define _MESSAGE_H

#include "data.h"
#include "entry.h"

/* Define os possiveis opcodes da mensagem */
#define OC_PUT		10
#define OC_COND_PUT	20
#define OC_GET   	30
#define OC_DEL		40
#define OC_SIZE		50
#define OC_GET_KEYS	60
/* Define codigos para os possiveis conteudos da mensagem */
#define CT_ENTRY  100
#define CT_KEY    200
#define CT_KEYS   300
#define CT_VALUE  400
#define CT_RESULT 500

/* Estrutura que representa uma mensagem generica a ser transmitida.
 * Esta mensagem pode ter varios tipos de conteudos.
 */
struct message_t {
	short opcode; /* codigo da operacao na mensagem */
	short c_type; /* tipo do content da mensagem */
	union content_u {
		struct entry_t *entry;
		char *key;
		char **keys;
		struct data_t *value;
		int result;
	} content; /* conteudo da mensagem */
};

/* Transforma uma message_t num char*, retornando o tamanho do
 * buffer alocado para a mensagem como um array de bytes, ou -1
 * em caso de erro.
 *
 * A mensagem serializada deve ter o seguinte formato:
 *
 * OPCODE	C_TYPE
 * [2 bytes]	[2 bytes]
 *  
 * a partir dai, o formato difere para cada c_type
 *
 * ct_type	dados
 * ENTRY	TIMESTAMP	KEYSIZE 	KEY 		DATASIZE	DATA
 *		[8 bytes]	[4 bytes]	[KS bytes]	[4 bytes]	[DS bytes]
 * KEY		KEYSIZE 	KEY 		
 *		[4 bytes]	[KS bytes]
 * KEYS		NKEYS		KEYSIZE 	KEY 		...
 *		[4 bytes]	[4 bytes]	[KS bytes] ...
 * VALUE	DATASIZE	DATA
 *		[4 bytes]	[DS bytes]
 * RESULT	RESULT
 *		[4 bytes]
 *
 * Note que o `\0� no fim da string e o NULL no fim do array de
 * chaves nao sao enviados nas mensagens.
 */
int message_to_buffer(struct message_t *msg, char **msg_buf);

/* Transforma uma mensagem em buffer para uma struct message_t*
 */
struct message_t *buffer_to_message(char *msg_buf, int msg_size);

/* Liberta a memoria alocada na fun��o buffer_to_message
 */
void free_message(struct message_t *message);
#endif
