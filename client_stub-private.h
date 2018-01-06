#ifndef _CLIENT_STUB_PRIVATE_H 
#define _CLIENT_STUB_PRIVATE_H

#include "network-client-private.h"

#define YES 1 // deve-se abortar
#define NO 0 // OK
struct rtable_t {
	struct server_t *myServer;
	int abort; // YES ou NO
};

#endif
