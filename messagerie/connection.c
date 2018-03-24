#include "connection.h"

int main(int argc, char* argv[]){
	char c = getchar();
	if(c == "s"){
		server(atoi(argv[1]));
	}
	else if(c == "c"){
		client(atoi(argv[1]), argv[2]);
	}
	return 1;

}

int server(int port) {
	printf ("port=%d\n");
	int sockd;
	struct sockaddr_in my_name, cli_name;
	int status;
	int addrlen;
	
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

	addrlen = sizeof(cli_name);
	
	while (1) {
		status = recvfrom(sockd, buf, MAX_BUF, 0, (struct sockaddr*) &cli_name, &addrlen);
				
		sendto(sockd, buf, MAX_BUF, 0, (struct sockaddr*)&cli_name, sizeof(cli_addr));
		
		printf("server : %s\n", buf);
	}
	close(sockd);
}

int client(int port, char addr[]) {	 
	  int sockd;
	  struct sockaddr_in my_addr, srv_addr;
	  char buf[MAX_BUF];
	  int count;
	  int addrlen;
	  int status;
	  	
	char buf[] = "Hello world\n";
	  
	  /* Create a UDP socket */
	  sockd = socket(AF_INET, SOCK_DGRAM, 0);
	  if (sockd == -1) {
	    perror("Socket creation error");
	    exit(1);
	  }
	 
	  /* Configure client address */
	  my_addr.sin_family = AF_INET;
	  my_addr.sin_addr.s_addr = INADDR_ANY;
	  my_addr.sin_port = 0;
	 
	  bind(sockd, (struct sockaddr*)&my_addr, sizeof(my_addr));
	 
	  memset(buf, 0, MAX_BUF);
	  /* Set server address */
	  srv_addr.sin_family = AF_INET;
	  inet_aton(addr, &srv_addr.sin_addr);
	  srv_addr.sin_port = htons(port);
	 
	  while(1) {  		
	      sendto(sockd, buf, MAX_BUF, 0, (struct sockaddr*)&srv_addr, sizeof(srv_addr));
	      
	  		status = recvfrom(sockd, buf, MAX_BUF, 0, (struct sockaddr*) &srv_name, &addrlen);
	  		
	  		printf("client : %s\n", buf);
	  		

	  }
	  
	  close(sockd);
}


// Structure permettant de faire des transferts de variables entre threads
struct arg_thread{
	char c;
	sem_t* bout_vide;
	sem_t* bout_plein;
	char* bout;
}args;


// getshared permet de créer un espace mémoire partagé
// file : nom de cet espace partagé
// size : taille de l'espace mémoire
char* getshared(char* file, int size){	 
	// Obtention d'un segment mémoire en mode lecture/ecriture et en définissant les droits d'accès. Renvoie un descripteur de fichier
	int fd = shm_open(file, O_CREAT | O_RDWR, 0777);
	if(fd<0){
		perror("shm_open\n");
		exit(0);
	}	
	// Fait en sorte que le fichier soit de taille 1000
	int truncate = ftruncate(fd, 1000);
	if(truncate<0){
		perror("truncate\n");
		exit(0);
	}	
	// Permet de partager le fichier entre les processus
	char* ptr = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	if(ptr==MAP_FAILED){
		perror("mmap\n");
		exit(0);	
	}
	return ptr;
}
