//=============================================
//         时间日期处理 javascript 脚本
//                陈逸少
//              2002.10.05
//         2010年8月重新修改
//=============================================

// 农历数据信息类
function lunarDate()
{
	//农历数据类
	this.year  = 0;	//农历年，如 2000
	this.month = 0;	//农历月，如 12, 1 ~ 12
	this.day   = 0;	//农历日，如 13, 1 ~ 30
	this.leapMonth; // 是否是闰月， 1 为农历闰月, 0 为非闰月
	// 返回字符串信息
	this.szYearGanZhi = "";		// 农历年的干支字符串, 如"辛未"
	this.szYearShengXiao = "";	// 生肖字符串, 如 "羊"
	this.szLunarMonth = "";		// 农历月的字符串, 如"闰四"
	this.szLunarDay   = "";		// 农历日期字符串, 如"初五"
};

//===============================================================
//农历计算函数, 从公历 yy-mm-dd计算农历
//  yy: 年 >1990
// mm: 月(1-12)
// dd: 日(1-31)
//===============================================================
function getLunar(yy, mm, dd)
{
	//===============================================================
	//      下面是不变的常量数据
	//===============================================================
	//每天的秒数
	var spd = 86400;

	//---------------------------------
	//  农历日期名称
	var Day_n="初一;初二;初三;初四;初五;初六;初七;初八;初九;初十;十一;十二;十三;十四;十五;十六;十七;十八;十九;二十;廿一;廿二;廿三;廿四;廿五;廿六;廿七;廿八;廿九;三十";

	//  农历月份名称，正常年份的月份名称
	var Month_n="正月;二月;三月;四月;五月;六月;七月;八月;九月;十月;冬月;腊月";
	//--------------------------------
	//  星期名字
	var WeekNames = new Array(7);
	WeekNames[0] = "星期日";
	WeekNames[1] = "星期一";
	WeekNames[2] = "星期二";
	WeekNames[3] = "星期三";
	WeekNames[4] = "星期四";
	WeekNames[5] = "星期五";
	WeekNames[6] = "星期六";
	///////////////////////////////////////////////////////////////////////
	//==========================================================================
	// 农历数据结构：
	//	startsecond  - 正月初一起始时间（秒数）
	//	yearname     -  农历年的名称
	//	dayofmonth - 农历年每月的天数
	//	leapmont      - 农历年的闰月, 1~12, 0-不是闰年
	//     例如: 2002年对应的数值是:(1013443200, "壬午年", "30;30;29;30;29;30;29;29;30;29;30;29", 0)
	//===========================================================================
	function TLunarData(startsecond, yearname, daysinmonth, leapmonth)
	{
		this.startsecond = startsecond;
		this.yearname    = yearname;
		this.daysinmonth  = daysinmonth;
		this.leapmonth   = leapmonth;
	};
	////////////////////////////////////////////////////////////
	//-----------------------------------------------------------------------------------
	//         下面是变化的常量数据
	//         有效期过后要更新数据
	//    只需要更新此处数据即可, 可以使用getlunar小工具
	//----------------------------------------------------------------------------------
	// 数组长度,  数据中的年份数量
	//  修正到：2009～2014
	var arrLen= 6;	
	// 计算的起始年份
	var startyear = 2009;	
	// 计算需要用的农历数据库
	var lunarData = new Array(arrLen);
	lunarData[0] = new TLunarData(1232899200, "己丑", "30;30;29;29;30;29;29;30;29;30;29;30;30", 5);	// 2009年 ,闰5月
	lunarData[1] = new TLunarData(1266076800, "庚寅", "30;29;30;29;30;29;29;30;29;30;29;30", 0);	// 2010年
	lunarData[2] = new TLunarData(1296662400, "辛卯", "30;29;30;30;29;30;29;29;30;29;30;29", 0);	// 2011年
	lunarData[3] = new TLunarData(1327248000, "壬辰", "30;29;30;30;29;30;29;30;29;30;29;30;29", 4);	// 2012年 ,闰4月
	lunarData[4] = new TLunarData(1360425600, "癸巳", "30;29;30;29;30;30;29;30;29;30;29;30", 0);	// 2013年
	lunarData[5] = new TLunarData(1391097600, "甲午", "29;30;29;30;29;30;29;30;30;29;30;29;30", 9);	// 2014年 ,闰9月
	/////////////////////////////////////////////////////////////
	//===========================================================================
	//    下面是计算农历代码
	//===========================================================================
	//----------------------------------------------------
	var D = new Date(yy, mm, dd);
	var ss = parseInt(D.getTime() / 1000);	// 当前时间:秒计数数
	//----------------------------------------------------
	//  农历年月日的下标
	var iyear  = 0;		//农历年名称的下标: 从 0 开始
	var imonth = 0;		//农历月的下标: 从 0 开始
	var iday   = 0;		//农历日的下标: 从 0 开始
	//--------------------------------------------------------
	// 计算年的下标: startYear年 为 0
	var sValue = 0; //当前距当年正月初一 00:00:00 的秒数
	for (i=0; i<arrLen; i++)
	{
		if (ss >= lunarData[i].startsecond)
		{
			iyear = i;
			sValue = ss - lunarData[i].startsecond;    //当年的秒数
		}
	}
	//--------------------------------------------------------
	// 计算月:
	//计算为当年的第几天
	var dayiy = parseInt(sValue / spd);
	//  计算当月的第几天
	var dayim = dayiy;	//当月的第几天

	//--------------------------------------
	//农历一年中每月的天数
	var dpm = "";
	if(iyear<6)
	{
		dpm = lunarData[iyear].daysinmonth;
	}
	else
	{
		dpm = lunarData[0].daysinmonth;
	}
	dpm = dpm.split(";");	//分割成数组: 每月的天数
	//------------------------------------------------------------
	// 计算农历月份的下标
	// 循环计算
	for(i=0; i<dpm.length; i++)
	{
		var t_dim = parseInt(dpm[i]);
		if(dayim >= t_dim)
		{
			dayim = dayim - t_dim ;	//减去这一月的天数
			imonth++ ;			//增加月份
		}
	}//(月份下标为 0-11)
	iday = dayim;	//剩余的天数,为当月的第(iday+1)日
	//-----------------------------------------------------------------------
	//创建返回用的农历对象
	//-----------------------------------------------------------------------
	lunard = new lunarDate();
	lunard.year  = iyear + startyear;
	lunard.month = imonth + 1;
	lunard.day   = iday +1;
	//设置字符串
	//--------------------------------------------
	//  计算农历日期字符串
	lunard.szYearGanZhi = lunarData[iyear].yearname;
	//lunard.szYearGanZhi = lunarData[iyear].yearname;
	var Dname = Day_n.split(";");	//日期名称: 分割成数组
	lunard.szLunarDay = Dname[iday];	//农历日期
	//--------------------------------------------
	//   计算月份字符串
	var Mname = Month_n.split(";"); //月分名称数组
	lunard.szLunarMonth = Mname[imonth];

	// 有闰月, 插入闰月的月份
	if(lunarData[iyear].leapmonth != 0)
	{
		if(imonth == lunarData[iyear].leapmonth)
		{
			lunard.szLunarMonth = "闰" + Mname[imonth - 1]; //闰月
			lunard.month -= 1;
			lunard.leapMonth = 1;	//当月是闰月
		}
		else if(imonth > lunarData[iyear].leapmonth)
		{
			lunard.szLunarMonth = Mname[imonth - 1];
			lunard.month -= 1;
		}
	}
	return lunard;
}

//---------------------------
//  显示时间日期(日历格式)
//---------------------------
function show_time()
{
	//  星期名字
	var WeekNames = new Array(7);
	WeekNames[0] = "星期日";
	WeekNames[1] = "星期一";
	WeekNames[2] = "星期二";
	WeekNames[3] = "星期三";
	WeekNames[4] = "星期四";
	WeekNames[5] = "星期五";
	WeekNames[6] = "星期六";
	//  获取公历信息
	var D = new Date();
	var yy = D.getYear();	/* 年 */
	if (yy < 1900)
	{	//针对省略了19xx的年份
		yy = 1900 + yy;	//年
	}
					
	var mm = D.getMonth() + 1;	/* 月: 0 - 11 , +1 */

	var dd = D.getDate();		/* 日: 1 - 31 */

	var ww = D.getDay();		/* 星期: 0 - 6 */

	var wn = WeekNames[ww];		// 获取星期名字
	if((ww==0)||(ww==6))
	{
		// 周末是红色的
		wn = "<font color=red>" + wn + "</font>";
		dd = "<font color=red>" + dd + "</font>";
	}

	//  获取农历信息
	var YY = D.getYear();
	var MM = D.getMonth();
	var DD = D.getDate();
	var lunar = getLunar(yy, MM, DD);
	// -------------下面定义日期字符串-------------------------
	//  公历:
	var date_line1 = yy + "年" + mm + "月";	// 第一行: 显示年月
	var date_line2 = dd;					// 第二行: 显示天, 大字符
	var date_line3 = wn;					// 第三行: 星期
	// 农历:
	var date_line4 = "农历" + lunar.szYearGanZhi + "年";	// 第四行: 农历年
	var date_line5 = lunar.szLunarMonth + lunar.szLunarDay;				// 第五行: 农历月日
	// 输出
	document.write("<table style='font-size: 9pt; padding-top:6px;' cellspacing='0' cellpadding='0' width='90' height='110'>");
	document.write("<tr><td align='center';padding-top:4px'>");
	document.write("<font style='font-family: Verdana;padding:2px;'>" + date_line1 + "</font><br />"); 	//第1行
	document.write("<span style='font-family: Arial Black;font-size:18pt;'>" + date_line2 + "</span><br />"); 	//第2行
	document.write("<span style='font-size:9pt;'>" + date_line3 + "</span><br />");	//第3行
	document.write("<hr style='padding:0;border:none;border-top:1px dashed #cde1f5;height:1px;windth:80%;'/>");
	document.write("" + date_line4 + "<br />");			//第4行
	document.write("" + date_line5 + "</td></tr></table>");	//第5行
}      

function get_time_text()
{
	//  星期名字
	var WeekNames = new Array(7);
	WeekNames[0] = "星期日";
	WeekNames[1] = "星期一";
	WeekNames[2] = "星期二";
	WeekNames[3] = "星期三";
	WeekNames[4] = "星期四";
	WeekNames[5] = "星期五";
	WeekNames[6] = "星期六";
	//  获取公历信息
	var D = new Date();
	var yy = D.getYear();	/* 年 */
	if (yy < 1900)
	{	//针对省略了19xx的年份
		yy = 1900 + yy;	//年
	}
					
	var mm = D.getMonth() + 1;	/* 月: 0 - 11 , +1 */

	var dd = D.getDate();		/* 日: 1 - 31 */

	var ww = D.getDay();		/* 星期: 0 - 6 */

	var wn = WeekNames[ww];		// 获取星期名字
	if((ww==0)||(ww==6))
	{
		// 周末是红色的
		wn = "<font color=red>" + wn + "</font>";
		dd = "<font color=red>" + dd + "</font>";
	}

	//  获取农历信息
	var YY = D.getYear();
	var MM = D.getMonth();
	var DD = D.getDate();
	var lunar = getLunar(yy, MM, DD);
	// -------------下面定义日期字符串-------------------------
	//  公历:
	var date_line1 = yy + "年" + mm + "月" + dd + "日";	// 第一行: 公历
	var date_line2 = "农历" + lunar.szYearGanZhi + "年" + lunar.szLunarMonth + lunar.szLunarDay;// 第二行: 农历
	var date_line3 = wn;					// 第三行: 星期
	// 返回字符串
	return (date_line1 + " " + date_line2 + " " + date_line3);
} 
