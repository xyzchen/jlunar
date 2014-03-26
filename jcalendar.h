#ifndef __JCalendar_H__
#define __JCalendar_H__
//==============================================
//  农历日历类，一直自之前本人写的的windows版本
//     作者: 逸少 
//    jmchxy@163.com
//      2003-6
//     修改: 2012-3
//===============================================
#include <gdk/gdk.h>
#include <cairo.h>
#include <gtk/gtk.h>
#include "jlunar.h"

//------------------------------------------------------
// 从windows移植程序的定义
//------------------------------------------------------
//点
typedef struct tagPOINT
{
	int x;
	int y;
}POINT;

//矩形
typedef struct tagRECT
{
	int left;
	int top;
	int right;
	int bottom;
}RECT;

//测试点是否在矩形中
bool PtInRect(const RECT& rect, const POINT& pt);

//RGB颜色
class RgbColor
{
public:
	RgbColor(unsigned int rgb = 0)
	{
		m_rgb = rgb;
	}
	
	RgbColor(int r, int g, int b)
	{
		m_rgb = (r<<16)|(g<<8)|b;
	}
	
	int red()const
	{
		return (m_rgb & 0xff0000)>>16;
	}
	
	int green()const
	{
		return (m_rgb & 0xff00)>>8;
	}
	
	int blue()const
	{
		return (m_rgb & 0xff);
	}
	
	unsigned int rgb()const
	{
		return m_rgb;
	}
private:
	unsigned int m_rgb;
};
//---------------------------------------------
//   包含农历信息的日历类
//---------------------------------------------
class JCalendar
{
public:
	//构造函数
	JCalendar(void);
	//析购函数
	~JCalendar(void);

	//公共方法
public:
	//--------------------------------------
	//设置日历网格的坐标范围(标题, 星期头和日期网格)
	void SetRect(const RECT&  CalendarRect);
	//--------------------------------------
	// 设置选择的日期, 第几行第几列
	void SetSel(int row, int col);
	// 转换客户坐标到行列
	//  如果坐标不在日历区域返回false (-1, -1)
	bool PointToGrid(const POINT& pt, int* row, int* col)const;
	// 获取当前选择的日期
	void GetSel(LUNARDATE* pLunarDate)const;
	// 获取今天的日期
	void GetNow(LUNARDATE* pLunarDate)const;
	// 选择下一个月
	void SelectNextMonth();
	// 选择前一个月
	void SelectPrevMonth();
	// 选择前一年
	void SelectPrevYear();
	// 选择后一年
	void SelectNextYear();
	// 选择指定的年月日
	void Select(int year, int month, int day = 1);
	// 重设日历到当前月
	void Reset();
	//---------------------------------------
	// 绘制日历
	//---------------------------------------
	void Draw(cairo_t* cr);
	//==========================================
	// 获取各部分坐标
	//-------------------------------------
	// 获取标题范围
	void GetTitleRect(RECT* pTitleRect)const;
	// 获取月历表格的范围(不包括标题)
	void GetGridRect(RECT*  pGridRect)const;
	// 获取全部范围坐标
	void GetRect(RECT* pCalendarRect)const;
private:
	// 计算需要的行数, 包括星期行
	int  CalcRows()const;
	void LoadSettings();	// 初始化, 从注册表读取设置信息
	void SaveSettings();	// 退出, 保存信息到注册表
//-----------------------------
// 私有的数据
//-----------------------------
private:
	LUNARDATE  m_lunarSel;	// 当前鼠标选择的日期
	LUNARDATE  m_lunarNow;	// 今天的日期
	RECT	   m_rectTitle;	// 标题区域
	RECT       m_rectGrid;	// 网格区域
	RECT	   m_rectAll;		// 整个日历的区域
	int        m_iGridWidth;	//网格宽度
	int		   m_iGridHeight;	//网格的高度
	//  绘图属性
	int			m_iLargeFontSize;		//大字体的点数
	int			m_iSmallFontSize;		//小字体的点数
	RgbColor	m_rgbNormal;			//正常日期的颜色
	RgbColor	m_rgbHoliday;			//节假日的颜色
	RgbColor	m_rgbGrid;				//网格线的颜色
};

#endif //__JCalendar_H__
