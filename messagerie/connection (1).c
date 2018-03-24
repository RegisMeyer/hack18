#include "connection.h"

int timespec_subtract (struct timespec *result, struct timespec *x, struct timespec *y)
{
  /* Perform the carry for the later subtraction by updating Y. */
	
  if (x->tv_nsec < y->tv_nsec) {
    int num_sec = (y->tv_nsec - x->tv_nsec) / 1000000000 + 1;
    y->tv_nsec -= 1000000000 * num_sec;
    y->tv_sec += num_sec;
  }
  if (x->tv_nsec - y->tv_nsec > 1000000000) {
    int num_sec = (x->tv_nsec - y->tv_nsec) / 1000000000;
    y->tv_nsec += 1000000000 * num_sec;
    y->tv_sec -= num_sec;
  }

  /* Compute the time remaining to wait.
     `tv_nsec' is certainly positive. */
  result->tv_sec = x->tv_sec - y->tv_sec;
  result->tv_nsec = x->tv_nsec - y->tv_nsec;

  /* Return 1 if result is negative. */
  return x->tv_sec < y->tv_sec;
}

int receiver(int port) {
	//sysClkRateSet(5000);
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
	
	//Get the initial time
	struct timespec begin, end, result;
	clock_gettime(CLOCK_MONOTONIC, &begin);
	
	while (1) {
		status = recvfrom(sockd, buf, MAX_BUF, 0, (struct sockaddr*) &cli_name, &addrlen);
		//printf("%s!\n", buf);
		
		semTake(graph_array_lock, WAIT_FOREVER);
		//Wait for the first board to send a value
		int ref = atoi(buf);
		ref_position = ref;
		int cycle = pwm_value;
		int irc = position;
			
		//Get the time of the reception
		clock_gettime(CLOCK_MONOTONIC, &end);
		timespec_subtract(&result, &end, &begin);
			
		//Store the values into the struct arrays
		requested_position[length%NB_VALUES].y = ref;
		requested_position[length%NB_VALUES].x = result.tv_sec*1000+result.tv_nsec/1000000;
			
		actual_position[length%NB_VALUES].y = irc;
		actual_position[length%NB_VALUES].x = result.tv_sec*1000+result.tv_nsec/1000000;
				
		PWM_cycle[length%NB_VALUES].y = cycle;
		PWM_cycle[length%NB_VALUES].x = result.tv_sec*1000+result.tv_nsec/1000000;
			
		length++;	
			
		//Set the value of the first value to read in the array
		if(length >= NB_VALUES)
		{
			start = length%NB_VALUES+1;
		}
		else
		{
			start = 0;
		}	
		semGive(graph_array_lock);
	}
	close(sockd);
}

int sender(int port, char addr[]) {	 
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
		  sprintf(buf, "%d", position);
	      sendto(sockd, buf, MAX_BUF, 0, (struct sockaddr*)&srv_addr, sizeof(srv_addr));
	      taskDelay(sysClkRateGet()/100);
	  }
	  
	  close(sockd);
}
