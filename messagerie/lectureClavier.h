// Structure permettant de faire des transferts de variables entre threads

#ifndef __LECTURE_CLAVIER__
#define __LECTURE_CLAVIER__
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <semaphore.h>

typedef struct {
	char c;
	sem_t* bout_vide;
	sem_t* bout_plein;
	char* bout;
	int sockd;
	struct sockaddr_in sockAddr;
} clavierArgsThread;


void* Lectureclavier(void* arguments);

#endif