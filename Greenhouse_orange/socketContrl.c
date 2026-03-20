#include<wiringPi.h>
#include<stdio.h>
#include<wiringSerial.h>
#include<unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include<stdlib.h>
#include<string.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include "inputCommand.h"
int socketGetCommand(struct InputCommand *socketMes)
{
	int n_read=0;
	int c_fd;
	struct sockaddr_in c_addr;
	char msg[128];
	memset(&c_addr,0,sizeof(struct sockaddr_in));
	int clen=sizeof(struct sockaddr_in);
	
	c_fd=accept(socketMes->sfd,(struct sockaddr *)&c_addr,&clen);
	n_read=read(c_fd,socketMes->command,sizeof(socketMes->command));
	//if(n_read == -1)
	//perror("read");
	//else 
	if(n_read>0)
		printf("\nget£º%d\n",n_read);
	else
	{
		printf("client quit\n");
	}
	return n_read;
}

int socketInit(struct InputCommand *socketMes,char *ipAdress,char *port)
{
	int s_fd;
	struct sockaddr_in s_addr;
	memset(&s_addr,0,sizeof(struct sockaddr_in));
	//1.socket
	s_fd=socket(AF_INET,SOCK_STREAM ,0);
	if(s_fd==-1)
	{
		perror("socket");
		exit(-1);
	}

	//2.bind
	s_addr.sin_family=AF_INET;
	s_addr.sin_port=htons(atoi(socketMes->port));
	inet_aton(socketMes->ipAddress,&(s_addr.sin_addr));
	
	bind(s_fd,(struct sockaddr *)&s_addr,sizeof(struct sockaddr_in));

	//3.listen
	listen(s_fd,10);
	printf("socket Server listening ...\n");
	socketMes->sfd=s_fd;
	return s_fd;
}

struct InputCommand socketContrl=
{
	.commandName="socketServer",
	.command={'\0'},
	.port="8082",
	.ipAddress="192.168.137.175",
	.Init=socketInit,
	.getCommand=socketGetCommand,
	.log={'\0'},
	.next=NULL
};

struct InputCommand* addSocketContrlToInputCommandLink(struct  InputCommand *phead)
{
	if(phead==NULL)
		return &socketContrl;
	else
	{
		socketContrl.next=phead;
		phead=&socketContrl;
	}	
}
