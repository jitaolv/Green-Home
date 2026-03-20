#include"contrlDevices.h"
#include<stdlib.h>
#include<fcntl.h>

int bedroomLightOpen(int pinNum)
{
	digitalWrite(pinNum,LOW);
}

int bedroomLightClose(int pinNum)
{
	digitalWrite(pinNum,HIGH);
}

int bedroomLightCloseInit(int pinNum)
{
	pinMode(pinNum,OUTPUT);
	digitalWrite(pinNum,HIGH);
}

int bedroomLightCloseStatus(int status)
{

}

struct Devices bedroomLight=
{
	.deviceName="bedroomLight",
	.pinNum=23,
	.open=bedroomLightOpen,
	.close=bedroomLightClose,
	.deviceInit=bedroomLightCloseInit,
	.changeStatus=bedroomLightCloseStatus
};

struct Devices * addBedroomLightToDeviceLink(struct Devices * phead)
{
	if(phead==NULL)
		return &bedroomLight;
	else
	{
		bedroomLight.next=phead;
		phead=&bedroomLight;
	}
};
