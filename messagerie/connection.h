#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "point.h"
#include "irc.h"
#include "pwm.h"
 
#define MAX_BUF 32

char buf[MAX_BUF];

int receiver(int port);
int sender(int port, char addr[]);
