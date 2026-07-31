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
sbit ST_CD_A = P3^2;
sbit ST_CD_B = P3^3;



#define I2C_DISPLAY_ADDR 0x4E /*I gotta change this based on my I2C Address*/
#define I2C_RADIO_ADDR 0x40 /*I gotta change this based on my I2C Address*/
#define ST_IDLE 0
#define ST_RINGING 1
#define ST_CALLING 2 
#define ST_ACCEPTED 3
#define ST_HANGUP 4
#define ST_BUSY 5

volatile unsigned char station_state = ST_IDLE;
volatile unsigned int timer = 0;
volatile unsigned char poll = 0x00;
volatile unsigned int selected_station = 0;
volatile unsigned char recieved_message = 0;

bit call_button_pressed = 0;
bit Is_Speaker_On = 1;

/* Master Station Section (The state of all stations is stored here) */

bit station1_busy = 0;
bit station2_busy = 0;
bit station3_busy = 0;
bit station4_busy = 0;

unsigned char station1_calltrgt = 0;
unsigned char station2_calltrgt = 0;
unsigned char station3_calltrgt = 0;
unsigned char station4_calltrgt = 0;

unsigned char station1_calling = 0;
unsigned char station2_calling = 0;
unsigned char station3_calling = 0;
unsigned char station4_calling = 0;

bit station1_on = Is_Speaker_On;
bit station2_on = 1;
bit station3_on = 1;
bit station4_on = 1;

bit station1_rcall = 0;
bit station2_rcall = 0;
bit station3_rcall = 0;
bit station4_rcall = 0;

bit end_call_recieved1 = 0;
bit end_call_recieved2 = 0;
bit end_call_recieved3 = 0;
bit end_call_recieved4 = 0;

bit station1_decline = 0;
bit station2_decline = 0;
bit station3_decline = 0;
bit station4_decline = 0;

bit station1_trgtbusy = 0;
bit station2_trgtbusy = 0;
bit station3_trgtbusy = 0;
bit station4_trgtbusy = 0;

/*///*/

unsigned char currently_polling = 2;

void unpacker(void){
    if (currently_polling == 2){
        station2_on = (recieved_message & 0x01);
        station2_rcall = (recieved_message >> 1) & 0x01;
        station2_calltrgt = (recieved_message >> 2) & 0x03;
        end_call_recieved2 = (recieved_message >> 4) & 0x01;
        station2_busy = (recieved_message >> 5) & 0x01;
        station2_decline = (recieved_message >> 6) & 0x01;
        station2_trgtbusy = (recieved_message >> 7) &0x01;
    }
    if (currently_polling == 3){
        station3_on = (recieved_message & 0x01);
        station3_rcall = (recieved_message >> 1) & 0x01;
        station3_calltrgt = (recieved_message >> 2) & 0x03;
        end_call_recieved3 = (recieved_message >> 4) & 0x01;
        station3_busy = (recieved_message >> 5) & 0x01;
        station3_decline = (recieved_message >> 6) & 0x01;
        station3_trgtbusy = (recieved_message >> 7) & 0x01;
    }
    if (currently_polling == 4){
        station4_on = (recieved_message & 0x01);
        station4_rcall = (recieved_message >> 1) & 0x01;
        station4_calltrgt = (recieved_message >> 2) & 0x03;
        end_call_recieved4 = (recieved_message >> 4) & 0x01;
        station4_busy = (recieved_message >> 5) & 0x01;
        station4_decline = (recieved_message >> 6) & 0x01;
        station4_trgtbusy = (recieved_message >> 7) & 0x01;
    }
}
void pack_poll_byte(unsigned char call, unsigned char call_target, unsigned char end_call, unsigned char busy, unsigned char decline, unsigned char trgt_busy){
    poll =      
     (Is_Speaker_On & 0x01) | /* (this is the station's on/off flag) */
      ((call & 0x01) << 1) | /* whether I want to call the polled station or not */
       ((call_target & 0x03) << 2) | /* If I do wanna call, who? */
        ((end_call & 0x01) << 4) | /* Gives the polled station a hangup order */
        ((busy & 0x01) << 5)  | /* The Station's busy flag */
        ((decline & 0x01) << 6) | 
        ((trgt_busy & 0x01) << 7)
}
void checker(void){

}
void Poller(void){
    unsigned char call_1 = 0; 
    unsigned char call_target_1 = 0;
    unsigned char end_call_1 = 0;
    unsigned char busy_1 = 0;
    unsigned char decline_1 = 0;
    unsigned char target_busy_1 = 0;
    if (currently_polling == 2) {
        select_station_poll(2);
        if (station2_rcall){
            if (station2_calltrgt == 3) {
                if (station3_busy) {
                    target_busy_1 = 1;
                }
                if (station3_decline) {
                    decline_1 = 1;
                    station3_decline = 0;
                }
            
            }
            if (station2_calltrgt == 4){
                if (station4_busy) {
                    target_busy_1 = 1;
                }
                if (station4_decline) {
                    decline_1 = 1;
                    station4_decline = 0;
                }
            }
            if (station2_calltrgt == 1) {
                if (station1_busy) {
                    target_busy_1 = 1;
                }
                if (station1_decline) {
                    decline_1 = 1;
                    station1_decline = 0;
                }
            }
        }
        if (station3_calltrgt == 2 && station3_rcall) {
            call_1 = 1;
            call_target_1 = 3;
        }
        else if (station4_calltrgt == 2 && station4_rcall) {
            call_1 = 1;
            call_target_1 = 4;
        }
        else if (station1_calltrgt == 2 && station1_rcall) {
            call_1 = 1;
            call_target_1 = 1; 
        }
        if (station2_calling == 1 && end_call_recieved1){
            end_call_recieved1 = 0;
            station2_calling = 0;
            station1_calling = 0;
            station1_busy = 0;
            end_call_1 = 1;
            station1_calltrgt = 0;
            station1_rcall = 0;
            station2_busy = 0;
        }
        if (station2_calling == 3 && end_call_recieved3){
            end_call_recieved3 = 0;
            station3_calling = 0;
            station2_calling = 0;
            station3_busy = 0;
            end_call_1 = 1;
            station3_calltrgt = 0;
            station3_rcall = 0;
            station2_busy = 0;
        }
        if (station2_calling == 4 && end_call_recieved4){
            end_call_recieved4 = 0;
            station4_calling = 0;
            station2_calling = 0;
            station4_busy = 0;
            end_call_1 = 1;
            station4_calltrgt = 0;
            station4_rcall = 0;
            station2_busy = 0;
        }
    }
    busy_1 = station1_busy;
    pack_poll_byte(call_1, call_target_1, end_call_1, busy_1, decline_1, target_busy_1);
}
void end_call(unsigned char target){
    return; /*end call function*/
}
void select_station_poll(unsigned char station){
    if(station == 2) {
        ST_CD_A = 0;
        ST_CD_B = 0;
        currently_polling = 2;
    }
    else if (station == 3){
        ST_CD_A = 0;
        ST_CD_B = 1;
        currently_polling = 3;
    }
    else if (station == 4){
        ST_CD_A = 1;
        ST_CD_B = 0;
        currently_polling = 4;
    }
}
void UART_Send(unsigned char data){
    SBUF = data;
    while (!TI);
    TI = 0;
}
void UART_Read(unsigned int timeout_limit){
    unsigned int timer_count = 0;
    while (!RI){
        timer_count++;
        if (timer_count >= timeout_limit){
            return;
        }

    }
    RI = 0;
    recieved_message = SBUF;
}
void Init_UART(void){
    TMOD &= 0x0F;
    TMOD |= 0x20;
    TH1 = 0xFD;
    TL1 = 0xFD;
    TR1 = 1;
    SCON = 0x50;
    TI = 0;
    RI = 0;
}
void Timer0_ISR(void) interrupt 1
{
    TH0 = 0xFC;
    TL0 = 0x66;
    
    timer++;
}
void Init_System(void){
    TMOD &= 0xF0;
    TMOD |= 0x01;

    TH0 = 0xFC;
    TL0 = 0x66;

    ET0 = 1;
    ET1 = 0;
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
    ST_CD_A = 0;
    ST_CD_B = 0;
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
    station1_busy = 1;
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
    unsigned int x = 0;
    unsigned int y = 0;
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
    Init_UART();
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
                Is_Speaker_On = 1;
            }
            else if (SPK_SWITCH_STATUS == 1) {
                ORANGE_LED = 0;
                Is_Speaker_On = 0;
            }
            if (current_state_scroll_btn == 0 && last_state_scroll_btn == 1) {
                /* Scroll Button Function */
            }
            last_state_scroll_btn = current_state_scroll_btn;
            if (current_state_call_btn == 0 && last_state_call_btn == 1) {
                call_button_pressed = 1;
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
            else if (station_state == ST_CALLING){
                station_state = ST_IDLE;
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
        if ((timer - y) >= 200){
            y = timer;
        }
    }
}

