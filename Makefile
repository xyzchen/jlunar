########################################
#通用参数设定
########################################
#指定编译器
cc     = g++

#编译参数: -fpic：生成共享库
cflags = `pkg-config --cflags --libs gtk+-2.0`

#编译文件夹（存放.o文件）
outdir = ./build

#项目名称(目标文件名) 
#lunar: 农历桌面月历 gtk
#   jmchxy@gmail.com
proj   = lunar

#生成目标文件使用的 .o 文件
objs   = \
	$(outdir)/jlunar.o \
	$(outdir)/jlunarJQ.o \
	$(outdir)/jcalendar.o \
	$(outdir)/lunar.o
	
#目标文件与 .o文件的依赖关系
$(proj):$(objs)
	$(cc) -o $(outdir)/$(proj) $(cflags) $(objs)

#.o文件与原文件的依赖关系
$(outdir)/jlunar.o: jlunar.cpp jlunar.h
	$(cc) $(cflags) -c -o $(outdir)/jlunar.o jlunar.cpp

$(outdir)/jlunarJQ.o: jlunarJQ.cpp jlunar.h
	$(cc) $(cflags) -c -o $(outdir)/jlunarJQ.o jlunarJQ.cpp

$(outdir)/jcalendar.o: jcalendar.cpp jlunar.h
	$(cc) $(cflags) -c -o $(outdir)/jcalendar.o jcalendar.cpp
	
$(outdir)/lunar.o: lunar.cpp jlunar.h
	$(cc) $(cflags) -c -o $(outdir)/lunar.o lunar.cpp

install:                     #安装
	cp $(outdir)/$(proj) /usr/local/bin/$(proj)

clean:                      #clean为目标
	rm -f $(objs) $(outdir)/$(proj)

