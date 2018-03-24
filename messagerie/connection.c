#include "connection.h"

int client(int port) {
	printf ("port=%d\n");
	int sockd;
	struct sockaddr_in my_name, cli_name;
	//char buf[MAX_BUF];
	int status;
	int addrlen;

	printf("port = %d\n", port);
	
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
	}
	close(sockd);
}

int server(int port, char addr[]) {	 
	  int sockd;
	  struct sockaddr_in my_addr, srv_addr;
	  char buf[MAX_BUF];
	  int count;
	  int addrlen;
	  
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


void* Lectureclavier(void* arguments);


int main (int argc, char *argv[])
{ 
	// Buffer d'entrée contenant le caractère qui devra être écrit à l'écran (stdin)
	char* bin;
	// Buffer de sortie contenant le caractère qui a été lu sur le clavier (stdout)
	char* bout;

	sem_t* bin_vide;
	sem_t* bin_plein;
	sem_t* bout_vide;
	sem_t* bout_plein;


	// Création des 4 sémaphores (on a besoin de 4 sémaphores car on ne peut que attendre le passage à 1 d'un sémaphore, pas le passage à 0)
	
	// Sémaphore indiquant si b1 est plein : initialisé à 0 (bloqué)
	sem_t* sem_b1_plein = sem_open("/b1_plein", O_CREAT,0777,0);
	if (sem_b1_plein == SEM_FAILED){
		perror("Erreur semaphore : b1_plein\n");
		exit(0);
	}

	// Sémaphore indiquant si b1 est vide : initialisé à 1 (non bloqué)
	sem_t* sem_b1_vide = sem_open("/b1_vide", O_CREAT,0777,1);
	if (sem_b1_vide == SEM_FAILED){
		perror("Erreur semaphore : b1_vide\n");
		exit(0);
	}

	// Sémaphore indiquant si b2 est plein : initialisé à 0 (bloqué)
	sem_t* sem_b2_plein = sem_open("/b2_pleinn", O_CREAT,0777,0);
	if (sem_b2_plein == SEM_FAILED){
		perror("Erreur semaphore : b2_plein\n");
		exit(0);
	}

	// Sémaphore indiquant si b2 est vide : initialisé à 1 (non bloqué)
	sem_t* sem_b2_vide = sem_open("/b2_videe", O_CREAT,0777,1);
	if (sem_b2_vide == SEM_FAILED){
		perror("Erreur semaphore : b2_vide\n");
		exit(0);
	}
	printf("Creation des sémaphores terminé\n");


	// Création de deux espaces mémoires partagés. L'un représentera le buffer d'entrée et l'autre le buffer de sortie
	char *b1 = getshared("/b111", sizeof(char));
	char *b2 = getshared("/b222", sizeof(char));
	printf("Creation des zones partagées terminé\n");
	

	// Attribution des objets : on inverse les buffers b1 et b2 selon le terminal dans lequel on lance le programme
	if(strcmp(argv[1],"1")==0){
		bin = b1;
		bout= b2;
		bin_vide = sem_b1_vide;
		bin_plein = sem_b1_plein;
		bout_vide = sem_b2_vide;
		bout_plein = sem_b2_plein;
	} else {
		bin = b2;
		bout= b1;
		bin_vide = sem_b2_vide;
		bin_plein = sem_b2_plein;
		bout_vide = sem_b1_vide;
		bout_plein = sem_b1_plein;
	}
	printf("Attribution des objets terminé\n");

	char c;
	int ret;
	
	// Attribution des arguments du thread
	args.c=c;
	args.bout_vide=bout_vide;
	args.bout_plein=bout_plein;
	args.bout=bout;

	// Création d'un thread pour la lecture du clavier
	pthread_t threadID;
	// On crée un thread dans lequel la fonction Lectureclavier sera exécutée. On passe également les arguments contenus dans args
	ret = pthread_create(&threadID, NULL, (void*)Lectureclavier, (void*)&args);
	// Vérifie s'il y a eu une erreur à la création du thread
	if (ret!=0) { perror("pthread"); }
	printf("Création du thread terminé\n");


	// Dans cette boucle infinie, on lit le contenu du buffer d'entrée et on l'affiche à l'écran
	while(1){
		
		// On attend que le sémaphore bin_plein soit libre : bin est alors plein
		ret=sem_wait(bin_plein);
		if (ret!=0) { perror("semwait"); }
		
		// Lorsque bin est plein, on lit le contenu de bin et on l'écrit dans c
		c = *bin;
		
		// On libère le sémaphore bin_vide pour indiquer qu'on a lu le contenu de bin
		ret=sem_post(bin_vide);
		if (ret!=0) { perror("sempost"); }

		// On écrit c à l'écran
		ret = write(STDOUT_FILENO, &c, 1);
		if (ret<0) { perror("write"); }
	}

	return 0;
}


// Lectureclavier tourne dans un thread et permet de récupérer les caractères tapés au clavier
// arguments : permet de passer des arguments au thread
void* Lectureclavier(void* arguments){
	struct arg_thread *args = arguments;
	int ret;
	
	while(1){
		// On lit stdin et on écrit ce qui a été lu dans args->c. Read est bloquant s'il n'y a rien à lire
		ret = read(STDIN_FILENO,&args->c, 1);
		if (ret<0) { perror("read"); }
		
		// On attend que bout soit vide
		ret = sem_wait(args->bout_vide);
		if (ret!=0) { perror("semwait"); }

		// On écrit dans bout le contenu de args->c
		*(args->bout) = args->c;

		// On libère le sémaphore bout_plein pour indiquer qu'on a remplit bout
		ret = sem_post(args->bout_plein);
		if (ret!=0) { perror("semwait"); }
	}
}


