#include<wiringPi.h>
#include<stdio.h>
#include<wiringSerial.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include"inputCommand.h"
int voiceGetCommand(struct InputCommand *voicer)
{
	int nread=0;
	memset(voicer->command,'\0',sizeof(voicer->command));
	nread=read(voicer->fd,voicer->command,sizeof(voicer->command));
	if(nread==0)
		printf("voice no datas\n");
	else
		return nread;
}

int voiceInit(struct InputCommand *voicer,char *ipAdress,char *port)
{
	int fd;
	if(fd=serialOpen(voicer->deviceName,9600)==-1)
	{
		exit(-1);
	}
	voicer->fd=fd;
	return fd;
}

struct InputCommand voiceContrl=
{
	.commandName="voice",
	.deviceName="/dev/ttyAMA0",
	.command={'\0'},
	.Init=voiceInit,
	.getCommand=voiceGetCommand,
	.log={'\0'},
	.next=NULL
};

struct InputCommand* addvoiceContrlToInputCommandLink(struct  InputCommand *phead)
{
	if(phead==NULL)
		return &voiceContrl;
	else
	{
		voiceContrl.next=phead;
		phead=&voiceContrl;
	}	
}
