#include "gnrmc.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

struct GNRMC_Info
{
unsigned int year;
unsigned char month;
unsigned char day;
unsigned char hour;
unsigned char Minute;
unsigned char second;


unsigned int longitude; 
unsigned char longitude_suffix; // E或W

unsigned int latitude; 
unsigned char latitude_suffix; 
unsigned char position_valid; // 有效

};
struct China_Time
{
	unsigned int year;
	unsigned char month;
	unsigned char day;
	unsigned char hour;
	unsigned char Minute;
	unsigned char second;
};
struct China_Time Ch_TM;

int comma_split(unsigned char *pstr, char *buf, int buflen, int s, int e)
{
    int i, k;
    int commaCount = 0;
    int rc = -1;
    i = k = 0;
    if (e <= s)
    {
        return -1;
    }
    while (pstr[i] != '\0')
    {
        if (pstr[i] == ',')
        {
            commaCount++;
        }
        if (commaCount == s)
        {
            k = ++i;
            break;
        }
        i++;
    }

    while (pstr[i] != '\0')
    {
        if (pstr[i] == ',')
        {
            commaCount++;
        }
        if (commaCount == e)
        {
            if (i - k + 1 > buflen)
            {
                return -1;
            }
            memset(buf, 0, sizeof(buf));
            memcpy(buf, pstr + k, i - k);

            rc = i - k;
            buf[rc] = '\0';
            //printf("%s\n", buf);
            break;
        }
        i++;
    }
    return rc;
}

int GNRMC_Parse(unsigned char *pstr, struct GNRMC_Info *pGNRMCInfo)
{
    int len = 0;
    int commaCount = 0;
    double temp = 0;
    char buf[32] = {0};
    // 计算字符串长度
    while (pstr[len] != '\0')
    {
        if (pstr[len] == ',')
        {
            commaCount++;
        }
        len++;
    }
    // 逗号个数是12个
    if (commaCount != 12)
    {
        return -1;
    }
    // 判断开始字符是:$GNRMC
    if (!((pstr[0] == '$') && (pstr[1] == 'G') && (pstr[2] == 'N') && (pstr[3] == 'R') && (pstr[4] == 'M') && (pstr[5] == 'C')))
    {
        return -1;
    }
    // 判断结束字符是:\r\n
    if ((pstr[len - 2] != '\r') && (pstr[len - 1] != '\n'))
    {
        return -1;
    }
    // UTC时间:第1个逗号与第2个逗号之前 (eg:092846.400 hhmmss.sss )
    len = comma_split(pstr, buf, sizeof(buf), 1, 2);
    if (len < 0)
    {
        return -1;
    }
    pGNRMCInfo->hour = (buf[0] - '0') * 10 + (buf[1] - '0');
    pGNRMCInfo->Minute = (buf[2] - '0') * 10 + (buf[3] - '0');
    pGNRMCInfo->second = (buf[4] - '0') * 10 + (buf[5] - '0');

    // 定位状态:第2个逗号与第3个逗号之前
    len = comma_split(pstr, buf, sizeof(buf), 2, 3);
    if (len != 1)
    {
        return -1;
    }
    pGNRMCInfo->position_valid = ((buf[0] == 'A') ? 1 : 0);

    // 经度
    len = comma_split(pstr, buf, sizeof(buf), 3, 4);
    if (len < 0)
    {
        return -1;
    }
    temp = atof(buf);
    temp = temp * 10000;
    pGNRMCInfo->longitude = temp;
    // 经度半球E(东经)或W(西经)
    len = comma_split(pstr, buf, sizeof(buf), 4, 5);
    if (len != 1)
    {
        return -1;
    }
    pGNRMCInfo->longitude_suffix = buf[0];
    // 纬度
    len = comma_split(pstr, buf, sizeof(buf), 5, 6);
    if (len < 0)
    {
        return -1;
    }
    temp = atof(buf);
    temp = temp * 10000;
    pGNRMCInfo->latitude = temp;

    // 纬度半球N(北半球)或S(南半球)
    len = comma_split(pstr, buf, sizeof(buf), 6, 7);
    if (len != 1)
    {
        return -1;
    }
    pGNRMCInfo->latitude_suffix = buf[0];

    // UTC日期:ddmmyy(日月年)格式 eg:070417
    len = comma_split(pstr, buf, sizeof(buf), 9, 10);
    if (len < 0)
    {
        return -1;
    }
    pGNRMCInfo->day = (buf[0] - '0') * 10 + (buf[1] - '0');
    pGNRMCInfo->month = (buf[2] - '0') * 10 + (buf[3] - '0');
    pGNRMCInfo->year = (buf[4] - '0') * 10 + (buf[5] - '0') + 2000;

    return 0;
}

void UTCToChina(unsigned int UTCYear,unsigned char UTCMonth,unsigned char UTCDay,unsigned char UTCHour,unsigned char UTCMinutes,unsigned char UTCSeconds)
{
    unsigned int ChinaYear = UTCYear;
    unsigned char ChinaMonth = UTCMonth;
    unsigned char ChinaDay = UTCDay;
    unsigned char ChinaSeconds = UTCSeconds;
    unsigned char ChinaMinutes = UTCMinutes;
    unsigned char ChinaHour = UTCHour + 8;
    if (ChinaHour > 23)
    {
        ChinaHour -= 24;
        ChinaDay++;
        switch (ChinaMonth)
        {
        case 1:
            if (ChinaDay > 31)
            {
                ChinaDay = 1;
                ChinaMonth++;
            }
            break;
        case 2:
            if ((0 == ChinaYear % 4 && 0 != ChinaYear % 100) || 0 == ChinaYear % 400)
            {
                if (ChinaDay > 29)
                {
                    ChinaDay = 1;
                    ChinaMonth++;
                }
            }
            else
            {
                if (ChinaDay > 28)
                {
                    ChinaDay = 1;
                    ChinaMonth++;
                }
            }
            break;
        case 3:
            if (ChinaDay > 31)
            {
                ChinaDay = 1;
                ChinaMonth++;
            }
            break;
        case 4:
            if (ChinaDay > 30)
            {
                ChinaDay = 1;
                ChinaMonth++;
            }
            break;
        case 5:
            if (ChinaDay > 31)
            {
                ChinaDay = 1;
                ChinaMonth++;
            }
            break;
        case 6:
            if (ChinaDay > 30)
            {
                ChinaDay = 1;
                ChinaMonth++;
            }
            break;
        case 7:
            if (ChinaDay > 31)
            {
                ChinaDay = 1;
                ChinaMonth++;
            }
            break;
        case 8:
            if (ChinaDay > 31)
            {
                ChinaDay = 1;
                ChinaMonth++;
            }
            break;
        case 9:
            if (ChinaDay > 30)
            {
                ChinaDay = 1;
                ChinaMonth++;
            }
            break;
        case 10:
            if (ChinaDay > 31)
            {
                ChinaDay = 1;
                ChinaMonth++;
            }
            break;
        case 11:
            if (ChinaDay > 30)
            {
                ChinaDay = 1;
                ChinaMonth++;
            }
            break;
        case 12:
            if (ChinaDay > 31)
            {
                ChinaDay = 1;
                ChinaMonth = 1;
                ChinaYear++;
            }
            break;
        default:
            break;
        }
    }
	Ch_TM.year=ChinaYear;
	Ch_TM.month=ChinaMonth;
	Ch_TM.day=ChinaDay;
	Ch_TM.hour=ChinaHour;
	Ch_TM.Minute=ChinaMinutes;
	Ch_TM.second=ChinaSeconds;
}
	/*for( int i = 0; i < 4; i++ )//这部分为发送数组赋值，整形拆分为4个字节
	{
		sy[ i ] = (char)(GNRMC.longitude >> ( 24 - i*8 ));
	}*/
char buff[]={'$','G','N','R','M','C',',','1','3','2','2','2','2','.','0','0','0',',','A',',',
			'3','1','0','3','.','8','9','6','6',',','N',',','1','2','1','1','8','.','3','5','7',
			'1',',','E',',','1','.','6','9',',','3','2','2','.','3','9',',','1','3','0','2','1','9',
			',',',',',','A','*','7','6','\r','\n'};//Test

//int gps(int argc, char* argv[])
//{
//	struct GNRMC_Info GNRMC;

//	char* p= "$GNRMC,132222.000,A,3103.8966,N,12118.3571,E,1.69,322.39,130219,,,A*76\r\n";
//	GNRMC_Parse((unsigned char *)p,&GNRMC);
//	UTCToChina(GNRMC.year,GNRMC.month,GNRMC.day,GNRMC.hour,GNRMC.Minute,GNRMC.second);
//	printf("时间:%d-%d-%d %d:%d:%d\n",Ch_TM.year,Ch_TM.month,Ch_TM.day,Ch_TM.hour,Ch_TM.Minute,Ch_TM.second);
//	printf("%u %c ---- %u %c 定位是否有效果:%d\n",GNRMC.longitude,GNRMC.longitude_suffix,GNRMC.latitude,GNRMC.latitude_suffix,GNRMC.position_valid);

//	getchar();
//	return 0;
//}

