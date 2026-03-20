#include"contrlDevices.h"
#include<stdlib.h>
#include<fcntl.h>

int livingroomLightOpen(int pinNum)
{
	digitalWrite(pinNum,LOW);
}

int livingroomLightClose(int pinNum)
{
	digitalWrite(pinNum,HIGH);
}

int livingroomLightCloseInit(int pinNum)
{
	pinMode(pinNum,OUTPUT);
	digitalWrite(pinNum,HIGH);
}

int livingroomLightCloseStatus(int status)
{
	
}

struct Devices livingroomLight=
{
	.deviceName="livingroomLight",
	.pinNum=24,
	.open=livingroomLightOpen,
	.close=livingroomLightClose,
	.deviceInit=livingroomLightCloseInit,
	.changeStatus=livingroomLightCloseStatus
};

struct Devices * addLivingroomLightToDeviceLink(struct Devices * phead)
{
	if(phead==NULL)
		return &livingroomLight;
	else
	{
		livingroomLight.next=phead;
		phead=&livingroomLight;
	}
};

