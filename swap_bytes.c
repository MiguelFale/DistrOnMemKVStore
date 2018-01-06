#include "message-private.h"

long long swap_bytes_64(long long number) {
	long long new_number;

	new_number = ((number & 0x00000000000000FF) << 56
			| (number & 0x000000000000FF00) << 40
			| (number & 0x0000000000FF0000) << 24
			| (number & 0x00000000FF000000) << 8
			| (number & 0x000000FF00000000) >> 8
			| (number & 0x0000FF0000000000) >> 24
			| (number & 0x00FF000000000000) >> 40
			| (number & 0xFF00000000000000) >> 56);

	return new_number;
}
