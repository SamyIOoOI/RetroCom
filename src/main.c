/**
 * CPU: 89C52
 * Freq: 11.0592Mhz
*/

#include <reg52.h>
#include <stdio.h>
#include <intrins.h>

sbit CDBUS1 = P1^0;
sbit CDBUS2 = P1^1;
sbit CDBUS3 = P1^2;
sbit CALL_BTN = P1^3;
sbit HANG_BTN = P1^4;
sbit SCROLL_BTN = P1^5;
sbit RADIO_BTN1 = P1^6;
sbit RADIO_BTN2 = P3^4;
sbit BUZZER = P1^7;
sbit YELLOW_LED = P2^0;
sbit GREEN_LED = P2^1;
sbit SDA_DISPLAY = P2^2;
sbit SCL_DISPLAY = P2^3;
sbit RADIO_SCLK = P2^4;
sbit RADIO_SDIO = P2^5;
sbit SPK_SWITCH_STATUS = P2^6;
sbit ORANGE_LED = P2^7;
sbit RX_PIN = P3^0;
sbit TX_PIN = P3^1;

#define I2C_DISPLAY_ADDR 0x4E /*I gotta change this based on my I2C Address*/
#define I2C_RADIO_ADDR 0X40 /*I gotta change this based on my I2C Address*/

volatile unsigned int timer = 0;
bit Is_Call_Active = 0;

void Timer0_ISR(void) interrupt 1
{
    TH0 = 0XFC;
    TL0 = 0X66;
    
    timer++;
}
void Init_System(void){
    TMOD &= 0xF0;
    TMOD |= 0X01;

    TH0 = 0XFC;
    TL0 = 0X66;

    ET0 = 1;
    EA = 1;
    TR0 = 1;

    SPK_SWITCH_STATUS = 1;
    RX_PIN = 1;
    TX_PIN = 1;
    GREEN_LED = 0;
    YELLOW_LED = 0;
    CDBUS1 = 0;
    CDBUS2 = 0;
    CDBUS3 = 0;
    CALL_BTN = 1;
    RADIO_BTN1 = 1;
    RADIO_BTN2 = 1;
    SCROLL_BTN = 1;
    HANG_BTN = 1;
}
void LISTEN_STATION2(void){ /* Listens to port 1 */
    CDBUS1 = 0;
    CDBUS2 = 0;
    CDBUS3 = 1;
}
void LISTEN_STATION3(void){  /* port 2 */
    CDBUS1 = 0;
    CDBUS2 = 1;
    CDBUS3 = 0;
}
void LISTEN_STATION4(void){  /* Port 3 */
    CDBUS1 = 0;
    CDBUS2 = 1;
    CDBUS3 = 1;
}
void LISTEN_RADIO(void){ /* Port 4*/
    CDBUS1 = 1;
    CDBUS2 = 0;
    CDBUS3 = 0;
}
void LISTEN_NONE(void){ /* Port 0 */
    CDBUS1 = 0;
    CDBUS2 = 0;
    CDBUS3 = 0;
}
void I2C_Delay(void){
    unsigned char i = 10;
    while(i--);
}
void LCD_START(void){
    SDA_DISPLAY = 1; SCL_DISPLAY = 1; I2C_Delay();
    SDA_DISPLAY = 0; I2C_Delay();
    SCL_DISPLAY = 0; I2C_Delay();
}
void Radio_Start(void){
    RADIO_SDIO = 1; RADIO_SCLK = 1; I2C_Delay();
    RADIO_SDIO = 0; I2C_Delay();
    RADIO_SCLK = 0; I2C_Delay();
}
void LCD_STOP(void){
    SDA_DISPLAY = 0; I2C_Delay();
    SCL_DISPLAY = 1; I2C_Delay();
    SDA_DISPLAY = 1; I2C_Delay();
}
void Radio_Stop(void){
    RADIO_SDIO = 0; I2C_Delay();
    RADIO_SCLK = 1; I2C_Delay();
    RADIO_SDIO = 1; I2C_Delay();
}
void LCD_Write(unsigned char dat){
    unsigned char i;
    for(i = 0; i < 8; i++){
        SDA_DISPLAY = (dat & 0x80) ? 1 : 0;
        SCL_DISPLAY = 1; I2C_Delay();
        SCL_DISPLAY = 0; I2C_Delay();
        dat <<= 1;
    }
    SDA_DISPLAY = 1; SCL_DISPLAY = 1; I2C_Delay();
    SCL_DISPLAY = 0; I2C_Delay();
}
void Radio_Write(unsigned char dat){
    unsigned char x;
    for(x = 0; x < 8; x++){
        RADIO_SDIO = (dat & 0x80) ? 1 : 0;
        RADIO_SCLK = 1; I2C_Delay();
        RADIO_SCLK = 0; I2C_Delay();
        dat <<= 1;
    }
    RADIO_SDIO = 1; RADIO_SCLK = 1; I2C_Delay();
    RADIO_SCLK = 0; I2C_Delay();
}
void main(void)
{
    unsigned long x = 0;
    bit last_state_scroll_btn = 1;
    bit last_state_call_btn = 1;
    bit last_state_hang_btn = 1;
    bit last_state_radio_btn1 = 1;
    bit last_state_radio_btn2 = 1;
    bit current_state_scroll_btn = 1;
    bit current_state_call_btn = 1;
    bit current_state_hang_btn = 1;
    bit current_state_radio_btn1 = 1;
    bit current_state_radio_btn2 = 1;

    Init_System();

    while(1) {
        if ((timer - x) >= 20) {
            x = timer;
            current_state_scroll_btn = SCROLL_BTN;
            current_state_call_btn = CALL_BTN;
            current_state_hang_btn = HANG_BTN;
            current_state_radio_btn1 = RADIO_BTN1;
            current_state_radio_btn2 = RADIO_BTN2;
            if (SPK_SWITCH_STATUS == 0) {
                ORANGE_LED = 1;
            }
            else if (SPK_SWITCH_STATUS == 1) {
                ORANGE_LED = 0;
            }
            if current_state_scroll_btn == 0 && last_state_scroll_btn == 1 {
                /* Scroll Button Function */
            }
            last_state_scroll_btn = current_state_scroll_btn;
            if current_state_call_btn == 0 && last_state_call_btn == 1 {
                /* Call Button Function */
            }
            last_state_call_btn = current_state_call_btn;
            if current_state_hang_btn == 0 && last_state_hang_btn == 1 {
                /* Hang Button Function */
            }
            last_state_hang_btn = current_state_hang_btn;
            if current_state_radio_btn1 == 0 && last_state_radio_btn1 == 1 {
                /* Radio Button 1 Function */
            }
            last_state_radio_btn1 = current_state_radio_btn1;
            if current_state_radio_btn2 == 0 && last_state_radio_btn2 == 1 {
                /* Radio Button 2 Function */
            }
            last_state_radio_btn2 = current_state_radio_btn2;
        }
    }
}

