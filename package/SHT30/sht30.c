#include "sht30.h"
#include "stdio.h"
/*
    SHT30-DIS-B2.5KS
    I2C Address A 0x44(default)  logic low
    I2C Address B 0x45           logic high
*/
#define SHT30_ADDR_WRITE 0x44<<1    /* 写7位I2C设备地址加0作为写取位 */
#define SHT30_ADDR_READ (0x44<<1)+1 /* 写7位I2C设备地址加0作为读取位 */
#define CRC8_POLYNOMIAL 0x31



/* 描述：向SHT30发送一条16bit指令 
 * 参数cmd：SHT30指令（在SHT30_MODE中枚举定义）
 * 返回值：发送成功返回0，发送失败返回1 		*/
static uint8_t SHT30_Send_Cmd(SHT30_CMD cmd){
    uint8_t cmd_buffer[2];
    uint8_t ret;
    
    cmd_buffer[0] = cmd >>8;
    cmd_buffer[1] = cmd;

    I2C_SendByte(SHT30_ADDR_WRITE);     
    ret = I2C_Wait_ACK();
    I2C_SendByte(cmd_buffer[0]);
    ret |= I2C_Wait_ACK();
    I2C_SendByte(cmd_buffer[1]);
    ret |= I2C_Wait_ACK();
    return ret;
}

uint8_t SHT30_Init(void){
    return SHT30_Send_Cmd(MEDIUM_2_CMD);
}
/* 描述：从SHT3x读取数据 
 * 参数data_len：读取多少个字节数据
 * 参数data_arr：读取的数据存放在一个数组里
 * 返回值：读取成功返回0，读取失败返回1 
*/
static uint8_t SHT30_Read_Data(uint8_t length, uint8_t *data){
    uint8_t ret,i;
    I2C_SendByte(SHT30_ADDR_WRITE);
    ret = I2C_Wait_ACK();
    if(ret != 0) return 1;
    for(i=0; i<(length-1); i++){
        data[i] = I2C_ReadByte();
        I2C_ACK();
    }
    data[i] = I2C_ReadByte();
    I2C_NACK();
    return 0;
}

/* 描述：读取传感器编号
 * 参数：存储编号数据的指针
 * 返回值：0-读取成功，1-读取失败 */
uint8_t SHT30_ReadSerialNumber(uint32_t *serialNumber){
    uint8_t ret = 0;
    uint8_t Num_buffer[4] = {0xFF,0xFF,0xFF,0xFF};

    I2C_Start();
    SHT30_Send_Cmd(READ_SERIAL_NUMBER);
    I2C_Stop();
    delay_ms(10);
    I2C_Start();
    ret = SHT30_Read_Data(4,Num_buffer);
    I2C_Stop();

    *serialNumber = (Num_buffer[0] << 24)|(Num_buffer[1]<<16)|(Num_buffer[2]<<8)|(Num_buffer[3]);
    if(0xFF == *serialNumber) return 1;
    return ret;
}

/* 描述：数据CRC校验
 * 参数message：需要校验的数据
 * 参数initial_value：crc初始值
 * 返回值：计算得到的CRC码 */
uint8_t CheckCrc8(uint8_t* const message, uint8_t initial_value){
    uint8_t remainder;
    uint8_t i=0, j=0;

    remainder = initial_value;
    for(j=0; j<2; j++){
        remainder ^= message[j];
        for(i=0; i<8; i++){
            if(remainder & 0x80){
                remainder = (remainder << 1) ^ CRC8_POLYNOMIAL;
            }else{
                remainder = (remainder << 1);
            }
        }
    }
    return remainder;
}

/* 描述：温湿度数据获取函数,周期读取，注意，需要提前设置周期模式   
 * 参数Tem_val：存储温度数据的指针, 温度单位为°C
 * 参数Hum_val：存储湿度数据的指针, 温度单位为%
 * 返回值：0-读取成功，1-读取失败*/
uint8_t SHT30_Get_Humiture_periodic(double *Tem_val, double *Hum_val){
    uint8_t ret=0;
    uint8_t buff[6];
    uint16_t tem,hum;
    double Temperature=0;
    double Humidity=0;

    I2C_Start();
    ret = SHT30_Send_Cmd(READOUT_FOR_PERIODIC_MODE);
    I2C_Start();
    ret = SHT30_Read_Data(6,buff);
    I2C_Stop();

    if(CheckCrc8(buff, 0xFF) != buff[2] || CheckCrc8(&buff[3], 0xFF) != buff[5]){
        printf("CRC_ERROR,ret = 0x%x\r\n",ret);
        return 1;
    }

    tem = (((uint16_t)buff[0]<<8) | buff[1]);
    Temperature =(175.0*(double)tem/65535.0-45.0);  // T = -45 + 175 * tem / (2^16-1)

    hum = (((uint16_t)buff[3]<<8) | buff[4]);
    Humidity=(100.0*(double)hum/65535.0);           // RH = hum*100 / (2^16-1)

    if((Temperature>=-20&&(Temperature<=125))&&(Humidity>=0)&&(Humidity<=100)){
        *Tem_val = Temperature;
        *Hum_val = Humidity;
        return 0;
    }else{
        return 1;

    }
}

/* 描述：温湿度数据获取函数，单次获取
 * 参数Tem_val：存储温度数据的指针, 温度单位为°C
 * 参数Hum_val：存储湿度数据的指针, 温度单位为%
 * 返回值：0-读取成功，1-读取失败*/
uint8_t SHT30_Get_Humiture_single(double *Tem_val,double *Hum_val){
    uint8_t ret=0;
    uint8_t buff[6];
    uint16_t tem,hum;
    double Temperature=0;
    double Humidity=0;

    I2C_Start();
    SHT30_Send_Cmd(HIGH_ENABLED_CMD);
    I2C_Stop();

    delay_ms(50);

    I2C_Start();
    ret = SHT30_Read_Data(6,buff);
    I2C_Stop();

    if(CheckCrc8(buff, 0xFF) != buff[2] || CheckCrc8(&buff[3], 0xFF) != buff[5]){
        printf("CRC_ERROR,ret = 0x%x\r\n",ret);
		return 1;
    }

    /* 转换温度数据 */
	tem = (((uint16_t)buff[0]<<8) | buff[1]);//温度数据拼接
	Temperature= (175.0*(double)tem/65535.0-45.0) ;	// T = -45 + 175 * tem / (2^16-1)
	
	/* 转换湿度数据 */
	hum = (((uint16_t)buff[3]<<8) | buff[4]);//湿度数据拼接
	Humidity= (100.0*(double)hum/65535.0);			// RH = hum*100 / (2^16-1)

    if((Temperature>=-20)&&(Temperature<=125)&&(Humidity>=0)&&(Humidity<=100)){
        *Tem_val = Temperature;
        *Hum_val = Humidity;
        return 0;
    }else{
        return 1;
    }
}
