# jlunar


## 农历日历计算工具包：

    jlunar.h jlunar.cpp jlunarJQ.cpp    农历计算函数

    jcalendar.h jcalendar.cpp        农历月历位置计算和绘图示范应用

	lunar.cpp   lunar_gtk3.cpp       使用 gtk2 和 gtk3 构建的桌面日历

    pylunar.cpp                      农历计算的python扩展

    setup.py                         python扩展安装脚本：python setup.py install

    junar.js                         JavaScript农历计算

    getlunar.cpp                     获取javascript函数需要的农历数据


## 构建

### 1. gkt2 桌面日历

```
	make 
	sudo make install
```

### 2. gtk3 桌面日历
```
	make -f makegtk3
	sudo make -f makegtk3 install
```
### 3. 安装 pylunar 扩展

```
sudo python setup.py install
```
