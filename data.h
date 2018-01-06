#ifndef _DATA_H
#define _DATA_H

/* Estrutura que contem os dados, isto e, o valor do par 
 * (chave, valor), juntamente com o seu tamanho.
 */
struct data_t {
	int datasize; /* Tamanho do bloco de dados data */
	void *data; /* Conteudo arbitrario */
};

/* Funcao que cria um novo bloco de dados (isto e, que inicializa
 * a estrutura e aloca a memoria necessaria).
 */
struct data_t *data_create(int size);

/* Funcao identica a anterior, mas com uma assinatura diferente.
 */
struct data_t *data_create2(int size, void *data);

/* Funcao que destroi um bloco de dados e liberta toda a memoria.
 */
void data_destroy(struct data_t *data);

/* Funcao que duplica um bloco de dados. Quando se criam duplicados
 * e necessario efetuar uma COPIA dos dados (e nao somente alocar a
 * memoria necessaria).
 */
struct data_t *data_dup(struct data_t *data);
#endif
