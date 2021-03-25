#ifndef i2c
#define i2c 

#include "hk32f10x.h"
#include "delay.h"
/* 定义I2C总线连接的GPIO端口, 用户只需要修改下面4行代码即可任意改变SCL和SDA的引脚 */
#define SHT30_GPIO_PORT GPIOB
/* IIC_SCL --> PB8
 * IIC_SDA --> PB9 */
#define SHT30_I2C_SCL_PIN  GPIO_Pin_8
#define SHT30_I2C_SDA_PIN  GPIO_Pin_9
#define SHT30_RCC   RCC_APB2Periph_GPIOB

/* 定义读写SCL和SDA的宏，已增加代码的可移植性和可阅读性 */
//定义SCL
#define SHT30_I2C_SCL_Set()    GPIO_SetBits(SHT30_GPIO_PORT, SHT30_I2C_SCL_PIN)
#define SHT30_I2C_SCL_Reset()    GPIO_ResetBits(SHT30_GPIO_PORT, SHT30_I2C_SCL_PIN)
//定义SDA
#define SHT30_I2C_SDA_Set()    GPIO_SetBits(SHT30_GPIO_PORT, SHT30_I2C_SDA_PIN)
#define SHT30_I2C_SDA_Reset()    GPIO_ResetBits(SHT30_GPIO_PORT, SHT30_I2C_SDA_PIN)
//读取SDA线的状态
#define SHT30_I2C_SDA_READ() GPIO_ReadInputDataBit(SHT30_GPIO_PORT, SHT30_I2C_SDA_PIN)

void I2C_INIT(void);        /* 软件模拟IIC引脚初始化 */
void I2C_Start(void);       /* 启动I2C总线,即发送I2C起始条件 */
void I2C_Stop(void);        /* 结束I2C总线,即发送I2C结束条件 */
void I2C_ACK(void);				/* 发送应答 ACK */
void I2C_NACK(void);			/* 发送非应答 NACK */
uint8_t I2C_Wait_ACK(void);		/* 等待ACK */
void I2C_SendByte(uint8_t byte);/* 一个字节数据发送函数 */ 
uint8_t I2C_ReadByte(void); 		/* 一个字节数据接收函数 */
uint8_t I2C_CheckDevice(uint8_t _Address); //检测I2C总线设备，CPU向发送设备地址，然后读取设备应答来判断该设备是否存在
#endif
