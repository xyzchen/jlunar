//--------------------------------------------------------
//  基于 gtk3 的农历程序       
//     作者: 逸少 
//    jmchxy@163.com
//      2003-6
//   修改: 200-3,2015-6
//--------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits>
#include <glib.h>
#include <cairo.h>
#include <gtk/gtk.h>
#include "jcalendar.h"

#ifndef MAX_PATH
	#define MAX_PATH 512
#endif

// 常量
const int MAIN_W = 320;
const int MAIN_H = 280;

//---------------------------------------------------------------------
// 全局变量
//---------------------------------------------------------------------
// 鼠标状态
gboolean   g_drag = FALSE;		//是否在拖动状态
int g_x = 0;	//鼠标相对于窗口的位置
int g_y = 0;	//鼠标相对于窗口的位置
//---------------------------------------------------------------------
// GUI 构件
GtkWidget* g_mainwin  = NULL;	//主窗口
GtkWidget* g_popmenu  = NULL;	//弹出式菜单
JCalendar* g_calendar = NULL; 	//日历对象
//---------------------------------------------------------------------
int g_posX = 0;		//窗口位置X
int g_posY = 0;		//窗口位置Y
double g_opacity = 0.0;	//主窗口背景区域透明度(0.0为完全透明，1.0为不透明)
//---------------------------------------------------------------------

//读取配置文件
static void load_config()
{
	//配置文件文件名
	char  config[MAX_PATH+1];
	strcpy(config, getenv("HOME"));
	strcat(config, "/.config/lunar.ini");
	//计算位置默认位置
	g_posX = gdk_screen_get_width(gdk_screen_get_default()) - MAIN_W - 4;
	g_posY = gdk_screen_get_height(gdk_screen_get_default()) - MAIN_H - 40;
//	fprintf(stderr, "Default Postion at (%d, %d)...\n", g_posX, g_posY);
	//读取配置文件
	GError* error = NULL;
	GKeyFileFlags flags = (GKeyFileFlags)(G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS);
	GKeyFile* keyfile = g_key_file_new ();
	if(!g_key_file_load_from_file(keyfile, config, flags, &error))
	{
		fprintf(stderr, "%s\n", error->message);
		g_error_free(error);
		return;
	}
	//读取保存的位置信息
	g_posX = g_key_file_get_integer(keyfile, "position", "x", NULL);
	g_posY = g_key_file_get_integer(keyfile, "position", "y", NULL);
	//读取透明度信息
	g_opacity = g_key_file_get_double(keyfile, "position", "opacity", NULL);
	//释放文件
	g_key_file_free(keyfile);
}

//保存配置文件
static void save_config()
{
	//配置文件文件名
	char  config[MAX_PATH+1];
	strcpy(config, getenv("HOME"));
	strcat(config, "/.config/lunar.ini");
	//保存位置到配置文件中
	GError* error = NULL;
	GKeyFileFlags flags = (GKeyFileFlags)(G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS);
	GKeyFile* keyfile = g_key_file_new ();
	g_key_file_set_integer(keyfile, "position", "x", g_posX);
	g_key_file_set_integer(keyfile, "position", "y", g_posY);
	g_key_file_set_double(keyfile, "position", "opacity", g_opacity);
	g_key_file_save_to_file(keyfile, config, &error);
	g_key_file_free(keyfile);
}

//---------------------------------------------------------------------
//关闭窗口事件
//---------------------------------------------------------------------
static void destroy(GtkWidget *widget, gpointer data)
{
	//退出前保存配置文件
	save_config();
	gtk_widget_destroy(GTK_WIDGET(g_mainwin));
}

//---------------------------------------------------------------------
// 鼠标事件
//---------------------------------------------------------------------
// 鼠标按下时的事件，只在左键按下时拖动窗体
static gint button_press_event(GtkWidget* widget, GdkEventButton* event, gpointer data)
{	//按键：1：左键；2：中键；3：右键
	if((event->button == 1)&& (event->y <40))	//判断是否左键按下
	{
		g_drag = TRUE;		// 进入拖动状态
		g_x    = int(event->x);	//取得鼠标相对于窗口的位置
		g_y    = int(event->y);
	}
	else if(event->button == 3)	//右键
	{ 
		if(g_popmenu != NULL)
		{
			gtk_menu_popup_at_pointer(GTK_MENU(g_popmenu), (GdkEvent *)event);
		}
		return TRUE;
	}
	return TRUE;
}

// 鼠标抬起事件
static gint button_release_event(GtkWidget* widget, GdkEventButton* event, gpointer data)         
{
	if(event->button == 1)
	{
		g_drag = FALSE;
	}
	return TRUE;
}

// 鼠标移动事件
static gint motion_notify_event(GtkWidget* widget, GdkEventMotion* event, gpointer data)         
{
	if(g_drag)
	{
		//这里的实现有问题，gtk3 在 Wayland 下 工作不正常。
		int x, y;
		GtkWidget* window = (GtkWidget *)data;
		gtk_window_get_position((GtkWindow *)g_mainwin, &x, &y);	//取窗体绝对坐标
//		fprintf(stderr, "Window old pisiton at (%d, %d)\n", x, y);
		g_posX = x + event->x - g_x;
		g_posY = y + event->y - g_y;
		gtk_window_move((GtkWindow*)g_mainwin, g_posX, g_posY);	//移动窗体
//		fprintf(stderr, "Window new pisiton at (%d, %d)\n", g_posX, g_posY);
	}
	return TRUE;
}

//---------------------------------------------------------------------
// 绘图事件
//---------------------------------------------------------------------
static gint draw_event(GtkWidget* widget,  cairo_t *cr, gpointer data)
{
	//获得绘图区尺寸
	GtkAllocation allocation;
	gtk_widget_get_allocation(widget, &allocation);
	int area_w = allocation.width;
	int area_h = allocation.height;
	
	//cairo绘图
	cairo_set_source_rgba(cr, 0, 0, 0, g_opacity);	//设置窗口背景的透明度
	cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);	// 重要
	cairo_paint(cr);
	//cairo_save (cr);
	
	//绘制日历
	g_calendar->Draw(cr);
	
	//cairo_restore (cr);
	return TRUE;
}

//---------------------------------------------------------------------
// 弹出式菜单和菜单项事件
//---------------------------------------------------------------------
//下一个月
static void menu_nextmonth(GtkWidget *widget,  gpointer data)
{
	g_calendar->SelectNextMonth();
	gtk_widget_queue_draw(g_mainwin);
}

//上一个月
static void menu_prevmonth(GtkWidget *widget,  gpointer data)
{
	g_calendar->SelectPrevMonth();
	gtk_widget_queue_draw(g_mainwin);
}

//下一年
static void menu_nextyear(GtkWidget *widget,  gpointer data)
{
	g_calendar->SelectNextYear();
	gtk_widget_queue_draw(g_mainwin);
}

//上一年
static void menu_prevyear(GtkWidget *widget,  gpointer data)
{
	g_calendar->SelectPrevYear();
	gtk_widget_queue_draw(g_mainwin);
}

//重置
static void menu_reset(GtkWidget *widget,  gpointer data)
{
	g_calendar->Reset();
	gtk_widget_queue_draw(g_mainwin);
}

//保存配置
static void menu_savecfg(GtkWidget *widget,  gpointer data)
{
	save_config();
}

//关于
static void menu_about(GtkWidget *widget,  gpointer data)
{
	const gchar* authors[] = {"陈逸少", NULL};
	GtkAboutDialog* about = (GtkAboutDialog*)gtk_about_dialog_new ();
	gtk_about_dialog_set_program_name(about, "农历桌面日历");
	gtk_about_dialog_set_comments(about, "农历桌面透明背景农历日历");
	gtk_about_dialog_set_version(about, "1.0.0");
	gtk_about_dialog_set_copyright(about, "(c)陈逸少");
	gtk_about_dialog_set_authors(about, (const gchar**)authors);
	gtk_about_dialog_set_website(about, "https://github.com/xyzchen/jlunar");
	gtk_window_set_position(GTK_WINDOW(about), GTK_WIN_POS_CENTER_ALWAYS);
	gtk_dialog_run (GTK_DIALOG(about));
	gtk_widget_destroy(GTK_WIDGET(about));
}

//创建弹出式菜单
static GtkWidget* create_popmenu()
{
	// 创建菜单
	GtkWidget* popmenu = gtk_menu_new();
	
	//创建菜单项： 
	//下一个月
	GtkWidget* nextmonth_item = gtk_menu_item_new_with_label("下一月");	//菜单项	
	gtk_menu_shell_append (GTK_MENU_SHELL(popmenu), nextmonth_item);	//添加到菜单中
	g_signal_connect_swapped(G_OBJECT(nextmonth_item), "activate", G_CALLBACK(menu_nextmonth), (gpointer)g_strdup("NextMonth"));	//绑定事件
	
	//上一个月
	GtkWidget* prevmonth_item = gtk_menu_item_new_with_label("上一月");	//菜单项	
	gtk_menu_shell_append(GTK_MENU_SHELL(popmenu), prevmonth_item);	//添加到菜单中
	g_signal_connect_swapped(G_OBJECT(prevmonth_item), "activate", G_CALLBACK(menu_prevmonth), (gpointer)g_strdup("PrevMonth"));	//绑定事件
	
	//下一年
	GtkWidget* nextyear_item = gtk_menu_item_new_with_label("下一年");	//菜单项	
	gtk_menu_shell_append(GTK_MENU_SHELL(popmenu), nextyear_item);	//添加到菜单中
	g_signal_connect_swapped(G_OBJECT(nextyear_item), "activate", G_CALLBACK(menu_nextyear), (gpointer)g_strdup("NextYear"));	//绑定事件
	
	//上一年
	GtkWidget* prevyear_item = gtk_menu_item_new_with_label("上一年");	//菜单项	
	gtk_menu_shell_append(GTK_MENU_SHELL(popmenu), prevyear_item);	//添加到菜单中
	g_signal_connect_swapped(G_OBJECT(prevyear_item), "activate", G_CALLBACK(menu_prevyear), (gpointer)g_strdup("PrevYear"));	//绑定事件
	
	//刷新
	GtkWidget* reset_item = gtk_menu_item_new_with_label("重置");	//菜单项	
	gtk_menu_shell_append(GTK_MENU_SHELL(popmenu), reset_item);	//添加到菜单中
	g_signal_connect_swapped(G_OBJECT(reset_item), "activate", G_CALLBACK(menu_reset), (gpointer)g_strdup("Reset"));	//绑定事件
	
	//保存配置
	GtkWidget* savecfg_item = gtk_menu_item_new_with_label("保存");	//菜单项	
	gtk_menu_shell_append(GTK_MENU_SHELL(popmenu), savecfg_item);	//添加到菜单中
	g_signal_connect_swapped(G_OBJECT(savecfg_item), "activate", G_CALLBACK(menu_savecfg), (gpointer)g_strdup("SaveCfg"));	//绑定事件

	//关于
	GtkWidget* about_item = gtk_menu_item_new_with_label("关于");	//菜单项
	gtk_menu_shell_append(GTK_MENU_SHELL(popmenu), about_item);	//添加到菜单中
	g_signal_connect_swapped(G_OBJECT(about_item), "activate", G_CALLBACK(menu_about), (gpointer)g_strdup("About"));	//绑定事件

	GtkWidget* line_item = gtk_separator_menu_item_new();	//分割线
	gtk_menu_shell_append(GTK_MENU_SHELL(popmenu), line_item);	//添加到菜单中

	//退出
	GtkWidget* quit_item = gtk_menu_item_new_with_label("退出");
	gtk_menu_shell_append(GTK_MENU_SHELL(popmenu), quit_item);	//添加到菜单中
	g_signal_connect_swapped(G_OBJECT(quit_item), "activate", G_CALLBACK(destroy), (gpointer)g_strdup("Quit"));	//绑定事件
	
	return popmenu;
}

//---------------------------------------------------------------------
// 主函数
//---------------------------------------------------------------------
static void on_app_activate(GApplication *app, gpointer data)
{
	//创建主窗口
	g_mainwin = gtk_application_window_new(GTK_APPLICATION(app));
	//设置窗口属性
	gtk_window_set_skip_taskbar_hint(GTK_WINDOW(g_mainwin), TRUE);	//不在任务栏显示窗口列表
	gtk_window_set_skip_pager_hint(GTK_WINDOW(g_mainwin), TRUE);
	gtk_window_set_keep_below(GTK_WINDOW(g_mainwin), TRUE);	//置底
	gtk_window_set_decorated(GTK_WINDOW(g_mainwin), FALSE);	//去掉边框
	//gtk_window_set_gravity(GTK_WINDOW(g_mainwin), GDK_GRAVITY_STATIC);
	//设置窗口大小、位置、背景
	gtk_widget_set_size_request(g_mainwin, MAIN_W, MAIN_H);	//设置大小
	gtk_window_move(GTK_WINDOW(g_mainwin), g_posX, g_posY);	//移动窗体
	gtk_widget_set_opacity(g_mainwin, 1.0);	//设置透明度，不透明，这个函数设置的是整个窗口的透明度
	//创建弹出菜单
	g_popmenu = create_popmenu();
	gtk_widget_show_all(GTK_WIDGET(g_popmenu)); // 一定要显示菜单项
	//创建日历对象s
	g_calendar = new JCalendar();
	RECT  rect={0, 0, MAIN_W, MAIN_H};
	g_calendar->SetRect(rect);
	//创建绘图区域
	GtkWidget* draw_area = gtk_drawing_area_new();
	//绘图区加入到主窗口
	gtk_container_add(GTK_CONTAINER(g_mainwin), draw_area); 
	gtk_widget_show(draw_area);
	//设置绘图区事件
	g_signal_connect(draw_area, "draw",  G_CALLBACK(draw_event), NULL);  
	
	//设置主窗体要处理的事件
	gtk_widget_set_events(g_mainwin,
				  GDK_EXPOSURE_MASK | GDK_LEAVE_NOTIFY_MASK
				| GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK
				| GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK);
	//设置事件处理函数	
	g_signal_connect(G_OBJECT(g_mainwin), "button_press_event", G_CALLBACK(button_press_event), g_mainwin);
	g_signal_connect(G_OBJECT(g_mainwin), "motion_notify_event", G_CALLBACK(motion_notify_event), g_mainwin);
	g_signal_connect(G_OBJECT(g_mainwin), "button_release_event", G_CALLBACK(button_release_event), g_mainwin);
	g_signal_connect(G_OBJECT(g_mainwin), "destroy", G_CALLBACK(destroy), NULL);

	//设置透明颜色处理
	GdkScreen* screen = gtk_widget_get_screen(g_mainwin);	// 重要
	GdkVisual* visual = gdk_screen_get_rgba_visual(screen);
	if(visual == NULL)
	{
		visual = gdk_screen_get_system_visual(screen);
	}
	gtk_widget_set_visual(GTK_WIDGET(g_mainwin), visual);
	//显示窗口
	gtk_widget_show_all(g_mainwin);
}

//---------------------------------------------------------------------
// 主函数
//---------------------------------------------------------------------
int main(int argc, char** argv)
{
	//初始化
	gtk_init(&argc, &argv);
	//装载配置文件
	load_config();
	//创建应用程序
	GtkApplication* app = gtk_application_new("org.gtk.lunar", G_APPLICATION_FLAGS_NONE);
	//连接事件
	g_signal_connect(app, "activate", G_CALLBACK(on_app_activate), NULL);
	//运行
	int status = g_application_run(G_APPLICATION(app), argc, argv);
	//删除app对象
	g_object_unref(app);
	//返回结果
	return status;
}
