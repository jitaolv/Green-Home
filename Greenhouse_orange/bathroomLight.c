#include"contrlDevices.h"
#include<stdlib.h>
#include<fcntl.h>
int bathroomLightOpen(int pinNum)
{
	digitalWrite(pinNum,LOW);
}
int bathroomLightClose(int pinNum)
{
	digitalWrite(pinNum,HIGH);
}
int bathroomLightCloseInit(int pinNum)
{
	pinMode(pinNum,OUTPUT);
	digitalWrite(pinNum,HIGH);
}
int bathroomLightCloseStatus(int status)
{
	
}
struct Devices bathroomLight=
{
	.deviceName="bathroomLight",
	.pinNum=22,
	.open=bathroomLightOpen,
	.close=bathroomLightClose,
	.deviceInit=bathroomLightCloseInit,
	.changeStatus=bathroomLightCloseStatus
};
struct Devices * addBathroomLightToDeviceLink(struct Devices * phead)
{
	if(phead==NULL)
		return &bathroomLight;
	else
		{
			bathroomLight.next=phead;
			phead=&bathroomLight;
		}
};
