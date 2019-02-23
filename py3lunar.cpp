//------------------------------------------------------
//
//    pylunar.cpp
//        jlunar 的 python3 扩展
//
//       作者: 逸少
//     jmchxy@gmail.com
//    创建日期: 2018.5.14
//
//-------------------------------------------------------
#include <Python.h>
#include <time.h>
#include "jlunar.h"

#ifdef _MSC_VER
#include <windows.h>
BOOL APIENTRY DllMain( HANDLE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
    return TRUE;
}
#endif

//设置错误信息
static PyObject* error_out(PyObject *m, const char* errmsg);

//把LUNARDATE对象转换到python 字典对象
static PyObject* lunar_to_dict(const LUNARDATE& lunardate)
{
	// 构造一个字典对象返回给调用者
	PyObject* pDict = PyDict_New(); // 创建一个字典对象
	assert(PyDict_Check(pDict));
	// 添加 名/值 对到字典中
	PyDict_SetItemString(pDict, "Year", Py_BuildValue("i", lunardate.wYear));
	PyDict_SetItemString(pDict, "Month", Py_BuildValue("i", lunardate.wMonth));
	PyDict_SetItemString(pDict, "Day", Py_BuildValue("i", lunardate.wDay));
	PyDict_SetItemString(pDict, "WeekDay", Py_BuildValue("i", lunardate.wWeekDay));
	PyDict_SetItemString(pDict, "LunarYear", Py_BuildValue("i", lunardate.wLunarYear));
	PyDict_SetItemString(pDict, "LunarMonth", Py_BuildValue("i", lunardate.wLunarMonth));
	PyDict_SetItemString(pDict, "LunarDay", Py_BuildValue("i", lunardate.wLunarDay));
	PyDict_SetItemString(pDict, "IsLeapMonth", Py_BuildValue("i", lunardate.wIsLeapMonth));
	PyDict_SetItemString(pDict, "LunarYearName", Py_BuildValue("s", lunardate.szYearGanZhi));
	PyDict_SetItemString(pDict, "ShengXiao", Py_BuildValue("s", lunardate.szYearShengXiao));
	PyDict_SetItemString(pDict, "LunarMonthName", Py_BuildValue("s", lunardate.szLunarMonth));
	PyDict_SetItemString(pDict, "LunarDayName", Py_BuildValue("s", lunardate.szLunarDay));
	// 返回字典对象给调用者
	return pDict;
}

//公历数据转换成农历数据
//  参数：公历年月日
static PyObject* wrap_GetLunarDate(PyObject* self, PyObject* args)
{
	int y, m, d;
	if (! PyArg_ParseTuple(args, "iii", &y, &m , &d))
	{
		PyErr_SetString(PyExc_TypeError, "无效的参数");
		return NULL;
	}
	LUNARDATE lunardate;
	memset(&lunardate, 0, sizeof(lunardate));
	lunardate.wYear  = y;
	lunardate.wMonth = m;
	lunardate.wDay   = d;
	cjxGetLunarDate(&lunardate);
	// 构造一个字典对象返回给调用者
	return lunar_to_dict(lunardate);
}

//农历数据转换成公历数据
//  参数：农历年月日，是否闰月
static PyObject* wrap_GetSolarDate(PyObject* self, PyObject* args)
{
	int y, m, d, leap=0;
	if (! PyArg_ParseTuple(args, "iii|i", &y, &m , &d, &leap))
	{
		PyErr_SetString(PyExc_TypeError, "无效的参数，参数为(year, month, day, leap)");
		return NULL;
	}
	LUNARDATE lunardate;
	memset(&lunardate, 0, sizeof(lunardate));
	//计算到公历的时间
	lunardate.wLunarYear  = (WORD)y;
	lunardate.wLunarMonth = (WORD)m;
	lunardate.wLunarDay   = (WORD)d;
	lunardate.wIsLeapMonth= (WORD)leap;
	cjxGetSolarDate(&lunardate);
	// 构造一个字典对象返回给调用者
	return lunar_to_dict(lunardate);
}

//从指定的UNIX时间戳获取日期对象
static PyObject* wrap_GetDate(PyObject* self, PyObject *args)
{
	time_t rawtime = time(NULL);
	//解析参数
	if (! PyArg_ParseTuple(args, "|l", &rawtime))
	{
		PyErr_SetString(PyExc_TypeError, "无效的参数, 接收一个参数为unix时间戳(int)");
		rawtime = NULL;
	}
	struct tm * ptm = gmtime ( &rawtime );

	LUNARDATE lunardate;
	memset(&lunardate, 0, sizeof(lunardate));
	lunardate.wYear  = ptm->tm_year + 1900;
	lunardate.wMonth = ptm->tm_mon  + 1;
	lunardate.wDay   = ptm->tm_mday;
	cjxGetLunarDate(&lunardate);
	// 构造一个字典对象返回给调用者
	return lunar_to_dict(lunardate);
}

//获取今天的日期数据
static PyObject* wrap_GetToday(PyObject* self, PyObject *args)
{
	time_t rawtime  = time(NULL);
	struct tm * ptm = gmtime ( &rawtime );

	LUNARDATE lunardate;
	memset(&lunardate, 0, sizeof(lunardate));
	lunardate.wYear  = ptm->tm_year + 1900;
	lunardate.wMonth = ptm->tm_mon  + 1;
	lunardate.wDay   = ptm->tm_mday;
	cjxGetLunarDate(&lunardate);
	// 构造一个字典对象返回给调用者
	return lunar_to_dict(lunardate);
}

//获取今天的日期数据字符串
static PyObject* wrap_GetTodayString(PyObject* self, PyObject *args)
{
	time_t rawtime  = time(NULL);
	struct tm * ptm = gmtime ( &rawtime );

	LUNARDATE lunardate;
	memset(&lunardate, 0, sizeof(lunardate));
	lunardate.wYear  = ptm->tm_year + 1900;
	lunardate.wMonth = ptm->tm_mon  + 1;
	lunardate.wDay   = ptm->tm_mday;
	cjxGetLunarDate(&lunardate);
	// 构造一个字符串返回给调用者
	char buf[80];
	int len = sprintf(buf, "%d年%d月%d日 农历%s(%s)年%s月%s 星期%s",
		lunardate.wYear, lunardate.wMonth, lunardate.wDay,
		lunardate.szYearGanZhi, lunardate.szYearShengXiao,
		lunardate.szLunarMonth, lunardate.szLunarDay, cjxGetWeekName(lunardate.wWeekDay));
	PyObject* pString = Py_BuildValue("s", buf);
	return pString;
}

//------------------------------------------------------
//  计算 公历 year-month-day 到 1900-1-0的天数
//     参数: 公历的年月日 1900-1-1 ~ 2100-12-31 之间
//     返回:
//          天数, 一个正数值 1900-1-1为 1
//-----------------------------------------------------
static PyObject* wrap_GetOffsetSolardays(PyObject* self, PyObject* args)
{
	int y, m, d;
	//解析参数
	if (! PyArg_ParseTuple(args, "iii", &y, &m , &d))
	{
		PyErr_SetString(PyExc_TypeError, "无效的参数");
		return NULL;
	}
	//计算天数
	int days = cjxGetOffsetSolarDays(y, m, d);
	//构造整数返回值
	return Py_BuildValue("i", days);
}

//-----------------------------------------------------
//获取 距离 1900年1月1日 days 天的日期
//      1900年1月1日为1
//  参数: 距离1900-1-1的天数
//-----------------------------------------------------
static PyObject* wrap_DateFromOffsetdays(PyObject* self, PyObject* args)
{
	int days;
	//解析参数
	if (! PyArg_ParseTuple(args, "i", &days))
	{
		PyErr_SetString(PyExc_TypeError, "无效的参数");
		return NULL;
	}
	//临时变量，计算用
	LUNARDATE  lunar_date;
	//计算日期
	cjxGetDate(days, &lunar_date);
	//构造整数返回值
	return lunar_to_dict(lunar_date);
}

///////////////////////////////////////////////////////////////////////
//----------------------------------------------------
// 模块提供的方法
//----------------------------------------------------
static PyMethodDef lunarMethods[] =
{
	{"get_today", wrap_GetToday, METH_VARARGS, "get_today(): 获取当天的农历日期"},
	{"get_todaystring", wrap_GetTodayString, METH_VARARGS, "get_todaystring(): 获取当天的日期字符串"},
	{"get_date", wrap_GetDate, METH_VARARGS, "get_date(time():int): 从指定的UNIX时间戳获取日期对象"},
	{"get_lunardate", wrap_GetLunarDate, METH_VARARGS, "get_lunardate(y, m, d): 从公历日期获取农历日期"},
	{"get_solardate", wrap_GetSolarDate, METH_VARARGS, "get_solardate(y, m, d, isleap): 从农历日期获取公历日期"},
	{"get_offset_solardays", wrap_GetOffsetSolardays, METH_VARARGS, "get_offset_solardays(y, m, d): 公历 y-m-d 到 1900-1-1(=1)的天数 "},
	{"get_date_from_offsetdays", wrap_DateFromOffsetdays, METH_VARARGS, "get_date_from_offsetdays(days): 计算距离1900-1-1(=1)为days天的日期"},
	{NULL, NULL}
};

// 模块的文档
static char* module_doc = "Python bindings for lunar（中国农历）\nauthor：陈逸少\nmail:jmchxy@gmail.com";
static char* module_author = "陈逸少(jmchxy@gmail.com)";
// 模块状态
struct module_state
{
	PyObject *error;
};
// 获取状态
#define GETSTATE(m) ((struct module_state*)PyModule_GetState(m))

static int pylunar_traverse(PyObject* m, visitproc visit, void *arg)
{
	Py_VISIT(GETSTATE(m)->error);
	return 0;
}

//清除错误信息
static int pylunar_clear(PyObject *m)
{
	Py_CLEAR(GETSTATE(m)->error);
	return 0;
}

//设置错误信息
static PyObject* error_out(PyObject *m, const char* errmsg)
{
	module_state* st = GETSTATE(m);
	PyErr_SetString(st->error, errmsg);
	return NULL;
}

// 模块定义
static PyModuleDef moduledef =
{
	PyModuleDef_HEAD_INIT,
	"pylunar",
	module_doc,	//m_doc, can be NULL
	sizeof(struct module_state),
	lunarMethods,
	NULL,				//*m_slots
	pylunar_traverse,	//m_traverse
	pylunar_clear,	//m_clear
	NULL	//m_free
};

// 初始化模块
PyMODINIT_FUNC PyInit_pylunar()
{
	//创建模块
	PyObject* m = PyModule_Create(&moduledef);
	if (m == NULL || !PyModule_Check(m))
	{
		return NULL;
	}
	//创建错误对象
	struct module_state *st = GETSTATE(m);
	st->error = PyErr_NewException("pylunar.Error", NULL, NULL);
	if (st->error == NULL)
	{
		Py_DECREF(m);
	}
	//获取模版字典，然后给模版字典增加参数
	PyObject* d = PyModule_GetDict(m);
	if(d == NULL)
	{
		return NULL;
	}
	//添加变量
	PyDict_SetItemString(d, "__author__", Py_BuildValue("s", module_author));
	//添加版本变量
	PyDict_SetItemString(d, "version", Py_BuildValue("(ii)", 0, 1));
	//返回模块
	return m;
}
