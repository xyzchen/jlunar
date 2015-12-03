#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <getopt.h>

#include "jlunar.h"

//----------------------------------------------------
//获取当天的日期
//----------------------------------------------------
static void getToday(LUNARDATE& today)
{
	time_t rawtime  = time(NULL);
	struct tm * ptm = gmtime ( &rawtime );

	today.wYear  = ptm->tm_year + 1900;
	today.wMonth = ptm->tm_mon  + 1;
	today.wDay   = ptm->tm_mday;
	cjxGetLunarDate(&today);
}

//----------------------------------------------------
//输出日期字符串, 
//  style:  0 - 短格式（只有农历），
//          1 - 长格式（公历 农历 星期），
//          2 - :分割的字符串（公历：农历：星期）
//----------------------------------------------------
static void printDateString(const LUNARDATE& lunardate, int style=0)
{
	if(style==0)
	{
		printf("农历%s(%s)年%s月%s\n", 
			lunardate.szYearGanZhi, lunardate.szYearShengXiao,
			lunardate.szLunarMonth, lunardate.szLunarDay);
	}
	else if(style == 1)
	{
		printf("%d年%d月%d日[农历%s(%s)年%s月%s] 星期%s\n", 
			lunardate.wYear, lunardate.wMonth, lunardate.wDay,
			lunardate.szYearGanZhi, lunardate.szYearShengXiao,
			lunardate.szLunarMonth, lunardate.szLunarDay, cjxGetWeekName(lunardate.wWeekDay));
	}
	else	//分隔的字段
	{
		printf("%d年%d月%d日:农历%s(%s)年%s月%s:星期%s\n", 
			lunardate.wYear, lunardate.wMonth, lunardate.wDay,
			lunardate.szYearGanZhi, lunardate.szYearShengXiao,
			lunardate.szLunarMonth, lunardate.szLunarDay, cjxGetWeekName(lunardate.wWeekDay));
	}
}

//test: s - 短格式， l - 长格式， f - 分隔的字段
int main(int argc, char *argv[])
{
	int style  = 1;
	//解析命令行
	int result;
	while((result = getopt(argc, argv, "slf")) != -1)
	{
		switch(result)
		{
			case 's':	//图形样式
				style = 0;
				break;
			
			case 'l':	//文字模式
				style = 1;
				break;
				
			case 'f':	//最大值
				style = 2;
				break;
				
			default:
				break;
		}
	}
	//获取当天的信息
	LUNARDATE today;
	memset(&today, 0, sizeof(today));
	getToday(today);
	
	//输出信息
	printDateString(today, style);
	
	return 0;
}
