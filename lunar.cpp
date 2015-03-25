//--------------------------------------------------------
//   基于gtk的农历程序       
//     作者: 逸少 
//    jmchxy@163.com
//      2003-6
//   修改: 2010-3
//--------------------------------------------------------

#include <gdk/gdkcursor.h>
#include <gdk/gdk.h>
#include <cairo.h>
#include <gtk/gtk.h>
#include "jcalendar.h"

// 常量
const int MAIN_W = 450;
const int MAIN_H = 400;

//---------------------------------------------------------------------
// 全局变量
//---------------------------------------------------------------------
// 鼠标状态
gboolean   g_drag = FALSE;		//是否在拖动状态
int g_x = 0;
int g_y = 0;

// GUI 构件
GtkWidget* g_mainwin  = NULL;	//主窗口
GtkWidget* g_popmenu  = NULL;	//弹出式菜单
JCalendar* g_calendar = NULL; 	//日历对象
//---------------------------------------------------------------------

//---------------------------------------------------------------------
//关闭窗口事件
//---------------------------------------------------------------------
static void destroy(GtkWidget *widget, gpointer data)
{
    gtk_main_quit();
}

//---------------------------------------------------------------------
// 鼠标事件
//---------------------------------------------------------------------
// 鼠标按下时的事件，只在左键按下时拖动窗体
static gint button_press_event(GtkWidget* widget, GdkEventButton* event, gpointer data)
{	//按键：1：左键；2：中键；3：右键
	if((event->button == 1)&& (event->y <40))	//判断是否左键按下
	{
		g_drag = TRUE;	// 进入拖动状态
		g_x    = event->x;	//取得鼠标相对于窗口的位置
		g_y    = event->y;
	}
	else if(event->button == 3)	//右键
	{ 
		if(g_popmenu != NULL)
		{
			gtk_menu_popup(GTK_MENU(g_popmenu), NULL, NULL, NULL, NULL, event->button, event->time);
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
static gint motion_notify_event(GtkWidget* widget, GdkEventButton* event, gpointer data)         
{
	if(g_drag)
	{
		int x, y;
		GtkWidget* window = (GtkWidget *)data;
		gtk_window_get_position((GtkWindow *)window, &x, &y);	//取窗体绝对坐标
		gtk_window_move((GtkWindow*)window, x + event->x - g_x, y + event->y - g_y);	//移动窗体
	}
	return TRUE;
}

//---------------------------------------------------------------------
// 绘图事件
//---------------------------------------------------------------------
static gint expose_event(GtkWidget* widget, GdkEventExpose* event)
{
	//获得绘图区尺寸
	int area_w = widget->allocation.width;
	int area_h = widget->allocation.height;
	
	//cairo绘图
	cairo_t* cr = gdk_cairo_create(widget->window);
	cairo_set_source_rgba(cr, 0, 0, 0, 0);	//设置透明度为完全透明
	cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);	// 重要
	cairo_paint(cr);
	//cairo_save (cr);
	
	//绘制日历
	g_calendar->Draw(cr);
	
	//cairo_restore (cr);
	cairo_destroy(cr);
           
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

//创建弹出式菜单
static GtkWidget* create_popmenu()
{
	// 创建菜单
	GtkWidget* popmenu = gtk_menu_new();
	
	//创建菜单项： 
	//下一个月
	GtkWidget* nextmonth_item = gtk_menu_item_new_with_label("下一月");	//菜单项	
	gtk_menu_append(GTK_MENU(popmenu), nextmonth_item);	//添加到菜单中
	g_signal_connect_swapped(G_OBJECT(nextmonth_item), "activate", G_CALLBACK(menu_nextmonth), (gpointer)g_strdup("NextMonth"));	//绑定事件
	
	//上一个月
	GtkWidget* prevmonth_item = gtk_menu_item_new_with_label("上一月");	//菜单项	
	gtk_menu_append(GTK_MENU(popmenu), prevmonth_item);	//添加到菜单中
	g_signal_connect_swapped(G_OBJECT(prevmonth_item), "activate", G_CALLBACK(menu_prevmonth), (gpointer)g_strdup("PrevMonth"));	//绑定事件
	
	//下一年
	GtkWidget* nextyear_item = gtk_menu_item_new_with_label("下一年");	//菜单项	
	gtk_menu_append(GTK_MENU(popmenu), nextyear_item);	//添加到菜单中
	g_signal_connect_swapped(G_OBJECT(nextyear_item), "activate", G_CALLBACK(menu_nextyear), (gpointer)g_strdup("NextYear"));	//绑定事件
	
	//上一年
	GtkWidget* prevyear_item = gtk_menu_item_new_with_label("上一年");	//菜单项	
	gtk_menu_append(GTK_MENU(popmenu), prevyear_item);	//添加到菜单中
	g_signal_connect_swapped(G_OBJECT(prevyear_item), "activate", G_CALLBACK(menu_prevyear), (gpointer)g_strdup("PrevYear"));	//绑定事件
	
	//刷新
	GtkWidget* reset_item = gtk_menu_item_new_with_label("重置");	//菜单项	
	gtk_menu_append(GTK_MENU(popmenu), reset_item);	//添加到菜单中
	g_signal_connect_swapped(G_OBJECT(reset_item), "activate", G_CALLBACK(menu_reset), (gpointer)g_strdup("Reset"));	//绑定事件
	
	//退出
	GtkWidget* quit_item = gtk_menu_item_new_with_label("退出");
	gtk_menu_append(GTK_MENU(popmenu), quit_item);
	g_signal_connect_swapped(G_OBJECT(quit_item), "activate", G_CALLBACK(destroy), (gpointer)g_strdup("Quit"));	//绑定事件
	
	return popmenu;
}

//---------------------------------------------------------------------
// 主函数
//---------------------------------------------------------------------
int main(int argc, char** argv)
{
	//初始化
	gtk_init(&argc, &argv);
	
	//创建主窗口
	g_mainwin = gtk_window_new(GTK_WINDOW_TOPLEVEL);	//创建主窗口	或 GTK_WINDOW_POPUP（POPUP窗口，没有边框，置顶）
	gtk_window_set_skip_taskbar_hint(GTK_WINDOW(g_mainwin), TRUE);	//不在任务栏显示窗口列表
	gtk_window_set_keep_below(GTK_WINDOW(g_mainwin), TRUE);	//置底
	gtk_window_set_decorated(GTK_WINDOW(g_mainwin), FALSE);	//去掉边框
	//设置窗口大小、位置、背景
	gtk_widget_set_size_request(g_mainwin, MAIN_W, MAIN_H);	//设置大小
	gint width  = gdk_screen_width();	//计算位置
	gint height = gdk_screen_height();
	//gtk_widget_set_app_paintable(g_mainwin, TRUE);
	gtk_window_move(GTK_WINDOW(g_mainwin), width-MAIN_W, height-MAIN_H-60);
	//gtk_window_set_opacity(GTK_WINDOW(g_mainwin), 0.5);	//设置透明度
	
	//创建弹出菜单
	g_popmenu = create_popmenu();
	gtk_widget_show_all(GTK_WIDGET(g_popmenu)); // 一定要显示菜单项
	
	//创建日历对象
	g_calendar = new JCalendar();
	RECT  rect={0, 0, MAIN_W, MAIN_H};
	g_calendar->SetRect(rect);
	
	//颜色
	GdkColor color;
	
	//设置前景/背景色
	color.red  = 0;
	color.blue = 0; 
	color.green= 0;
	gtk_widget_modify_bg(GTK_WIDGET(g_mainwin), GTK_STATE_NORMAL, &color);	//设置背景
	color.green= 65535;
	gtk_widget_modify_fg(GTK_WIDGET(g_mainwin), GTK_STATE_NORMAL, &color);	//设置前景颜色
	
	//创建绘图区域
	GtkWidget* draw_area = gtk_drawing_area_new();
	
	//设置绘图区颜色
	color.green= 0;
	gtk_widget_modify_bg(GTK_WIDGET(draw_area), GTK_STATE_NORMAL, &color);	//设置背景
	color.green= 65535;
	gtk_widget_modify_fg(GTK_WIDGET(draw_area), GTK_STATE_NORMAL, &color);	//设置前景颜色
	
	//绘图区加入到主窗口
	gtk_container_add(GTK_CONTAINER(g_mainwin), draw_area); 
	gtk_widget_show(draw_area);
	
	//绘图事件 
	gtk_widget_set_events(draw_area, GDK_EXPOSURE_MASK);
	gtk_signal_connect(GTK_OBJECT(draw_area), "expose_event",  GTK_SIGNAL_FUNC(expose_event), NULL);  
	
	// 设置窗体要处理的事件
	gtk_widget_set_events(g_mainwin,
				  GDK_EXPOSURE_MASK | GDK_LEAVE_NOTIFY_MASK
				| GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK
				| GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK);
			
	//设置事件处理函数	
	gtk_signal_connect(GTK_OBJECT(g_mainwin), "button_press_event", GTK_SIGNAL_FUNC(button_press_event), g_mainwin);
	gtk_signal_connect(GTK_OBJECT(g_mainwin), "motion_notify_event", GTK_SIGNAL_FUNC(motion_notify_event), g_mainwin);
	gtk_signal_connect(GTK_OBJECT(g_mainwin), "button_release_event", GTK_SIGNAL_FUNC(button_release_event), g_mainwin);
	gtk_signal_connect (GTK_OBJECT(g_mainwin), "destroy", GTK_SIGNAL_FUNC(destroy), NULL);
	//设置透明颜色处理
	GdkScreen* screen = gtk_widget_get_screen(g_mainwin);	// 重要
	GdkColormap* colormap =  gdk_screen_get_rgba_colormap(screen);
	gtk_widget_set_colormap(g_mainwin, colormap);
	
	//显示窗口
	gtk_widget_show_all(g_mainwin);
	
	//进入消息循环
	gtk_main();
	
	return 0;
}

