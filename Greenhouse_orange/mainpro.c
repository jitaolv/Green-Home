#include<stdio.h>
#include"contrlDevices.h"
#include<string.h>
#include<unistd.h>
#include"inputCommand.h"
#include "pthread.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include<wiringSerial.h>
#include<wiringPi.h>
#include<softPwm.h>
#define led1 24
#define led2 25
#define led3 23
#define led4 22
#define pwm_pin 1
//设备结构体、指令结构体、网络结构体，不动就行
struct Devices *pdeviceHead=NULL;
struct InputCommand *pCommandHead=NULL;
struct InputCommand *socketHandler=NULL;
/*
 * 标识符就看成是某个设备/文件的 ID，Linux系统下一切皆文件思想
 * c_fd:客户端网络标识符
 * fd0: 串口0标识符  TX 用于mp3 music player 播放音乐   Rx用于接收LD3322语音识别模块指令
 * fd2：串口2标识符  Tx 用于 NB 打电话串口  Rx用于接收openMV人脸数据
 * flag：网络接收数据标志位，接收到了然后发，发线程还没有做好
 * Send_bbuf：网络发送buf
 */
int c_fd,fd0,fd2,flag=0,Send_buf[10];
//需拨打的电话、NB模组记得要插打正常的电话卡，模组要按一下闪绿灯才算启动，末尾 分号和\n 不要改
char number[]={"ATD19883516906;\n"};
char msg_32[128]={'\0'};
//硬件端口初始化

void HARDWARE_Init()
{
	pinMode(led1,OUTPUT);
	pinMode(led2,OUTPUT);
	pinMode(led3,OUTPUT);
	pinMode(led4,OUTPUT);
	//pwm初始化
	softPwmCreate(pwm_pin,0,50);
	softPwmCreate(4,0,50);
	softPwmWrite(pwm_pin,15);
	softPwmWrite(4,15);
	
	digitalWrite(led1,HIGH);
	digitalWrite(led2,HIGH);
	digitalWrite(led3,HIGH);
	digitalWrite(led4,HIGH);
	//打开串口2  Tx 用于 NB 打电话串口 Rx用于接收openMV人脸数据
	fd2 = serialOpen("/dev/ttyAMA2",9600);  
	//打开川口0  TX 用于 music player 播放音乐   Rx用于接收LD3322语音识别模块指令
	fd0 = serialOpen("/dev/ttyS0",9600);
	//串口1是用于和STm32的全双工通信接口，在STM32线程中进行配置与初始化	
}
//关灯
void closeAllLight()
{
	digitalWrite(led1,HIGH);
	digitalWrite(led2,HIGH);
	digitalWrite(led3,HIGH);
	digitalWrite(led4,HIGH);
}
//开灯
void openAllLight()
{
	digitalWrite(led1,LOW);
	digitalWrite(led2,LOW);
	digitalWrite(led3,LOW);
	digitalWrite(led4,LOW);
}
//music player驱动程序，不改
void SendCmd(int len)
{
	int a=0;
	serialPutchar(fd0,0x7E);
	for(a=0;a<len;a++)
	{
		serialPutchar(fd0,Send_buf[a]);
	}
	serialPutchar(fd0,0xEF);
}
//music player驱动程序，不改
void DoSum(int *str,int len)
{
	int xorsum=0;
	int a;
	for(a=0;a<len;a++)
	{
		xorsum=	xorsum+str[a];
	}
	xorsum=0-xorsum;
	*(str+a)=(int)(xorsum>>8);
	*(str+a+1)=(int)(xorsum&0x00ff);
}

/*
 * music player发送指令程序，用于播放不同序号的音乐
 * CMD      ： 0x03   (不变)
 * feedback :   0     (不变)
 * dati     :   歌曲序号(十六进制)   例如：0x01  0x03  0x1e 等
 */
void Uart_SendCMD(int CMD,int feedback,int dati)
{
	Send_buf[0]=0xff;
	Send_buf[1]=0x06;
	Send_buf[2]=CMD;
	Send_buf[3]=feedback;
	Send_buf[4]=(int) (dati>>8);
	Send_buf[5]=(int) (dati);
	DoSum(&Send_buf[0],6);
	SendCmd(8);
}
//工厂模式：工厂加入设备节点（不变）
struct Devices *findDeviceByName(char *name,struct Devices *phead)
{
	struct Devices *tmp=phead;
	if(phead==NULL)
	{
		return NULL;
	}
	else
	{
		while(tmp!=NULL)
		{
			if(strcmp(tmp->deviceName,name)==0)
			{
				return tmp;
			}
			tmp=tmp->next;
		}
		return tmp;
	}
}
//工厂模式：查找指令（不变）
struct InputCommand *findCommandByName(char *name,struct InputCommand *phead)
{
	struct InputCommand *tmp=phead;
	if(phead==NULL)
	{
		return NULL;
	}
	else
	{
		while(tmp!=NULL)
		{
			if(strcmp(tmp->commandName,name)==0)
			{
				return tmp;
			}
			tmp=tmp->next;
		}
		return NULL;
	}
}



//通过翔云人工智能云平台（有次数限制，还要钱）人脸识别，主要涉及拍照、对接与接收
////现在不用了，现在用openMV了，想用结合get.c自行研究
void face(void)
{
	char ch[1];
	FILE *p;
	int count=3;	//一定每次启动最多识别3次即可，1分钟就没次数了
	int i;
	while(count>0)
	{
		memset(ch,'0',1);
		printf("photeing.......\n");
		//以下接的树莓派摄像头拍照
		//system("raspistill -t 2000 -o img2.jpg -q 5");
		//以下是uSB拍照程序，记得修改ip+端口号，这个端口不是网络端口，是在摄像头那边设备的端口，默认是8080
		system("wget  http://192.168.43.216:8081/?action=snapshot -O ./img2.jpg");
		ch[0]='0';	
		printf("photo successed! please waiting...\n"); 
		p=popen("./facerx","r"); 	//启动get.c编译出来的可执行程序
		fread(ch,1,1,p);                        
		if(ch[0]=='1')          
		{
			printf("The same person\n");
			openAllLight();
			sleep(5);
			closeAllLight();
			count=0;		//识别成功就提前结束本次识别，不然在浪费次数
		}
		else
			printf("The different person\n");
		count--;
	}
}
//语音线程，用于接收LD3322语音模块数据
//线程可当做中断服务函数理解，为核心控制代码区
void *voice_thread(void *datas)
{	
	int nread;		//接收串口接收的字符指令，LD3322发的是字符
	char msg[128]={'\0'};	//接收数据msg
//	Uart_SendCMD(0x03,0,0x01);	//园区初始化语音播报
	while(1)
	{
		while(serialDataAvail(fd0)!=-1 )	//有数据来了
		{
			nread=read(fd0,msg,128);	//接收数据，接收的数据msg，返回接收到的字节数存入nread
			if(nread==0)			//没数据
				continue;
			//打印接收来自LD3322的n字节数据 msg
			printf("From LD3320 get :%d Byte context:%s \n",nread,msg);
			//判断接收到的数据，然后开灯+播放音乐
			if(strstr(msg,"A")!=NULL){
				Uart_SendCMD(0x03,0,0x02);
			} 

			if(strstr(msg,"B")!=NULL){
			//	Uart_SendCMD(0x03,0,0x04);
				digitalWrite(led3,LOW);
			}
			if(strstr(msg,"C")!=NULL){
				Uart_SendCMD(0x03,0,0x05);
				digitalWrite(led2,LOW);
			}
			if(strstr(msg,"D")!=NULL){
			//	Uart_SendCMD(0x03,0,0x04);
				digitalWrite(led1,LOW);
			}
			if(strstr(msg,"E")!=NULL){
				Uart_SendCMD(0x03,0,0x05);
				digitalWrite(led4,LOW);
			}
			if(strstr(msg,"F")!=NULL){
				closeAllLight();
			//	Uart_SendCMD(0x03,0,0x04);
			}
			if(strstr(msg,"G")!=NULL){
				openAllLight();
			//	Uart_SendCMD(0x03,0,0x04);
			}
			
			if(strstr(msg,"P")!=NULL)
				Uart_SendCMD(0x03,0,0x0b);
			if(strstr(msg,"Q")!=NULL)
				Uart_SendCMD(0x03,0,0x0b);
			if(strstr(msg,"R")!=NULL)
				Uart_SendCMD(0x03,0,0x15);
			if(strstr(msg,"S")!=NULL)
				Uart_SendCMD(0x03,0,0x0d);
			if(strstr(msg,"T")!=NULL)
				Uart_SendCMD(0x03,0,0x0e);
			if(strstr(msg,"U")!=NULL)
				Uart_SendCMD(0x03,0,0x21);
			//清空接收区
			 memset(msg,'\0',128);
		}
	}		
}
//网络接收线程，用于读取来自客户端的指令并做控制
void *read_thread(void *datas)
{
	int n_read;			//读取到的字节数
	struct Devices *tmp=NULL;	//暂存设备节点
	memset(socketHandler->command,'\0',sizeof(socketHandler->command));	//清空接收区
	//接收区为 socketHandler->command	结构体成员，看成buf字符串就行
	while(1)
	{		
		//网络控制硬件
		//接收，返回接收到的字节数放入n_read，指令在socketHandler->command内
		n_read=read(c_fd,socketHandler->command,sizeof(socketHandler->command));
		//打印接收到的数据
		printf("\n ********get %d Byte：%s*************\n",n_read,socketHandler->command);
	
		//通过名称找设备，设备名称存入tmp
		tmp=findDeviceByName(socketHandler->command,pdeviceHead);
		//控制方式1：这里就是工厂模式的开灯指令
		if(tmp!=NULL)
		{
			tmp->deviceInit(tmp->pinNum);
			tmp->open(tmp->pinNum);
		}

		//给客户端发送指令		
		//	write(c_fd,socketHandler->command,sizeof(socketHandler->command));
		flag=1;		//说明收到指令，在发送线程调用
		
		
		//控制方式2：直接判断接收到的字符串，接收到关灯指令，关灯
		if(strstr(socketHandler->command,"closeLED1")!=NULL)	
			digitalWrite(led3,HIGH);
		if(strstr(socketHandler->command,"closeLED2")!=NULL)
			digitalWrite(led4,HIGH);
		if(strstr(socketHandler->command,"closeLED3")!=NULL)
			digitalWrite(led1,HIGH);
		if(strstr(socketHandler->command,"closeLED4")!=NULL)
			digitalWrite(led2,HIGH);
		if(strstr(socketHandler->command,"openwindow")!=NULL)
		{
			softPwmWrite(4,5);
			softPwmWrite(1,5);
		}
		if(strstr(socketHandler->command,"closewindow")!=NULL)
		{
			
			softPwmWrite(4,15);
			softPwmWrite(1,15);
		}
		//手机发送火灾指令，路演时偷偷强行打电话
		if(strstr(socketHandler->command,"fire")!=NULL)
		{	
			write(fd2,number,sizeof(number));
			printf("Called\n");
		}

		//清空接收区
		memset(socketHandler->command,'\0',sizeof(socketHandler->command));
	}
}


//openMV串口线程，用于接收openMV的数据
void *openMV_thread(void *datas)
{
	int nread2;				//接收openMV数据字节数
	char msg_mv[128]={'\0'};		//接收openMV
	while(1)
	{	
		memset(msg_mv,'\0',128);	//清空接收区
		while(serialDataAvail(fd2)!=-1)
		{

			nread2=read(fd2,msg_mv,128);	//读取，数据存入msg_mv，返回字节存入nread2
			if(nread2==0)
				continue;
			printf("get open_MV:%d Byte context:%s \n",nread2,msg_mv);	//打印接收到哦数据和字节
			if(strstr(msg_mv,"face")!=NULL) 			//接收到face指令
				openAllLight();					//开灯并延时
			if(strstr(msg_mv,"A")!=NULL)	
			Uart_SendCMD(0x03,0,0x19);
		//	openAllLight();
			if(strstr(msg_mv,"B")!=NULL)
			Uart_SendCMD(0x03,0,0x1A);	
			if(strstr(msg_mv,"C")!=NULL)
			Uart_SendCMD(0x03,0,0x1B);	
			if(strstr(msg_mv,"D")!=NULL)
			Uart_SendCMD(0x03,0,0x1C);
			memset(msg_mv,'\0',128);				//情况字符串
		}
	}
}

//与STM32通信进行
void *STM32_thread(void *datas)
{
	int nread1,fd1;
	fd1=serialOpen("/dev/ttyAMA1",9600);    
	while(1)
	{
		while(serialDataAvail(fd1)!=-1)
		{
			nread1=read(fd1,msg_32,128);
			if(nread1==0)
				continue;
			//	printf("get data from stm32:%d Byte context:%s\n",nread1,msg_32);	
			printf("get:%s\r\n",msg_32);
			delayMicroseconds(3000000);
			//STM32接收来自云平台数据后，将数据发送给树莓派	
			//server open
			if(strstr(msg_32,"enLED1")!=NULL)
				digitalWrite(led1,LOW);
			if(strstr(msg_32,"seLED1")!=NULL)
				digitalWrite(led1,HIGH);
			if(strstr(msg_32,"enLED2")!=NULL)
				digitalWrite(led2,LOW);
			if(strstr(msg_32,"seLED2")!=NULL)
				digitalWrite(led2,HIGH);
			//server close
			if(strstr(msg_32,"enLED3")!=NULL)
				digitalWrite(led3,LOW);
			if(strstr(msg_32,"seLED3")!=NULL)
				digitalWrite(led3,HIGH);
			if(strstr(msg_32,"enLED4")!=NULL)
				digitalWrite(led4,LOW);
			if(strstr(msg_32,"seLED4")!=NULL)
				digitalWrite(led4,HIGH);

			//火灾信号，这个上云的
			if(strstr(msg_32,"fire")!=NULL)
			{
				write(fd2,number,sizeof(number));
				printf("get STM32 fire\n");
			}
			memset(msg_32,'\0',128);	
		}
	}
}

void *write_thread(void *datas) //通知线程，向客服端发送消息
{
	while (1)
	{
		if(flag==1)
		{
			//printf("%s\n",msg_32);
			//	delayMicroseconds(3000000);	
			//	write(c_fd,msg_32,128);
		}
	}
}
//网络线程：创建网络TCP服务器，并等待客户端接入
void *socket_thread(void *datas)
{
	int n_read=0;
	pthread_t readThread;
	int clen=sizeof(struct sockaddr_in);
	struct sockaddr_in c_addr;
	memset(&c_addr,0,sizeof(struct sockaddr_in));
	socketHandler=findCommandByName("socketServer",pCommandHead);
	if(socketHandler==NULL)
	{
		printf("find socketHandler error\n");
		pthread_exit(NULL);
	}
	else
	{
		printf("%s init success\n",socketHandler->commandName);
	}
	socketHandler->Init(socketHandler,NULL,NULL);
	while(1)
	{
		c_fd=accept(socketHandler->sfd,(struct sockaddr *)&c_addr,&clen);
		pthread_create(&readThread,NULL,read_thread,NULL);
	}
}

int main()
{
	//进程变量
	pthread_t voiceThread;
	pthread_t socketThread;
	pthread_t STM32Thread;
	pthread_t openMVThread;
	pthread_t writeThread;
	
	//树莓派库初始化
	if(-1==wiringPiSetup())
	{
		printf("setup error\n");
		exit(-1);
	}
	
	//硬件初始化
	HARDWARE_Init();

	//添加指令池、设备池到链表
	pCommandHead=addvoiceContrlToInputCommandLink(pCommandHead);
	pCommandHead=addSocketContrlToInputCommandLink(pCommandHead);
	pdeviceHead=addBathroomLightToDeviceLink(pdeviceHead);
	pdeviceHead=addBedroomLightToDeviceLink(pdeviceHead);
	pdeviceHead=addRestaurantLightToDeviceLink(pdeviceHead);
	pdeviceHead=addLivingroomLightToDeviceLink(pdeviceHead);

	//创建线程
	pthread_create(&voiceThread,NULL,voice_thread,NULL);
	pthread_create(&socketThread,NULL,socket_thread,NULL);
	pthread_create(&STM32Thread,NULL,STM32_thread,NULL);	
	pthread_create(&openMVThread,NULL,openMV_thread,NULL);	
	pthread_create(&writeThread,NULL,write_thread,NULL);	
	
	//加入线程
	pthread_join(voiceThread,NULL);
	pthread_join(socketThread,NULL);
	pthread_join(STM32Thread,NULL);
	pthread_join(openMVThread,NULL);
	pthread_join(writeThread,NULL);

	return 0;                               
}


