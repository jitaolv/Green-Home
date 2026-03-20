#include"contrlDevices.h"
#include<stdlib.h>
#include<fcntl.h>

int upstairLightOpen(int pinNum)
{
	digitalWrite(pinNum,LOW);
}

int upstairLightClose(int pinNum)
{
	digitalWrite(pinNum,HIGH);
}

int upstairLightCloseInit(int pinNum)
{
	pinMode(pinNum,OUTPUT);
	digitalWrite(pinNum,HIGH);
}

int upstairLightCloseStatus(int status)
{

}

struct Devices upstairLight=
{
	.deviceName="upstairLight",
	.pinNum=21,
	.open=upstairLightOpen,
	.close=upstairLightClose,
	.deviceInit=upstairLightCloseInit,
	.changeStatus=upstairLightCloseStatus
};

struct Devices * addUpstairLightToDeviceLink(struct Devices * phead)
{
	if(phead==NULL)
		return &upstairLight;
	else
	{
		upstairLight.next=phead;
		phead=&upstairLight;
	}
};
