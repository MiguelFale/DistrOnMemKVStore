#include <string.h>
#include "table-private.h"

int hash(char* string, int n) {

	int tamanho = strlen(string);
	int soma = 0;
	int i = 0;

	if (tamanho <= 5)

		//soma eh valor ASCII de todos os caracteres da chave
		for (; string[i] != '\0'; i++)
			soma += string[i];

	else {

		//soma eh valor ASCII dos primeiros 3 e ultimos 2 caracteres
		for (; i <= 2; i++)
			soma += string[i];

		for (i = tamanho - 2; string[i] != '\0'; i++)
			soma += string[i];
	}

	return soma % n;
}
