########################################
#通用参数设定
########################################
#指定编译器
cc     = g++

#编译参数: -fpic：生成共享库
cflags = -c

#编译文件夹（存放.o文件）
outdir = ./build

#项目名称(目标文件名)
proj   = getlunar

#生成目标文件使用的 .o 文件
objs   = \
	$(outdir)/jlunar.o \
	$(outdir)/jlunarJQ.o \
	$(outdir)/getlunar.o

#目标文件与 .o文件的依赖关系
$(proj):$(objs)
	$(cc) -o $(outdir)/$(proj) $(objs)

#.o文件与原文件的依赖关系
$(outdir)/jlunar.o:jlunar.cpp jlunar.h
	$(cc) $(cflags) -o $(outdir)/jlunar.o jlunar.cpp

$(outdir)/jlunarJQ.o:jlunarJQ.cpp jlunar.h
	$(cc) $(cflags) -o $(outdir)/jlunarJQ.o jlunarJQ.cpp

$(outdir)/getlunar.o:getlunar.cpp jlunar.h
	$(cc) $(cflags) $(cinc) -o $(outdir)/getlunar.o getlunar.cpp

clean:     #clean为目标
	rm -f $(objs) $(outdir)/$(proj)

