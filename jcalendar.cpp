#include <time.h>
#include <string.h>
#include <stdlib.h>
#include "jcalendar.h"
//==============================================
//  农历日历类
//     作者: 逸少 
//    jmchxy@163.com
//      2003-6
//   修改: 2010-3
//===============================================

//---------------------------------------
//   X行7 列 的网格
//---------------------------------------
const int GRIDCOLS = 7;

// 月历表格头
const char* g_szWeek[] = { _T("日"), _T("一"), _T("二"), _T("三") , _T("四"), _T("五"), _T("六") };

// 测试点是否在矩形中
bool PtInRect(const RECT& rect, const POINT& pt)
{
	if((pt.x <= rect.right)&&(pt.x >= rect.left)
		&&(pt.y <= rect.bottom)&&(pt.y >= rect.top))
	{
		return true;
	}
	return false;
}
//-----------------------------------------
//   构造函数, 初始化
//-----------------------------------------
JCalendar::JCalendar(void)
{
	// 获取当前系统日期
	time_t rawtime  = time(NULL);
	struct tm * ptm = gmtime ( &rawtime );

	// 获取当前日期的农历日期
	memset(&m_lunarNow, 0, sizeof(m_lunarNow));
	m_lunarNow.wYear  = ptm->tm_year + 1900;
	m_lunarNow.wMonth = ptm->tm_mon  + 1;
	m_lunarNow.wDay   = ptm->tm_mday;
	cjxGetLunarDate(&m_lunarNow);
	// 当前选择的日期
	m_lunarSel = m_lunarNow;
	// 位置大小
	memset(&m_rectGrid, 0, sizeof(m_rectGrid));
	memset(&m_rectTitle, 0, sizeof(m_rectTitle));
	memset(&m_rectAll, 0, sizeof(m_rectAll));
	m_iGridWidth  = 0;
	m_iGridHeight = 0;
	// 绘图对象
	m_iLargeFontSize = 32;	//大字体的点数
	m_iSmallFontSize = m_iLargeFontSize / 2;	//小字体的点数
	m_rgbNormal	 = RgbColor(0, 192, 0);		//正常日期的颜色
	m_rgbHoliday = RgbColor(255, 0, 0);		//节假日的颜色
	m_rgbGrid	 = RgbColor(192, 192, 192);	//网格线的颜色
}

//-------------------------------------------
// 析购函数
//-------------------------------------------
JCalendar::~JCalendar(void)
{
	//销毁刷子
}

//------------------------------------------------------------------
// 设置日历网格的坐标范围(标题, 星期头和日期网格)
//------------------------------------------------------------------
void JCalendar::SetRect(const RECT&  CalendarRect)
{
	// 标题高度
	int titleHeight = (CalendarRect.bottom - CalendarRect.top) / 7;
	// 格子宽度
	m_iGridWidth  = (CalendarRect.right - CalendarRect.left ) / GRIDCOLS;
	// 格子行数
	int GRIDROWS = CalcRows();
	// 格子高度
	m_iGridHeight = ( CalendarRect.bottom - CalendarRect.top  - titleHeight) / GRIDROWS;

	// 整个区域
	m_rectAll = CalendarRect;

	// 标题矩形位置
	m_rectTitle.left  = (( CalendarRect.right - CalendarRect.left )  - m_iGridWidth * GRIDCOLS ) / 2 + CalendarRect.left;
	m_rectTitle.right = m_rectTitle.left + m_iGridWidth * GRIDCOLS;
	m_rectTitle.top   = CalendarRect.top;
	m_rectTitle.bottom = m_rectTitle.top + titleHeight;

	// 网格矩形
	m_rectGrid.left  = m_rectTitle.left;
	m_rectGrid.right = m_rectTitle.right;
	m_rectGrid.top   = m_rectTitle.bottom + 1;
	m_rectGrid.bottom = m_rectGrid.top + m_iGridHeight * GRIDROWS;
}

//-------------------------------------------
// 设置选择的日期, 第几行第几列
//-------------------------------------------
void JCalendar::SetSel(int row, int col)
{
	//  当月 1 号是星期几
	int weekStart = cjxGetWeekday(m_lunarSel.wYear, m_lunarSel.wMonth, 1);
	//  第几行几列在当月的索引(与1号的差)
	int index = row * GRIDCOLS + col - weekStart;
	//  计算 当月1 号 到初始点的天数
	int days = cjxGetOffsetSolarDays(m_lunarSel.wYear, m_lunarSel.wMonth, 1) + index;
	//  计算时间
	LUNARDATE  lunardate;
	if(cjxGetDate(days, & lunardate))
	{
		// 更新当前选择
		m_lunarSel = lunardate;
	}
}

// 获取当前选择的日期
void JCalendar::GetSel(LUNARDATE* pLunarDate)const
{
	if(pLunarDate)
	{
		*pLunarDate = m_lunarSel;
	}
}
// 获取今天的日期
void JCalendar::GetNow(LUNARDATE* pLunarDate)const
{
	if(pLunarDate)
	{
		*pLunarDate = m_lunarNow;
	}
}

//选择到下一个月
void JCalendar::SelectNextMonth()
{
	m_lunarSel.wDay  = 1;
	m_lunarSel.wMonth += 1;
	if(m_lunarSel.wMonth > 12)
	{
		m_lunarSel.wMonth = 1;
		m_lunarSel.wYear += 1;
		if(m_lunarSel.wYear > 2100)
		{
			m_lunarSel.wYear = 2100;
		}
	}
	cjxGetLunarDate(&m_lunarSel);
}

//选择到前一个月
void JCalendar::SelectPrevMonth()
{
	m_lunarSel.wDay  = 1;
	m_lunarSel.wMonth -= 1;
	if(m_lunarSel.wMonth < 1)
	{
		m_lunarSel.wMonth = 12;
		m_lunarSel.wYear -= 1;
		if(m_lunarSel.wYear < 1900)
		{
			m_lunarSel.wYear = 1900;
		}
	}
	cjxGetLunarDate(&m_lunarSel);
}

//选择到下一年
void JCalendar::SelectNextYear()
{
	m_lunarSel.wYear += 1;
	if(m_lunarSel.wYear > 2100)
	{
		m_lunarSel.wYear = 2100;
	}
	if(m_lunarSel.wDay > cjxGetSolarMonthDays(m_lunarSel.wYear, m_lunarSel.wMonth))
	{
		m_lunarSel.wDay = cjxGetSolarMonthDays(m_lunarSel.wYear, m_lunarSel.wMonth);
	}
	cjxGetLunarDate(&m_lunarSel);
}

//选择到上一年
void JCalendar::SelectPrevYear()
{
	m_lunarSel.wYear -= 1;
	if(m_lunarSel.wYear < 1900)
	{
		m_lunarSel.wYear = 1900;
	}
	if(m_lunarSel.wDay > cjxGetSolarMonthDays(m_lunarSel.wYear, m_lunarSel.wMonth))
	{
		m_lunarSel.wDay = cjxGetSolarMonthDays(m_lunarSel.wYear, m_lunarSel.wMonth);
	}
	cjxGetLunarDate(&m_lunarSel);
}

// 选择指定的年月日
void JCalendar::Select(int year, int month, int day)
{
	m_lunarSel.wYear  = year;
	m_lunarSel.wMonth = month;
	m_lunarSel.wDay   = day;
	cjxGetLunarDate(&m_lunarSel);
}

//重置到当前系统时间
void JCalendar::Reset()
{
	// 获取当前系统日期
	time_t rawtime  = time(NULL);
	struct tm * ptm = gmtime ( &rawtime );

	// 获取当前日期的农历日期
	memset(&m_lunarNow, 0, sizeof(m_lunarNow));
	m_lunarNow.wYear  = ptm->tm_year + 1900;
	m_lunarNow.wMonth = ptm->tm_mon  + 1;
	m_lunarNow.wDay   = ptm->tm_mday;
	cjxGetLunarDate(&m_lunarNow);
	
	// 当前选择的日期
	m_lunarSel = m_lunarNow;
}

/////////////////////////////////////////////
//  private
/////////////////////////////////////////////
// 计算需要的行数, 包括星期行
int JCalendar::CalcRows()const
{
	//  当月 1 号是星期几
	int weekStart = cjxGetWeekday(m_lunarSel.wYear, m_lunarSel.wMonth, 1);
	//  计算当前月需要的格子数
	int days = cjxGetSolarMonthDays(m_lunarSel.wYear, m_lunarSel.wMonth) + weekStart;
	//  计算行数
	return ((days + 6) / 7 + 1);
}

//-------------------------------------------
// 获取日历坐标范围
//-------------------------------------------
void JCalendar::GetRect(RECT*  pCalendarRect)const
{
	if(pCalendarRect)
	{
		*pCalendarRect = m_rectAll;
	}
}

//获取标题坐标
void JCalendar::GetTitleRect(RECT*  pTitleRect)const
{
	if(pTitleRect)
	{
		*pTitleRect = m_rectTitle;
	}
}

//获取网格坐标
void JCalendar::GetGridRect(RECT*  pGridRect)const
{
	if(pGridRect)
	{
		*pGridRect = m_rectGrid;
	}
}

// 转换客户坐标到行列
bool JCalendar::PointToGrid(const POINT& pt, int* row, int* col)const
{
	if((row == NULL) ||(col == NULL))
	{
		return false;
	}

	RECT  dataRect = m_rectGrid;
	dataRect.top  += m_iGridHeight;
	if( PtInRect(dataRect, pt) != TRUE )
	{
		*row = -1;
		*col = -1;
		return false;
	}
	*row = (pt.y - dataRect.top) / m_iGridHeight;
	*col = (pt.x - dataRect.left) / m_iGridWidth;
	return true;
}

//===============================================================
// 绘制日历
//===============================================================
void JCalendar::Draw(cairo_t* cr)
{
	// 重新计算格子高度
	int GRIDROWS = CalcRows();	//格子行数
	// 格子高度
	m_iGridHeight   = ( m_rectAll.bottom - m_rectTitle.bottom ) / GRIDROWS;
	m_rectGrid.top  = m_rectTitle.bottom + 1;
	m_rectGrid.bottom = m_rectGrid.top + m_iGridHeight * GRIDROWS;
	// 字体大小
	m_iLargeFontSize = (m_iGridHeight - 8) *2 / 3;	//大字体的点数
	m_iSmallFontSize = m_iLargeFontSize / 2;		//小字体的点数
	
	//设置颜色和线宽
	cairo_set_source_rgb(cr, m_rgbGrid.red()/255.0, m_rgbGrid.green()/255.0, m_rgbGrid.blue()/255.0);
	cairo_set_line_width(cr, 0.5);
	// 绘制网格线
	double  y = m_rectGrid.top;
	for(int i = 0; i <= GRIDROWS; i++)
	{
		cairo_move_to(cr, m_rectGrid.left, y-2);  //改变绘制起始点
		cairo_line_to(cr, m_rectGrid.right, y-2);
		cairo_stroke(cr);
		y += m_iGridHeight;
	}
	double  x = m_rectGrid.left;
	for(int i = 0; i <= GRIDCOLS; i++)
	{
		cairo_move_to(cr, x,  m_rectGrid.top);  //改变绘制起始点
		cairo_line_to(cr, x,  m_rectGrid.bottom);
		cairo_stroke(cr);
		x += m_iGridWidth;
	}
	//绘制文字
	//  当前选择的 当月 1 号是星期几
	int weekStart = cjxGetWeekday(m_lunarSel.wYear, m_lunarSel.wMonth, 1);
	// 需要写多少排
	int nlines = (cjxGetSolarMonthDays(m_lunarSel.wYear, m_lunarSel.wMonth) + weekStart) / 7;
	// 最后一行的个数
	int weekEnd = (cjxGetSolarMonthDays(m_lunarSel.wYear, m_lunarSel.wMonth) + weekStart) % 7;

	//计算选择的格子是第几行第几列
	//---------------------------------------------------------------
	int  row = ( m_lunarSel.wDay + weekStart - 1) / 7 + 1;
	int  col = m_lunarSel.wWeekDay;

	// 计算选择的日期的方框的坐标
	int	selleft = m_rectGrid.left + col * m_iGridWidth + 1;
	int	seltop  = m_rectGrid.top  + row * m_iGridHeight + 1;
	int selwidth = m_iGridWidth - 3;
	int selheight= m_iGridHeight - 3;
	// 在选择的日期的格子绘制一个矩形背景
	cairo_set_source_rgb(cr, m_rgbHoliday.red()/255.0, m_rgbHoliday.green()/255.0, m_rgbHoliday.blue()/255.0);
	cairo_rectangle (cr, selleft, seltop, selwidth, selheight);  //绘制长方形
	cairo_stroke(cr);	//矩形框
	
	//如果选择的当月, 则绘制"今天"的背景
	if( (m_lunarNow.wYear == m_lunarSel.wYear) && (m_lunarNow.wMonth == m_lunarSel.wMonth) )
	{
		//-------------------------------------------------
		// 计算今天是在第几行第几列
		//-------------------------------------------------
		row = ( m_lunarNow.wDay + weekStart - 1) / 7 + 1;
		col = m_lunarNow.wWeekDay;

		selleft = m_rectGrid.left + col * m_iGridWidth + 1;
		seltop  = m_rectGrid.top  + row * m_iGridHeight + 1;
		selwidth = m_iGridWidth - 3;
		selheight= m_iGridHeight - 3;

		// 在选择的日期的格子绘制一个矩形背景
		cairo_set_source_rgba(cr, 0.3, 0.3, 0.3, 0.5);
		cairo_rectangle (cr, selleft, seltop, selwidth, selheight);  //绘制长方形
		cairo_fill(cr);	//矩形框
	}
	//------------------------------------------------------
	// 绘制文本
	//------------------------------------------------------
	// 设置字体名称
	//cairo_select_font_face(cr, "Arial", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
	// 设置字体大小
	cairo_set_font_size(cr, m_iLargeFontSize);
	cairo_set_source_rgb(cr, m_rgbNormal.red()/255.0, m_rgbNormal.green()/255.0, m_rgbNormal.blue()/255.0);
	
	cairo_text_extents_t extents;
	char buffer[40];
	sprintf(buffer, "%d年%d月", m_lunarSel.wYear, m_lunarSel.wMonth);
	cairo_text_extents(cr, buffer, &extents);
	cairo_move_to(cr, m_rectTitle.left + (m_rectTitle.right - extents.width)/2, m_rectTitle.bottom - (m_rectTitle.bottom - extents.height)/2);
	cairo_show_text(cr, buffer);
	
	//输出星期标题
	int tx = 0;
	int ty = m_rectGrid.top + m_iGridHeight - (m_iGridHeight - m_iLargeFontSize) /2;
	for(int i=0; i<7;i++)
	{
		cairo_text_extents(cr, g_szWeek[i], &extents);
		tx = m_rectGrid.left + i * m_iGridWidth + (m_iGridWidth - extents.width)/2;
		if(i==0 || i==6)
		{
			cairo_set_source_rgb(cr, m_rgbHoliday.red()/255.0, m_rgbHoliday.green()/255.0, m_rgbHoliday.blue()/255.0);
		}
		else
		{
			cairo_set_source_rgb(cr, m_rgbNormal.red()/255.0, m_rgbNormal.green()/255.0, m_rgbNormal.blue()/255.0);
		}
		cairo_move_to(cr, tx, ty);
		cairo_show_text(cr, g_szWeek[i]);
	}
	//绘制日历日期: 公历文本
	int wDay = 1;
	m_rectGrid.top += m_iGridHeight;
	//   先写第一行
	for(int i = weekStart; i < 7; i++)
	{
		sprintf(buffer, "%d", wDay);
		cairo_text_extents(cr, buffer, &extents);
		
		tx  = m_rectGrid.left + m_iGridWidth * i + (m_iGridWidth - extents.width)/2;
		ty  = m_rectGrid.top + m_iGridHeight - m_iSmallFontSize - (m_iGridHeight - m_iLargeFontSize) /2;

		if(i==0 || i==6)
		{
			cairo_set_source_rgb(cr, m_rgbHoliday.red()/255.0, m_rgbHoliday.green()/255.0, m_rgbHoliday.blue()/255.0);
		}
		else
		{
			cairo_set_source_rgb(cr, m_rgbNormal.red()/255.0, m_rgbNormal.green()/255.0, m_rgbNormal.blue()/255.0);
		}
		cairo_move_to(cr, tx, ty);
		cairo_show_text(cr, buffer);
		wDay ++;
	}
	// 写中间的行
	for(int j = 1; j < nlines; j++)
	{
		for(int i=0; i<7; i++)
		{
			sprintf(buffer, "%d", wDay);
			cairo_text_extents(cr, buffer, &extents);
		
			tx  = m_rectGrid.left + m_iGridWidth * i + (m_iGridWidth - extents.width)/2;
			ty  = m_rectGrid.top + m_iGridHeight * (j+1) - m_iSmallFontSize - (m_iGridHeight - m_iLargeFontSize) /2;

			if(i==0 || i==6)
			{
				cairo_set_source_rgb(cr, m_rgbHoliday.red()/255.0, m_rgbHoliday.green()/255.0, m_rgbHoliday.blue()/255.0);
			}
			else
			{
				cairo_set_source_rgb(cr, m_rgbNormal.red()/255.0, m_rgbNormal.green()/255.0, m_rgbNormal.blue()/255.0);
			}
			cairo_move_to(cr, tx, ty);
			cairo_show_text(cr, buffer);
			wDay ++;
		}
	}
	// 最后一行
	for(int i=0; i< weekEnd; i++)
	{
		sprintf(buffer, "%d", wDay);
		cairo_text_extents(cr, buffer, &extents);
	
		tx  = m_rectGrid.left + m_iGridWidth * i + (m_iGridWidth - extents.width)/2;
		ty  = m_rectGrid.top + m_iGridHeight * (nlines+1) - m_iSmallFontSize - (m_iGridHeight - m_iLargeFontSize) /2;

		if(i==0 || i==6)
		{
			cairo_set_source_rgb(cr, m_rgbHoliday.red()/255.0, m_rgbHoliday.green()/255.0, m_rgbHoliday.blue()/255.0);
		}
		else
		{
			cairo_set_source_rgb(cr, m_rgbNormal.red()/255.0, m_rgbNormal.green()/255.0, m_rgbNormal.blue()/255.0);
		}
		cairo_move_to(cr, tx, ty);
		cairo_show_text(cr, buffer);
		wDay ++;
	}
	// 绘制农历
	//   先写第一行
	LUNARDATE  curDate = m_lunarSel;
	curDate.wDay = 1;
	cjxGetLunarDate(&curDate);
	cairo_set_font_size(cr, m_iSmallFontSize);
	for(int i =  weekStart; i < 7; i++)
	{	
		const char *pszJieri = cjxGetLunarHolidayName(curDate.wLunarMonth, curDate.wLunarDay);
		if(pszJieri != NULL) //农历传统节日
		{
			strcpy(buffer, pszJieri); 
		}
		else if((pszJieri = cjxGetTermName(curDate.wYear, curDate.wMonth, curDate.wDay)) != NULL)
		{	//24节气
			strcpy(buffer, pszJieri); 
		}
		else if(curDate.wLunarDay == 1) //农历初一
		{
			strcpy(buffer, curDate.szLunarMonth); 
			strcat(buffer, _T("月"));
		}
		else	//普通日期
		{
			strcpy(buffer, curDate.szLunarDay);
		}

		cairo_text_extents(cr, buffer, &extents);
		tx  = m_rectGrid.left + m_iGridWidth * i + (m_iGridWidth - extents.width)/2;
		ty  = m_rectGrid.top  + m_iGridHeight - 8;
		
		if(i==0 || i==6)
		{
			cairo_set_source_rgb(cr, m_rgbHoliday.red()/255.0, m_rgbHoliday.green()/255.0, m_rgbHoliday.blue()/255.0);
		}
		else
		{
			cairo_set_source_rgb(cr, m_rgbNormal.red()/255.0, m_rgbNormal.green()/255.0, m_rgbNormal.blue()/255.0);
		}
		cairo_move_to(cr, tx, ty);
		cairo_show_text(cr, buffer);
		cjxLunarNextDay(&curDate);
	}
	// 写中间的行
	for(int j = 1; j < nlines; j++)
	{
		for(int i=0; i<7; i++)
		{
			const char *pszJieri = cjxGetLunarHolidayName(curDate.wLunarMonth, curDate.wLunarDay);
			if(pszJieri != NULL) //农历传统节日
			{
				strcpy(buffer, pszJieri); 
			}
			else if((pszJieri = cjxGetTermName(curDate.wYear, curDate.wMonth, curDate.wDay)) != NULL)
			{	//24节气
				strcpy(buffer, pszJieri); 
			}
			else if(curDate.wLunarDay == 1) //农历初一
			{
				strcpy(buffer, curDate.szLunarMonth); 
				strcat(buffer, _T("月"));
			}
			else	//普通日期
			{
				strcpy(buffer, curDate.szLunarDay);
			}

			cairo_text_extents(cr, buffer, &extents);
			tx  = m_rectGrid.left + m_iGridWidth * i + (m_iGridWidth - extents.width)/2;
			ty  = m_rectGrid.top  + m_iGridHeight * (j+1) - 8;
		
			if(i==0 || i==6)
			{
				cairo_set_source_rgb(cr, m_rgbHoliday.red()/255.0, m_rgbHoliday.green()/255.0, m_rgbHoliday.blue()/255.0);
			}
			else
			{
				cairo_set_source_rgb(cr, m_rgbNormal.red()/255.0, m_rgbNormal.green()/255.0, m_rgbNormal.blue()/255.0);
			}
			cairo_move_to(cr, tx, ty);
			cairo_show_text(cr, buffer);
			cjxLunarNextDay(&curDate);
		}
	}
	// 最后一行
	for(int i=0; i< weekEnd; i++)
	{
		const char *pszJieri = cjxGetLunarHolidayName(curDate.wLunarMonth, curDate.wLunarDay);
		if(pszJieri != NULL) //农历传统节日
		{
			strcpy(buffer, pszJieri); 
		}
		else if((pszJieri = cjxGetTermName(curDate.wYear, curDate.wMonth, curDate.wDay)) != NULL)
		{	//24节气
			strcpy(buffer, pszJieri); 
		}
		else if(curDate.wLunarDay == 1) //农历初一
		{
			strcpy(buffer, curDate.szLunarMonth); 
			strcat(buffer, _T("月"));
		}
		else	//普通日期
		{
			strcpy(buffer, curDate.szLunarDay);
		}

		cairo_text_extents(cr, buffer, &extents);
		tx  = m_rectGrid.left + m_iGridWidth * i + (m_iGridWidth - extents.width)/2;
		ty  = m_rectGrid.top  + m_iGridHeight * (nlines+1) - 8;
		
		if(i==0 || i==6)
		{
			cairo_set_source_rgb(cr, m_rgbHoliday.red()/255.0, m_rgbHoliday.green()/255.0, m_rgbHoliday.blue()/255.0);
		}
		else
		{
			cairo_set_source_rgb(cr, m_rgbNormal.red()/255.0, m_rgbNormal.green()/255.0, m_rgbNormal.blue()/255.0);
		}
		cairo_move_to(cr, tx, ty);
		cairo_show_text(cr, buffer);
		cjxLunarNextDay(&curDate);
	}
	m_rectGrid.top -= m_iGridHeight;
}

