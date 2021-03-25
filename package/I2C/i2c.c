#include "i2c.h"

/* 软件模拟IIC引脚初始化
 * IIC_SCL --> PB8
 * IIC_SDA --> PB9 */
void I2C_INIT(void){
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = SHT30_I2C_SCL_PIN|SHT30_I2C_SDA_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;   //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SHT30_GPIO_PORT, &GPIO_InitStructure);
    I2C_Stop();
}

/* 描述：启动I2C总线,即发送I2C起始条件. 
 * 参数：  无
 * 返回值：无						*/
void I2C_Start(void){
    SHT30_I2C_SDA_Set();
    SHT30_I2C_SCL_Set();
    delay_us(4);
    SHT30_I2C_SDA_Reset();
    delay_us(4);
    SHT30_I2C_SCL_Reset();
}

/* 描述：结束I2C总线,即发送I2C结束条件.  
 * 参数：  无
 * 返回值：无						*/
void I2C_Stop(void){
    SHT30_I2C_SDA_Reset();
    SHT30_I2C_SCL_Reset();
    delay_us(4);
    SHT30_I2C_SDA_Set();
}

/* 描述：发送应答 ACK 
 * 参数：  无
 * 返回值：无		*/
void I2C_ACK(void){
    SHT30_I2C_SDA_Reset();
    delay_us(2);
    SHT30_I2C_SCL_Set();
    delay_us(2);
    SHT30_I2C_SCL_Reset();
    delay_us(2);
    SHT30_I2C_SDA_Set();
}

/* 描述：发送非应答 NACK 
 * 参数：  无
 * 返回值：无		*/
void I2C_NACK(void){
    SHT30_I2C_SDA_Set();
    delay_us(2);
    SHT30_I2C_SCL_Set();
    delay_us(2);
    SHT30_I2C_SCL_Reset();
    delay_us(2);
}

/* 描述：等待ACK 
 * 参数：  无
 * 返回值：等待应答返回0，没有等待到应答返回1	*/
uint8_t I2C_Wait_ACK(void){
    uint8_t t;

    SHT30_I2C_SDA_Set();
    delay_us(1);
    SHT30_I2C_SCL_Set();
    delay_us(1);
    if(SHT30_I2C_SDA_READ()){
        t = 1;
    }else{
        t = 0;
    }
    SHT30_I2C_SCL_Reset();
    delay_us(1);
    return t;
}

/* 描述：一个字节数据发送函数               
 * 参数：  无
 * 返回值：无		*/
void I2C_SendByte(uint8_t Byte){
    uint8_t i;
    for(i=0; i<8; i++){     /* 要传送的数据长度为8位 */
        if(Byte & 0x80){    /* 取最高发送位即第8位 */
            SHT30_I2C_SDA_Set();
        }else{
            SHT30_I2C_SDA_Reset();
        }
        delay_us(2);
        SHT30_I2C_SCL_Set();
        delay_us(2);
        SHT30_I2C_SCL_Reset();
        if(i == 7){
            SHT30_I2C_SDA_Set();    // 释放总线
        }
        Byte <<= 1;     //左移1个位
        delay_us(2);

    }
}

/* 描述：一个字节数据接收函数               
 * 参数：  无
 * 返回值：接收到的字节数据		*/   
uint8_t I2C_ReadByte(void){
    uint8_t i,value;
    value = 0;
    for(i=0; i<8; i++){
        value <<= 1;
        SHT30_I2C_SCL_Set();
        delay_us(2);
        if(SHT30_I2C_SDA_READ()){
            value++;
        }
        SHT30_I2C_SCL_Reset();
        delay_us(1);
    }
    return value;
}

/*	功能说明: 检测I2C总线设备，CPU向发送设备地址，然后读取设备应答来判断该设备是否存在
*	形    参：_Address：设备的I2C总线地址
*	返 回 值: 返回值 0 表示正确， 返回1表示未探测到*/
uint8_t I2C_CheckDevice(uint8_t _Address){
    uint8_t Ack;
    I2C_INIT();

    I2C_Start();

    I2C_SendByte(_Address | 0);
    Ack = I2C_Wait_ACK();

    I2C_Stop();
    return Ack;
}
