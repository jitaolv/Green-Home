#include"contrlDevices.h"
#include<stdlib.h>
#include<fcntl.h>

int swimmingPoolLightOpen(int pinNum)
{
	digitalWrite(pinNum,LOW);
}

int swimmingPoolLightClose(int pinNum)
{
	digitalWrite(pinNum,HIGH);
}

int swimmingPoolLightCloseInit(int pinNum)
{
	pinMode(pinNum,OUTPUT);
	digitalWrite(pinNum,HIGH);
}

int swimmingPoolLightCloseStatus(int status)
{

}

struct Devices swimmingPoolLight=
{
	.deviceName="swimmingPoolLight",
	.pinNum=25,
	.open=swimmingPoolLightOpen,
	.close=swimmingPoolLightClose,
	.deviceInit=swimmingPoolLightCloseInit,
	.changeStatus=swimmingPoolLightCloseStatus
};

struct Devices * addSwimmingPoolLightToDeviceLink(struct Devices * phead)
{
	if(phead==NULL)
		return &swimmingPoolLight;
	else
	{
		swimmingPoolLight.next=phead;
		phead=&swimmingPoolLight;
	}
};

