#include"contrlDevices.h"
int firelfOrNotInit(int pinNum)
{
	pinMode(pinNum,INPUT);
	digitalWrite(pinNum,HIGH);
}
int fireStatueRead(int pinNum)
{
	return digitalRead(pinNum);
}
struct Devices fireIfOrNot=
{
	.deviceName="fireIfOrNot",
	.pinNum=7,
	.deviceInit=firelfOrNotInit,
	.readStatus=fireStatueRead
};
struct Devices* addFireToDeviceLink(struct Devices *phead)
{
	if(phead==NULL)
	{
		return &fireIfOrNot;
	}
	else
	{
		fireIfOrNot.next=phead;
		phead=&fireIfOrNot;
	}
}
