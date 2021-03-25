#include "oled.h"
#include "stdlib.h"
#include "oledfont.h"    
#include "delay.h"

static void i2c_start(void);
static void i2c_stop(void);
static void i2c_wait_ack(void);
static void oled_write_byte(unsigned dat, unsigned cmd);
static void write_i2c_byte(uint8_t IIC_Byte);

static void i2c_start(void)
{
    oled_sck_set();
    oled_sda_set();
    oled_sda_reset();
    oled_sck_reset();
}

static void i2c_stop(void)
{
    oled_sck_set();
    oled_sda_reset();
    oled_sda_set();
}

static void i2c_wait_ack(void)
{
    oled_sck_set();
    oled_sck_reset();
}

static void write_i2c_byte(uint8_t IIC_Byte)
{
    uint8_t i;
    uint8_t m, da;
    da = IIC_Byte;
    oled_sck_reset();
    for (i = 0; i < 8; i++)
    {
        m = da;
        m = m & 0x80;
        if (m == 0x80)
        {
            oled_sda_set();
        }
        else
            oled_sda_reset();
        da = da << 1;
        oled_sck_set();
        oled_sck_reset();
    }
}

//---------------------------------------------------

static void oled_write_byte(unsigned dat, unsigned cmd)
{
    uint8_t wr_con = cmd ? 0x40 : 0x00;

    i2c_start();
    write_i2c_byte(0x78);
    i2c_wait_ack();
    write_i2c_byte(wr_con);
    i2c_wait_ack();
    write_i2c_byte(dat);
    i2c_wait_ack();
    i2c_stop();
}

/********************************************
 // fill_Picture
 ********************************************/
void fill_picture(uint8_t fill_Data)
{
    uint8_t m, n;
    for (m = 0; m < 8; m++)
    {
        oled_write_byte(0xb0 + m, OLED_CMD);       	//page0-page1
        oled_write_byte(0x00, OLED_CMD);     				//low column start address
        oled_write_byte(0x10, OLED_CMD);     				//high column start address
        for (n = 0; n < 128; n++)
        {
            oled_write_byte(fill_Data, 1);
        }
    }
}

//坐标设置
void oled_set_pos(uint8_t x, uint8_t y)
{
    oled_write_byte(0xb0 + y, OLED_CMD);
    oled_write_byte(((x & 0xf0) >> 4) | 0x10, OLED_CMD);
    oled_write_byte((x & 0x0f), OLED_CMD);
}
//开启OLED显示    
void oled_display_on(void)
{
    oled_write_byte(0X8D, OLED_CMD);  //SET DCDC命令
    oled_write_byte(0X14, OLED_CMD);  //DCDC ON
    oled_write_byte(0XAF, OLED_CMD);  //DISPLAY ON
}
//关闭OLED显示     
void oled_display_off(void)
{
    oled_write_byte(0X8D, OLED_CMD);  //SET DCDC命令
    oled_write_byte(0X10, OLED_CMD);  //DCDC OFF
    oled_write_byte(0XAE, OLED_CMD);  //DISPLAY OFF
}
//清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!    
void oled_clear(void)
{
    uint8_t i, n;
    for (i = 0; i < 8; i++)
    {
        oled_write_byte(0xb0 + i, OLED_CMD);    //设置页地址（0~7）
        oled_write_byte(0x00, OLED_CMD);      //设置显示位置—列低地址
        oled_write_byte(0x10, OLED_CMD);      //设置显示位置—列高地址   
        for (n = 0; n < 128; n++)
        {
            oled_write_byte(0, OLED_DATA);
        }
    } //更新显示
}

void oled_user_clear(void)
{
    uint8_t i, n;
    for (i = 2; i < 8; i++)
    {
        oled_write_byte(0xb0 + i, OLED_CMD);    //设置页地址（0~7）
        oled_write_byte(0x00, OLED_CMD);      //设置显示位置—列低地址
        oled_write_byte(0x10, OLED_CMD);      //设置显示位置—列高地址   
        for (n = 0; n < 128; n++)
        {
            oled_write_byte(0, OLED_DATA);
        }
    } //更新显示    
}
void OLED_On(void)
{
    uint8_t i, n;
    for (i = 0; i < 8; i++)
    {
        oled_write_byte(0xb0 + i, OLED_CMD);    //设置页地址（0~7）
        oled_write_byte(0x00, OLED_CMD);      //设置显示位置—列低地址
        oled_write_byte(0x10, OLED_CMD);      //设置显示位置—列高地址   
        for (n = 0; n < 128; n++)
            oled_write_byte(1, OLED_DATA);
    } //更新显示
}

//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//mode:0,反白显示;1,正常显示                 
//size:选择字体 16/12 
void oled_show_char(uint8_t x, uint8_t y, uint8_t chr, uint8_t Char_Size)
{
    uint8_t c = 0, i = 0;
    c = chr - ' ';  //得到偏移后的值           
    if (x > Max_Column - 1)
    {
        x = 0;
        y = y + 2;
    }
    if (Char_Size == 16)
    {
        oled_set_pos(x, y);
        for (i = 0; i < 8; i++)
            oled_write_byte(F8X16[c * 16 + i], OLED_DATA);
        oled_set_pos(x, y + 1);
        for (i = 0; i < 8; i++)
            oled_write_byte(F8X16[c * 16 + i + 8], OLED_DATA);
    }
    else
    {
        oled_set_pos(x, y);
        for (i = 0; i < 6; i++)
            oled_write_byte(F6x8[c][i], OLED_DATA);

    }
}
//m^n函数
uint32_t oled_pow(uint8_t m, uint8_t n)
{
    uint32_t result = 1;
    while (n--)
        result *= m;
    return result;
}


//显示2个数字
//x,y :起点坐标  
//len :数字的位数
//size:字体大小
//mode:模式 0,填充模式;1,叠加模式
//num:数值(0~4294967295);             
void oled_show_number(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t size2)
{
    uint8_t t, temp;
    uint8_t enshow = 0;
    for (t = 0; t < len; t++)
    {
        temp = (num / oled_pow(10, len - t - 1)) % 10;
        if (enshow == 0 && t < (len - 1))
        {
            if (temp == 0)
            {
                oled_show_char(x + (size2 / 2) * t, y, ' ', size2);
                continue;
            }
            else
						{
                enshow = 1;
						}

        }
        oled_show_char(x + (size2 / 2) * t, y, temp + '0', size2);
    }
}

//显示整数，包含正负
void oled_show_int_number(uint8_t x, uint8_t y, int num, uint8_t Char_Size)
{
    uint8_t len = 0;
    int buf = num;
    while (buf)
    {
        len++;
        buf /= 10;
    }
    if (num > 0)
        oled_show_number(x, y, num, len, Char_Size);
    else
    {
        oled_show_char(x, y, '-', Char_Size);
        num = abs(num);
        if (Char_Size == 16)
            oled_show_number(x + 8, y, num, len, Char_Size);
        else
            oled_show_number(x + 6, y, num, len, Char_Size);
    }
}

/*
 描述：显示整数(可显示正负数)（-65535~65535）
 输入：
 x:显示的x起始位置
 y:显示的y起始位置
 num:要显示的数字
 Char_Size:要显示数字的大小
 局限：目前只能显示确定范围的小数，且小数部分显示不准确
 */
void oled_show_float_number(uint8_t x, uint8_t y, float num, uint8_t Char_Size)
{
    uint8_t zheng_len = 0, xiao_len = 0, polarity_flag;
    int zheng = 0, xiao = 0;
    if (num > 0)
        polarity_flag = 1;
    else
    {
        polarity_flag = 0;
        num = -num;
    }
    zheng = (int) num;
    xiao = ((num - zheng) * 100) / 1;   //显示小数点后两位
    while (zheng)    //计算整数部分位数
    {
        zheng_len++;
        zheng /= 10;
    }
    xiao_len = 2;
    zheng = (int) num; //在计算整数部分数值时，原赋值被篡改，这里重新赋值
    if (polarity_flag)
    {
        if (Char_Size == 16)
        {
            oled_show_number(x, y, zheng, zheng_len, Char_Size);
            oled_show_char(x + zheng_len * 8, y, '.', Char_Size);
            oled_show_number(x + (zheng_len + 1) * 8, y, xiao, xiao_len, Char_Size);
        }
        else
        {
            oled_show_number(x, y, zheng, zheng_len, Char_Size);
            oled_show_char(x + zheng_len * 6, y, '.', Char_Size);
            oled_show_number(x + (zheng_len + 1) * 6, y, xiao, xiao_len, Char_Size);
        }
    }
    else
    {
        oled_show_char(x, y, '-', Char_Size);
        if (Char_Size == 16)
        {
            oled_show_number(x + 8, y, zheng, zheng_len, Char_Size);
            oled_show_char(x + (zheng_len + 1) * 8, y, '.', Char_Size);
            oled_show_number(x + (zheng_len + 2) * 8, y, xiao, xiao_len, Char_Size);
        }
        else
        {
            oled_show_number(x + 6, y, zheng, zheng_len, Char_Size);
            oled_show_char(x + (zheng_len + 1) * 6, y, '.', Char_Size);
            oled_show_number(x + (zheng_len + 2) * 6, y, xiao, xiao_len, Char_Size);
        }
    }
}

//显示一个字符号串
void oled_show_string(uint8_t x, uint8_t y, uint8_t *chr, uint8_t Char_Size)
{
    uint8_t j = 0;
    while (chr[j] != '\0')
    {
        oled_show_char(x, y, chr[j], Char_Size);
        x += 8;
        if (x > 120)
        {
            x = 0;
            y += 2;
        }
        j++;
    }
}
//显示汉字
void oled_show_chinese(uint8_t x, uint8_t y, uint8_t no)
{
    uint8_t t, adder = 0;
    oled_set_pos(x, y);
    for (t = 0; t < 16; t++)
    {
        oled_write_byte(Hzk[2 * no][t], OLED_DATA);
        adder += 1;
    }
    oled_set_pos(x, y + 1);
    for (t = 0; t < 16; t++)
    {
        oled_write_byte(Hzk[2 * no + 1][t], OLED_DATA);
        adder += 1;
    }
}
/***********功能描述：显示显示BMP图片128×64起始点坐标(x,y),x的范围0～127，y为页的范围0～7*****************/
void oled_draw_bmp(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t BMP[])
{
    unsigned int j = 0;
    uint8_t x, y;

    if (y1 % 8 == 0)
        y = y1 / 8;
    else
        y = y1 / 8 + 1;
    for (y = y0; y < y1; y++)
    {
        oled_set_pos(x0, y);
        for (x = x0; x < x1; x++)
        {
            oled_write_byte(BMP[j++], OLED_DATA);
        }
    }
}

//初始化SSD1306                     
void oled_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructer;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitStructer.GPIO_Pin=GPIO_Pin_8 | GPIO_Pin_9; //6--SCL   7--SDA
    GPIO_InitStructer.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_InitStructer.GPIO_Mode=GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructer);

    delay_ms(200);
    oled_write_byte(0xAE, OLED_CMD); //--display off
    oled_write_byte(0x00, OLED_CMD); //---set low column address
    oled_write_byte(0x10, OLED_CMD); //---set high column address
    oled_write_byte(0x40, OLED_CMD); //--set start line address  
    oled_write_byte(0xB0, OLED_CMD); //--set page address
    oled_write_byte(0x81, OLED_CMD); // contract control
    oled_write_byte(0xFF, OLED_CMD); //--128   
    oled_write_byte(0xA1, OLED_CMD); //set segment remap 
    oled_write_byte(0xA6, OLED_CMD); //--normal / reverse
    oled_write_byte(0xA8, OLED_CMD); //--set multiplex ratio(1 to 64)
    oled_write_byte(0x3F, OLED_CMD); //--1/32 duty
    oled_write_byte(0xC8, OLED_CMD); //Com scan direction
    oled_write_byte(0xD3, OLED_CMD); //-set display offset
    oled_write_byte(0x00, OLED_CMD); //

    oled_write_byte(0xD5, OLED_CMD); //set osc division
    oled_write_byte(0x80, OLED_CMD); //

    oled_write_byte(0xD8, OLED_CMD); //set area color mode off
    oled_write_byte(0x05, OLED_CMD); //

    oled_write_byte(0xD9, OLED_CMD); //Set Pre-Charge Period
    oled_write_byte(0xF1, OLED_CMD); //

    oled_write_byte(0xDA, OLED_CMD); //set com pin configuartion
    oled_write_byte(0x12, OLED_CMD); //

    oled_write_byte(0xDB, OLED_CMD); //set Vcomh
    oled_write_byte(0x30, OLED_CMD); //

    oled_write_byte(0x8D, OLED_CMD); //set charge pump enable
    oled_write_byte(0x14, OLED_CMD); //

    oled_write_byte(0xAF, OLED_CMD); //--turn on oled panel
}


void oled_temp_humi_display(float temp, float Humi)
{
	int i = 0;
    oled_clear(); //清屏函数     
    for (i = 0; i < 7; i++)
    {
        oled_show_chinese(i * 18, 0, i);
    }
    
    oled_show_chinese(6, 3, 4);
    oled_show_chinese(24, 3, 6);
    oled_show_char(42, 3, ':', 16);
    oled_show_float_number(52, 3, temp, 16);
    oled_show_string(100, 3, "'C", 16);
    
    oled_show_chinese(6, 6, 4);
    oled_show_chinese(24, 6, 6);
    oled_show_char(42, 6, ':', 16);
    oled_show_float_number(52, 6, Humi, 16);
    oled_show_string(100, 6, "%", 16);
}

