#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>
#include<ctype.h>

#include<string>

#include "jlunar.h"

const int sec_per_day   = 24 * 60 * 60;	//每天的秒数
const int local_rectify = 8 * 60 * 60;	//北京时间校正的秒数

int main(int argc, char** argv)
{
	if(argc < 3)
	{
		printf("\n----------------------------------------------------------------\n");
		printf("            为javascript构造农历数据的小工具\n");
		printf("               作者: 陈子逸(jmchxy@163.com)\n");
		printf(" 用法:\n");
		printf("    getlunar  startyear  years\n");
		printf("               [startyear] : 开始的年份\n");
		printf("               [years]:      要构造的年数\n");
		printf("-----------------------------------------------------------------\n");
		return 0;
	}

	int start_year  = atoi(argv[1]);
	int year_count  = atoi(argv[2]);

	printf("// 数组长度,  数据中的年份数量\n");
	printf("var arrLen = %d;\n", year_count);

	printf("// 计算的起始年份\n");
	printf("var startyear = %d;\n", start_year);

	printf("// 计算需要用的农历数据库\n");
	printf("var lunarData = new Array(arrLen);\n");

	// 1970年1月1日的日期数
	int days70 = cjxGetOffsetSolarDays(1970, 1, 1);
	for(int i = 0; i < year_count; i++)
	{
		int year = start_year + i;
		//========================================
		//计算春节是哪一天
		int month = cjxSpringMonth(year);
		int day   = cjxSpringDay(year);
		
		// 春节日从1970年1月1日0时0分0秒起计算的秒数（按UTC时间计算）
		//   即农历年起始的秒数（从1970年1月1日0时0分0秒起计算的秒数）
		unsigned int sec_t =  (cjxGetOffsetSolarDays(year, month, day) - days70) * sec_per_day - local_rectify;
		
		// 农历年名称:
		LUNARDATE  ld;
		memset(&ld, 0, sizeof(ld));
		ld.wYear  = year;
		ld.wMonth = 5;
		ld.wDay   = 1;
		cjxGetLunarDate(&ld);
		
		// 农历年每月的天数
		int mons = 12;	//月数
		int leap = cjxLunarLeapMonth(year);	//闰月， 没有为0
		if(leap != 0)	//有闰月	
		{
			mons = 13;
		}
		std::string strDaysInMonth;
		// 计算各月的天数
		for(int month = 1; month <= mons; month++)
		{
			char buffer[20];
			sprintf(buffer, "%d;", 29 + cjxLunarMonthBig(year, month));
			strDaysInMonth += buffer;
		}
		strDaysInMonth = strDaysInMonth.substr(0, strDaysInMonth.length()-1);
		//========================================
		//  输出:
		//-----------------------------------------
		printf("lunarData[%d] = new TLunarData(%d, \"%s\", \"%s\", %d);",
			i, sec_t, ld.szYearGanZhi, strDaysInMonth.c_str(), leap);

		if( leap != 0)
		{
			printf("\t// %d年 ,闰%d月\n", year, leap);
		}
		else
		{
			printf("\t// %d年\n", year);
		}
	}
	return 0;
}

