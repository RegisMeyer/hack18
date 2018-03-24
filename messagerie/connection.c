#include "connection.h"

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

int server(int port) {
	printf ("port=%d\n", port);
	int sockd;
	struct sockaddr_in my_name, cli_addr;
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
		sendto(sockd, buf, MAX_BUF, 0, (struct sockaddr*)&cli_addr, sizeof(cli_addr));
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
		 if (status < 0){
				perror("Erreur bind client\n");
				exit(0);
			}
	  memset(buf, 0, MAX_BUF);
	  /* Set server address */
	  srv_addr.sin_family = AF_INET;
	  inet_aton(addr, &srv_addr.sin_addr);
	  srv_addr.sin_port = htons(port);
	 
	  while(1) {  		
	      status = sendto(sockd, buf, MAX_BUF, 0, (struct sockaddr*)&srv_addr, sizeof(srv_addr));
	      if (status < 0){
			perror("Erreur sendto client\n");
			exit(0);
		}
	  		status = recvfrom(sockd, buf, MAX_BUF, 0, (struct sockaddr*) &srv_addr, &addrlen);
	  		if (status < 0){
			perror("Erreur recvfrom client\n");
			exit(0);
		}
	  		printf("client : %s\n", buf);
	  		

	  }
	  
	  close(sockd);
}
