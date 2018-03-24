// Structure permettant de faire des transferts de variables entre threads

#ifndef __LECTURE_CLAVIER__
#define __LECTURE_CLAVIER__

typedef struct {
	char c;
	sem_t* bout_vide;
	sem_t* bout_plein;
	char* bout;
} clavierArgsThread;


void* Lectureclavier(void* arguments);

#endif