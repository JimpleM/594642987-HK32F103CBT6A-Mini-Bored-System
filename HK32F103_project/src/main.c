#include "main.h"
#include "sys.h"
#include "delay.h"
#include "oled.h"
#include "bmp.h"
#include "ws2812.h"
#include "wifi.h"
#include "usart.h"
void OLED_Show(int num);
int main(void){
	delay_init();
    NVIC_Configuration();

    oled_init();
    RGB_LED_Init();
    oled_display_on();

    USART3_Init();
    wifi_protocol_init();
   
    int num = 9;
    while(1){
        wifi_uart_service();
        if(num == 9){
            mcu_set_wifi_mode(SMART_CONFIG);
        }
        switch(mcu_get_wifi_work_state()){
            case SMART_CONFIG_STATE: 
                num = 0;
                break;
			case AP_STATE:  
				num = 1;
			    break;
			case WIFI_NOT_CONNECTED:
                num = 2;
                break;
			case WIFI_CONNECTED:
                num = 3;
                break;
			default:break;
        }
        OLED_Show(num);
    }
}
void OLED_Show(int num){
    uint8_t i;
    uint8_t x;
    oled_user_clear();
    x=0;
    for(i=0; i<2; i++){
        oled_show_chinese(x,0,i);
        x+=16;
    }
    x=x-1;
    oled_show_char(x,0,'H',16);
    x=x+8;
    oled_show_char(x,0,'K',16);
    x=x+8;
    oled_show_char(x,0,' ',16);
    x=x+8;
    oled_show_char(x,0,'M',16);
    x=x+8;
    oled_show_char(x,0,'C',16);
    x=x+8;
    oled_show_char(x,0,'U',16);
    x=x+8;
    for(i=4;i<7;i++){
        oled_show_chinese(x,0,i);
        x+=16;
    }
    oled_show_number(0,3,num,1,16);
}
