main(argc,argv) 
int argc ; 
char *argv[] ; 
{ 
	int sd; 
	struct sockaddr_in sa; /* Structure Internet sockaddr_in */ 
	struct hostent *hptr ; /* Infos sur le serveur */ 
	int port; /* Numero de port du serveur */ 
	int newsd; /* Id de la socket entrante */ 
	struct sockaddr_in newsa; /* sockaddr_in de la connection entrante */ 
	int newsalength; struct hostent *newhptr; /* Infos sur le client suivant /etc/hosts */ 
	char buf[256]; 
	int n, i; 
	char accueil[] = "\ HTTP/1.0 200 OK\n\ Content-Type: text/html\n\ \n\ <HTML>\n\ Coucou\n\ <BR>\n\ Salut\n\ <BLINK>\n\ Yolo\n\ </BLINK>\n\ Hihi\n\ <HR>\n\ Ouah\n\ </HTML>";

	/* verification du nombre d'arguments de la ligne de commande */ 
	if (argc != 2) { 
		printf("pingserveurTCP. Erreur d'arguments\n"); 
		printf("Syntaxe : %% pingserveurTCP numero_port\n"); exit(1); 
	} 

	/* Recuperation numero port passe en argument */ 
	port = atoi(argv[1]); 

	/* Initialisation la structure sockaddr sa avec les infos formattees : */

	/* Famille d'adresse : AF_INET = PF_INET */ 
	sa.sin_family = AF_INET; 

	/* Initialisation du numero du port */ 
	sa.sin_port = htons(port); 

	sa.sin_addr.s_addr = INADDR_ANY; 

	/* Creation de la socket TCP */ 
	if((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) { 
		printf("Probleme lors de la creation de socket\n"); 
		exit(1); 
	} 

	/* Bind sur la socket */ 
	if(bind(sd, (struct sockaddr *) &sa, sizeof(sa)) == -1) { 
		printf("Probleme avec le bind\n"); 
		exit(1); 
	} 

	/* Initialisation de la queue d'ecoute des requetes (5 max) */ 
	listen(sd, 5); 
	printf("Serveur pingTCP en ecoute...\n"); 

	i = 0; 
	while(1) { 
		/* Ne pas oublier : newsalength contient la taille de la structure sa attendue */ 
		newsalength = sizeof(newsa) ; 
		if((newsd = accept(sd, ( struct sockaddr * ) &newsa, &newsalength)) < 0 ) { 
			printf("Erreur sur accept\n"); 
			exit(1); 
		} 
		write(newsd, accueil, sizeof(accueil)); 
		close(newsd);
	 } 
	 
	 /* Fermeture du serveur. Never reached */
	close(sd); 
	printf("Fin du serveur. Bye...\n"); 
	exit(0); 
}