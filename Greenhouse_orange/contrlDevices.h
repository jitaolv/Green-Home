#include<wiringPi.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
struct Devices
{
	char deviceName[128];
	int status;
	int pinNum;
	int (*open)(int pinNum);
	int (*close)(int pinNum);
	int (*deviceInit)(int pinNum);
	int (*readStatus)(int pinNum);
	int (*changeStatus)(int status);
	struct Devices *next;
};
struct Devices * addBathroomLightToDeviceLink(struct Devices * phead);
struct Devices * addBedroomLightToDeviceLink(struct Devices * phead);
struct Devices * addRestaurantLightToDeviceLink(struct Devices * phead);
struct Devices * addLivingroomLightToDeviceLink(struct Devices * phead);
struct Devices * addFireToDeviceLink(struct Devices *phead);
struct Devices * addSwimmingPoolLightToDeviceLink(struct Devices * phead);
