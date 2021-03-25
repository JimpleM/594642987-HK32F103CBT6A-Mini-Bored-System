# HK32F103CBT6A最小开发板系统
## 功能描述
利用国产MCU HK32F103CBT6A设计出一个最小开发板系统，并利用该开发板实现OLED显示、联网模组MCU开发方案、温湿度传感器、USB通信的功能。
## 设计过程
### 一、元件选型

 1. 主控选择HK32F103CBT6A，借此机会了解国产航顺的MCU的性能，并探索与STM32F103系列的区别（STM32价格越来越贵了）。
 2. 稳压芯片使用比较流行的AMS1117-3.3，最高达到1A，满足最小系统一般需求。
 3. 采用现在流行的Type-C接口。
 4. 采用sht30作为温湿度传感器，利用多余的联网模组拓展WIFI功能。
### 二、PCB设计
PCB设计采用最小核心板+扩展板的形式。
![PCB板](https://img-blog.csdnimg.cn/20210324204145108.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L0phY2tfTWFzaw==,size_16,color_FFFFFF,t_70)
左侧为拓展板，拓展板上将BOOT0与GND连接，HK32可以从FLASH启动，板上还有涂鸦WBR3联网模块和SHT30的传感器，可拼接到开发板上使用，也可以用于其他板上的使用，同时，留有0欧电阻焊盘，可将模组单独使用
右侧为核心板，元件摆放紧凑，3v3的供电线宽为1.2mm，保障芯片的供电，同时，在D+信号传输线上上拉了1.5k欧电阻。
### 焊接后的电路板
![在这里插入图片描述](https://img-blog.csdnimg.cn/20210324215032181.jpg?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L0phY2tfTWFzaw==,size_16,color_FFFFFF,t_70)

## 三、程序
 - [x] OLED显示
 - [x] SHT30温湿度传感器
 - [ ] USB通信
 - [x] 联网模块
 
### 1、OLED显示
![在这里插入图片描述](https://img-blog.csdnimg.cn/2021032421380660.jpg?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L0phY2tfTWFzaw==,size_16,color_FFFFFF,t_70)
OLED显示工作正常。
### 2、SHT30温湿度传感器
![在这里插入图片描述](https://img-blog.csdnimg.cn/20210324214418521.jpg?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L0phY2tfTWFzaw==,size_16,color_FFFFFF,t_70)
由于设计时没有考虑引脚问题，涂鸦USART3与I2C2引脚冲突，无法使用OLED显示温湿度数据。（新手小白，打板后才知道I2C总线可以控制不同地址的传感器，地址相同时可以通过通断电或传输线来寻址。在这里，SHT30温湿度传感器地址（0x44）、OLED（0x78），可以用同一个I2C总线控制，下次可以尝试）
SHT30温湿度传感器正常工作。
### 3、USB通信
USB通信代码比较复杂，还在学习中，后续补上。
### 4、涂鸦联网模组
在测试助手上，目前MCU对联网模组配置成功，但由于模组还在路上，只能后续补上了。
![在这里插入图片描述](https://img-blog.csdnimg.cn/2021032519514310.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L0phY2tfTWFzaw==,size_16,color_FFFFFF,t_70)
通过对比HKF103和STM32F103，发现代码中HK的USART数据寄存器与STM32有所不同。
HK是Send和Receive共用DR，而STM32时分别用TDR和RDR。同样STM32的ISR在HK中变成SR，在代码移植时要阅读库代码对比后才发现。
![在这里插入图片描述](https://img-blog.csdnimg.cn/20210325201231259.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L0phY2tfTWFzaw==,size_16,color_FFFFFF,t_70)
