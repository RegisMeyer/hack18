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
	sem_t* sem_b2_plein = sem_open("/b2_plein", O_CREAT,0777,0);
	if (sem_b2_plein == SEM_FAILED){
		perror("Erreur semaphore : b2_plein\n");
		exit(0);
	}

	// Sémaphore indiquant si b2 est vide : initialisé à 1 (non bloqué)
	sem_t* sem_b2_vide = sem_open("/b2_vide", O_CREAT,0777,1);
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
	bin = b1;
	bout= b2;
	bin_vide = sem_b1_vide;
	bin_plein = sem_b1_plein;
	bout_vide = sem_b2_vide;
	bout_plein = sem_b2_plein;
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