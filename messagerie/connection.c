#include "connection.h"
#include "lectureClavier.h"
#include <pthread.h>

int main(int argc, char* argv[]){
	char c = getchar();
	if(c == 's'){
		server(atoi(argv[1]));
	}
	else if(c == 'c'){
		client(atoi(argv[1]), argv[2]);
	}
	return 1;

}


struct connected{
	int nb;
	struct sockaddr_in tab_sock[3]; 
}; 


int sockaddr_cmp(struct sockaddr_in x, struct sockaddr_in y)
{
	int rep = 0;
	
	if(x.sin_addr.s_addr == y.sin_addr.s_addr){
		printf("ok\n");
		rep = 1;
	}
	return rep;
}

int is_notin(struct connected myconnections, struct sockaddr_in sock){
	int lim = myconnections.nb;
	int rep = 1;
	for(int i = 0; i < lim; i++){
		if(sockaddr_cmp(myconnections.tab_sock[i], sock)){
			rep = 0;
		}
	}
	return rep;
}






int server(int port) {
	printf ("port=%d\n", port);
	int sockd;
	struct sockaddr_in my_name, cli_addr;
	int status;
	int addrlen;
	struct connected allConnections;
	
	/*Init tab connections*/
	allConnections.nb = 0;
	
	/* Create a UDP socket */
	sockd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockd == -1) {
		perror("Socket creation error");
		exit(1);
	}

	/* Configure server address */
	my_name.sin_family = AF_INET;
	my_name.sin_addr.s_addr = INADDR_ANY;
	my_name.sin_port = htons(port);

	status = bind(sockd, (struct sockaddr*) &my_name, sizeof(my_name));
	if (status < 0){
		perror("Erreur bind\n");
		exit(0);
	}

	addrlen = sizeof(cli_addr);
	
	while (1) {
		status = recvfrom(sockd, buf, MAX_BUF, 0, (struct sockaddr*) &cli_addr, &addrlen);
		if (status < 0){
			perror("Erreur recvfrom\n");
			exit(0);
		}
		if (is_notin(allConnections, cli_addr)){
			allConnections.nb += 1;
			allConnections.tab_sock[allConnections.nb] = cli_addr;
		}
		for(int i = 0; i < allConnections.nb; i++){
			sendto(sockd, buf, MAX_BUF, 0, (struct sockaddr*)&(allConnections.tab_sock[i]), sizeof(allConnections.tab_sock[i]));
		}
		if (status < 0){
			perror("Erreur sendto\n");
			exit(0);
		}
		printf("server : %s\n", buf);
	}
	close(sockd);
}

int client(int port, char addr[]) {	 
	int sockd;
	struct sockaddr_in my_addr, srv_addr;
	char buf[MAX_BUF];
	//int count;
	int addrlen;
	int status;
	strcpy(buf,"Hello world\n");

	// Create a UDP socket
	sockd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockd == -1) {
	perror("Socket creation error");
	exit(1);
	}

	// Configure client address
	my_addr.sin_family = AF_INET;
	my_addr.sin_addr.s_addr = INADDR_ANY;
	my_addr.sin_port = 0;

	bind(sockd, (struct sockaddr*)&my_addr, sizeof(my_addr));
	if (status < 0){
		perror("Erreur bind client\n");
		exit(0);
	}

	// Set server address 
	srv_addr.sin_family = AF_INET;
	inet_aton(addr, &srv_addr.sin_addr);
	srv_addr.sin_port = htons(port);
	 



	char c;
	int ret;
	clavierArgsThread args;

	// Attribution des arguments du thread

	args.sockd = sockd;
	args.sockAddr = srv_addr;
	// Création d'un thread pour la lecture du clavier
	pthread_t threadID;
	// On crée un thread dans lequel la fonction Lectureclavier sera exécutée. On passe également les arguments contenus dans args
	ret = pthread_create(&threadID, NULL, (void*)Lectureclavier, (void*)&args);

	addrlen = sizeof(srv_addr);
	
	while(1) {  		
		status = recvfrom(sockd, buf, MAX_BUF, 0, (struct sockaddr*) &srv_addr, &addrlen);//&addrlen);
		if (status < 0){
			perror("Erreur recvfrom client\n");
			exit(0);
			printf("client : %s\n", buf);
		}
	}
	
	  	
	close(sockd);
}
