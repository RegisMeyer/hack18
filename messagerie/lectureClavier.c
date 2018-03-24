#include "lectureClavier.h"

// Lectureclavier tourne dans un thread et permet de récupérer les caractères tapés au clavier
// arguments : permet de passer des arguments au thread
void* Lectureclavier(void* arguments){
	clavierArgsThread* args = arguments;
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