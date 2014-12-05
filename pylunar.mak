########################################
#通用参数设定
########################################
#指定编译器
cc     = g++

#编译参数: -fpic：生成共享库
cflags = -fpic -c

#编译文件夹（存放.o文件）
outdir = ./build

#项目相关,附加的头文件目录
cinc   = -I/usr/include/python2.7 -I/usr/lib/python2.7/config

#项目名称(目标文件名)
proj   = pylunar

#生成目标文件使用的 .o 文件
objs   = \
	$(outdir)/jlunar.o \
	$(outdir)/jlunarJQ.o \
	$(outdir)/pylunar.o

#目标文件与 .o文件的依赖关系
$(proj).so:$(objs)
	$(cc) -fpic -shared -o $(proj).so $(objs)

#.o文件与原文件的依赖关系
$(outdir)/jlunar.o:jlunar.cpp jlunar.h
	$(cc) $(cflags) -o $(outdir)/jlunar.o jlunar.cpp

$(outdir)/jlunarJQ.o:jlunarJQ.cpp jlunar.h
	$(cc) $(cflags) -o $(outdir)/jlunarJQ.o jlunarJQ.cpp

$(outdir)/pylunar.o:pylunar.cpp jlunar.h
	$(cc) $(cflags) $(cinc) -o $(outdir)/pylunar.o pylunar.cpp


clean:                           #clean为目标
	rm -f $(objs) $(proj).so

