/*
#include <stdio.h>
#include <curl/curl.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


char buf[1024]={'\0'};

typedef unsigned int bool;
#define false 0
#define true  1

size_t readData( void *ptr, size_t size,size_t nmemb, void *stream)
{
	strncpy(buf,ptr,1024);
//	printf("\n=====================get data==============================\n");
//	printf("%s\n",buf);
}


char *getBase64(char *filePath)
{
	char *bufPic;
	char cmd[128]={'\0'};
	sprintf(cmd,"base64 %s > tempFile",filePath);
	system(cmd);

	int fd=open("./tempFile",O_RDWR);
        int filelen=lseek(fd,0,SEEK_END);
        lseek(fd,0,SEEK_SET);
        bufPic=(char *)malloc(filelen+2);
        memset(bufPic,'\0',filelen+2);
        read(fd,bufPic,filelen);
        close(fd);	
	
	system("rm -f tempFile");
	return bufPic;

}
bool postUrl()
{
	CURL *curl;
	CURLcode res;
	char *postString;
	
	char *key="5VSnKr8tPXTF7fNRjwDd54";
	char *secret="f1fd0787398941c2b6895c0340a9cea1";
	int typeId=21;
	char *format= "xml";
	
	char *bufimg1=getBase64("./img1.jpg");
	char *bufimg2=getBase64("./img2.jpg");
	
	int len=strlen(key)+strlen(secret)+strlen(bufimg1)+strlen(bufimg2)+128;	
	postString=(char *)malloc(len);
	memset(postString,'\0',len);

	sprintf(postString,"&img1=%s&img2=%s&key=%s&secret=%s&typeId=%d&format=%s",bufimg1,bufimg2,key,secret,21,"xml");
//	free(bufimg1);
//	free(bufimg2);

	curl = curl_easy_init();
	if (curl)
	{
		curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "/tmp/cookie.txt"); // 指定cookie文件
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postString);    // 指定post内容
		curl_easy_setopt(curl, CURLOPT_URL, "https://netocr.com/api/faceliu.do");   // 指定url
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,readData); //readdata
		res = curl_easy_perform(curl);
//		printf("OK:%d\n",res);	
		if(strstr(buf,"是")!=NULL)
			printf("1\n");
		else
			printf("0\n");
		curl_easy_cleanup(curl);
	}	return true;
}

int main(void)
{
	postUrl();
}*/
