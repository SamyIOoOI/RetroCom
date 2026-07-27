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

#define ST_IDLE 0
#define ST_RINGING 1
#define ST_CALLING 2 
#define ST_ACCEPTED 3
#define ST_HANGUP 4
#define ST_BUSY 5

volatile unsigned char station_state = ST_IDLE;


volatile unsigned int timer = 0;
volatile unsigned int selected_station = 0;

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
void LCD_Nibble(unsigned char nibble, unsigned char iscontrol) {
    unsigned char data_value = (nibble & 0x0F) | (iscontrol ? 0x01 : 0x00) | 0x08;
    LCD_START();
    LCD_Write(I2C_DISPLAY_ADDR);
    LCD_Write(data_value | 0x04);
    I2C_Delay();
    LCD_Write(data_value & ~0x04);
    LCD_STOP();
}
void radio_registery(unsigned char upper, unsigned char lower){
    RADIO_Start();
    Radio_Write(I2C_RADIO_ADDR);
    Radio_Write(upper);
    Radio_Write(lower);
    Radio_Stop();
}
void cursor(unsigned char row, unsigned char col){
    unsigned char position;
    if (row == 1) {
        position = 0x80 + (col - 1);
    }
    else {
        position = 0xC0 + (col - 1);
    }
    LCD_CMD(position);
}
void LCD_Send(unsigned char value, unsigned char iscontrol) {
    LCD_Nibble((value & 0xF0), iscontrol);
    LCD_Nibble((value << 4) & 0xF0, iscontrol);
}
void LCD_CMD(unsigned char cmd){
    LCD_Send(cmd, 0);
}
void LCD_Letter(unsigned char letter){
    LCD_Send(letter, 1);
}
void LCD_Sentence(unsigned char *sentence){
    while(*sentence){
        LCD_Letter(*sentence++);
    }
}
void Init_Display(void){
    cursor(1, 3);
    LCD_Sentence("=RetroCom=");
    cursor(2, 1);
    LCD_Sentence("St:0 Radio:OFF");
    LCD_CMD(0x0F);
    cursor(2, 1);
}
void Radio_Process(unsigned char r2_high, unsigned char r2_low, unsigned char r3_high, unsigned char r3_low){
    Radio_Start();
    Radio_Write(I2C_RADIO_ADDR);
    Radio_Write(r2_high);
    Radio_Write(r2_low);
    Radio_Write(r3_high);
    Radio_Write(r3_low);
    Radio_Stop();
}
void Set_Radio(unsigned char mode){ /* Registered Radio Stations (I can expand this later) */
    if (mode == 0){
        Radio_Process(0x00, 0x00, 0x00, 0x00);
        cursor(2, 6);
        LCD_Sentence("Radio:OFF");
    }
    else if (mode == 1){
        Radio_Process(0xC0, 0X0F, 0X1C, 0X10);
        cursor(2, 6);
        LCD_Sentence("Radio:QUR");
    }
    else if (mode == 2){
        Radio_Process(0xC0, 0X0F, 0X22, 0X10);
        cursor(2, 6);
        LCD_Sentence("Radio:FM ")
    }
}
void Call_Active_Status(unsigned char number){
    Is_Call_Active = 1;
    LCD_CMD(0x0C);
    cursor(2, 1);
    if (number == 1) {
        LCD_Sentence("Call Active: 1  ");
    }
    else if (number == 2) {
        LCD_Sentence("Call Active: 2  ");
    }
    else if (number == 3) {
        LCD_Sentence("Call Active: 3  ");
    }
    else if (number == 4) {
        LCD_Sentence("Call Active: 4  ");
    }
    GREEN_LED = 1;
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
    Init_Display();

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
            if (current_state_scroll_btn == 0 && last_state_scroll_btn == 1) {
                /* Scroll Button Function */
            }
            last_state_scroll_btn = current_state_scroll_btn;
            if (current_state_call_btn == 0 && last_state_call_btn == 1) {
                if (station_state == ST_RINGING) {
                    station_state = ST_ACCEPTED;
                    BUZZER = 0;
                }
                /*Call Button Function */
            }
            last_state_call_btn = current_state_call_btn;
            if (current_state_hang_btn == 0 && last_state_hang_btn == 1) {
                if (station_state == ST_RINGING){
                    station_state = ST_HANGUP;
                    BUZZER = 0;
                } 
                /* Hang Button Function */
            }
            last_state_hang_btn = current_state_hang_btn;
            if (current_state_radio_btn1 == 0 && last_state_radio_btn1 == 1) {
                /* Radio Button 1 Function */
            }
            last_state_radio_btn1 = current_state_radio_btn1;
            if (current_state_radio_btn2 == 0 && last_state_radio_btn2 == 1) {
                /* Radio Button 2 Function */
            }
            last_state_radio_btn2 = current_state_radio_btn2;
        }
    }
}

