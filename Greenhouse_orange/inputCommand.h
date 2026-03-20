#include<wiringPi.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
struct InputCommand
{
	char commandName[128];
	char deviceName[128];
	char command[32];
	int (*Init)(struct InputCommand *voicer,char *ipAdress,char *port);
	int (*getCommand)(struct InputCommand *voicer);	
	char log[1024];
	int fd;
	char port[12];
	char ipAddress[32];
	int sfd;
	struct InputCommand *next;
};

struct InputCommand* addvoiceContrlToInputCommandLink(struct  InputCommand *phead);
struct InputCommand* addSocketContrlToInputCommandLink(struct  InputCommand *phead);


