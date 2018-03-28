#include "lectureClavier.h"
#include <sys/select.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <pthread.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_BUF 32

// Lectureclavier tourne dans un thread et permet de récupérer les caractères tapés au clavier
// arguments : permet de passer des arguments au thread
void* Lectureclavier(void* arguments){
	clavierArgsThread* args = arguments;
	int ret;
	int status;
	char buf[MAX_BUF];
	
	while(1){
		memset(buf,0,MAX_BUF);

		// On lit stdin et on écrit ce qui a été lu dans args->c. Read est bloquant s'il n'y a rien à lire
		ret = read(STDIN_FILENO,buf, MAX_BUF);
		if (ret<0) { perror("read"); }

		status = sendto(args->sockd, (void*)buf, MAX_BUF, 0, (struct sockaddr*) &(args->sockAddr), sizeof(args->sockAddr));
		if (status < 0){
			perror("Erreur sendto client\n");
			exit(0);
		}

	}
}