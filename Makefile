#lunar: 农历桌面月历 gtk
#   jmchxy@gmail.com

CC=g++

CFLAGS=`pkg-config --cflags --libs gtk+-2.0`

OBJECTS=jlunar.o jlunarJQ.o jcalendar.o lunar.o

lunar:$(OBJECTS)
	$(CC) -o lunar $(CFLAGS) $(OBJECTS) 

jlunar.o:jlunar.cpp jlunar.h
	$(CC) $(CFLAGS) -c jlunar.cpp

jlunarJQ.o:jlunarJQ.cpp jlunar.h
	$(CC) $(CFLAGS) -c jlunarJQ.cpp

jcalendar.o:jcalendar.cpp jcalendar.h jlunar.h
	$(CC) $(CFLAGS) -c jcalendar.cpp

lunar.o:lunar.cpp jcalendar.h jlunar.h
	$(CC) $(CFLAGS) -c lunar.cpp

install:
	cp lunar /usr/local/bin/lunar

clean:
	rm -f *.o

