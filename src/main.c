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


void I2C_Delay(void);
void LCD_START(void);
void LCD_STOP(void);
void LCD_Write(unsigned char dat);
void LCD_Nibble(unsigned char nibble, unsigned char iscontrol);
void LCD_Send(unsigned char value, unsigned char iscontrol);
void LCD_CMD(unsigned char cmd);
void LCD_Letter(unsigned char letter);
void LCD_Sentence(unsigned char *sentence);
void cursor(unsigned char row, unsigned char col);
void Radio_Start(void);
void Radio_Stop(void);
void Radio_Write(unsigned char dat);
void radio_registery(unsigned char upper, unsigned char lower);
void Radio_Process(unsigned char r2_high, unsigned char r2_low, unsigned char r3_high, unsigned char r3_low);
void Set_Radio(unsigned char mode);
void select_station_poll(unsigned char station);
void Poller(void);
void unpacker(void);
void pack_poll_byte(unsigned char call, unsigned char call_target, unsigned char end_call, unsigned char busy, unsigned char decline, unsigned char trgt_busy);
void UART_Send(unsigned char data);
unsigned char UART_Read(unsigned char timeout_limit);
void Init_UART(void);
void Init_System(void);
void Init_Display(void);
void LISTEN_STATION2(void);
void LISTEN_STATION3(void);
void LISTEN_STATION4(void);
void LISTEN_RADIO(void);
void LISTEN_NONE(void);
void calldisplay(void);
void endcall(void);
void incoming(void);

volatile unsigned char timer = 0;
volatile unsigned char poll = 0x00;
volatile unsigned char selected_station = 2;
volatile unsigned char recieved_message = 0;

bit Is_Speaker_On = 1;
bit selected_mode = 0;
bit Is_Display_Busy = 0;
bit incoming_call = 0;
bit outgoing_call = 0;
bit buzzer_flag = 0;
unsigned char selected_radio_station = 0;
unsigned char Busy_Display_Clock = 0;
unsigned char who_main_wants = 0;
unsigned char ring_start_time = 0;
unsigned char seconds = 0;
volatile unsigned int ms_time = 0;

/* Master Station Section (The state of all stations is stored here) */

bit station1_busy = 0;
bit station2_busy = 0;
bit station3_busy = 0;
bit station4_busy = 0;
bit radio_on = 0;

unsigned char station1_calltrgt = 0;
unsigned char station2_calltrgt = 0;
unsigned char station3_calltrgt = 0;
unsigned char station4_calltrgt = 0;

unsigned char station1_calling = 0;
unsigned char station2_calling = 0;
unsigned char station3_calling = 0;
unsigned char station4_calling = 0;

bit station1_on = 1;
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
bit station1_accept = 0;
bit station2_accept = 0;
bit station3_accept = 0;
bit station4_accept = 0;

bit send_accept_station1 = 0;
bit send_accept_station2 = 0;
bit send_accept_station3 = 0;
bit send_accept_station4 = 0;


/*///*/
unsigned char currently_polling = 2;

void LCD_Nibble(unsigned char nibble, unsigned char iscontrol) {
    unsigned char data_value = (nibble & 0x0F) | (iscontrol ? 0x01 : 0x00) | 0x08;
    LCD_START();
    LCD_Write(I2C_DISPLAY_ADDR);
    LCD_Write(data_value | 0x04);
    I2C_Delay();
    LCD_Write(data_value & ~0x04);
    LCD_STOP();
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
void select_station_poll(unsigned char station){
    if(station == 2) {
        ST_CD_A = 0;
        ST_CD_B = 0;
        _nop_();
        _nop_();
    }
    else if (station == 3){
        ST_CD_A = 0;
        ST_CD_B = 1;
        _nop_();
        _nop_();
    }
    else if (station == 4){
        ST_CD_A = 1;
        ST_CD_B = 0;
        _nop_();
        _nop_();
    }
}
void Poller(void){
    unsigned char call_1 = 0; 
    unsigned char call_target_1 = 0;
    unsigned char end_call_1 = 0;
    unsigned char busy_1 = 0;
    unsigned char decline_1 = 0;
    unsigned char accept_1 = 0;
    if (currently_polling == 2) {
        select_station_poll(2);
        if (station2_rcall){
            if (station2_calltrgt == 3) {
                if (station3_busy) {
                    decline_1 = 1;
                }
                if (station3_decline) {
                    decline_1 = 1;
                    station3_decline = 0;
                }
            
            }
            if (station2_calltrgt == 4){
                if (station4_busy) {
                    decline_1 = 1;
                }
                if (station4_decline) {
                    decline_1 = 1;
                    station4_decline = 0;
                }
            }
            if (station2_calltrgt == 1) {
                if (station1_busy) {
                    decline_1 = 1;
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
        if (station3_rcall && (station3_calltrgt == 2) && station2_accept){
            station3_rcall = 0;
            station3_calling = 2;
            station2_calling = 3;
            station3_busy = 1;
            send_accept_station3 = 1;
        }
        else if (station4_rcall && (station4_calltrgt == 2) && station2_accept){
            station4_rcall = 0;
            station4_calling = 2;
            station2_calling = 4;
            station4_busy = 1;
            send_accept_station4 = 1;
        }
        else if (station1_rcall && (station1_calltrgt == 2) && station2_accept){
            station1_rcall = 0;
            station1_calling = 2;
            station2_calling = 1;
            station1_busy = 1;
            send_accept_station1 = 1;
        }
        if (send_accept_station2){
            send_accept_station2 = 0;
            accept_1 = 1;
            if (station2_calltrgt == 1){
                station1_busy = 1;
                incoming_call = 0;
            }
            else if (station2_calltrgt == 3){
                station3_busy = 1;
            }
            else if (station2_calltrgt == 4){
                station4_busy = 1;
            }
        }
    }
    if (currently_polling == 3) {
        select_station_poll(3);
        if (station3_rcall){
            if (station3_calltrgt == 2) {
                if (station2_busy) {
                    decline_1 = 1;
                }
                if (station2_decline) {
                    decline_1 = 1;
                    station2_decline = 0;
                }
            
            }
            if (station3_calltrgt == 4){
                if (station4_busy) {
                    decline_1 = 1;
                }
                if (station4_decline) {
                    decline_1 = 1;
                    station4_decline = 0;
                }
            }
            if (station3_calltrgt == 1) {
                if (station1_busy) {
                    decline_1 = 1;
                }
                if (station1_decline) {
                    decline_1 = 1;
                    station1_decline = 0;
                }
            }
        }
        if (station2_calltrgt == 3 && station2_rcall) {
            call_1 = 1;
            call_target_1 = 3;
        }
        else if (station4_calltrgt == 3 && station4_rcall) {
            call_1 = 1;
            call_target_1 = 4;
        }
        else if (station1_calltrgt == 3 && station1_rcall) {
            call_1 = 1;
            call_target_1 = 1; 
        }
        if (station3_calling == 1 && end_call_recieved1){
            end_call_recieved1 = 0;
            station3_calling = 0;
            station1_calling = 0;
            station1_busy = 0;
            end_call_1 = 1;
            station1_calltrgt = 0;
            station1_rcall = 0;
            station3_busy = 0;
        }
        if (station3_calling == 2 && end_call_recieved2){
            end_call_recieved2 = 0;
            station3_calling = 0;
            station2_calling = 0;
            station2_busy = 0;
            end_call_1 = 1;
            station2_calltrgt = 0;
            station2_rcall = 0;
            station3_busy = 0;
        }
        if (station3_calling == 4 && end_call_recieved4){
            end_call_recieved4 = 0;
            station4_calling = 0;
            station3_calling = 0;
            station4_busy = 0;
            end_call_1 = 1;
            station4_calltrgt = 0;
            station4_rcall = 0;
            station3_busy = 0;
        }
        if (station2_rcall && (station2_calltrgt == 3) && station3_accept){
            station2_rcall = 0;
            station2_calling = 3;
            station3_calling = 2;
            station2_busy = 1;
            send_accept_station2 = 1;
        }
        else if (station4_rcall && (station4_calltrgt == 3) && station3_accept){
            station4_rcall = 0;
            station4_calling = 3;
            station3_calling = 4;
            station4_busy = 1;
            send_accept_station4 = 1;
        }
        else if (station1_rcall && (station1_calltrgt == 3) && station3_accept){
            station1_rcall = 0;
            station1_calling = 3;
            station3_calling = 1;
            station1_busy = 1;
            send_accept_station1 = 1;
        }
        if (send_accept_station3){
            send_accept_station3 = 0;
            accept_1 = 1;
            if (station3_calltrgt == 1){
                station1_busy = 1;
                incoming_call = 0;
            }
            else if (station3_calltrgt == 2){
                station2_busy = 1;
            }
            else if (station3_calltrgt == 4){
                station4_busy = 1;
            }
        }
    }
    if (currently_polling == 4) {
        select_station_poll(4);
        if (station4_rcall){
            if (station4_calltrgt == 3) {
                if (station3_busy) {
                    decline_1 = 1;
                }
                if (station3_decline) {
                    decline_1 = 1;
                    station3_decline = 0;
                }
            
            }
            if (station4_calltrgt == 2){
                if (station2_busy) {
                    decline_1 = 1;
                }
                if (station2_decline) {
                    decline_1 = 1;
                    station2_decline = 0;
                }
            }
            if (station4_calltrgt == 1) {
                if (station1_busy) {
                    decline_1 = 1;
                }
                if (station1_decline) {
                    decline_1 = 1;
                    station1_decline = 0;
                }
            }
        }
        if (station3_calltrgt == 4 && station3_rcall) {
            call_1 = 1;
            call_target_1 = 3;
        }
        else if (station2_calltrgt == 4 && station2_rcall) {
            call_1 = 1;
            call_target_1 = 2;
        }
        else if (station1_calltrgt == 4 && station1_rcall) {
            call_1 = 1;
            call_target_1 = 1; 
        }
        if (station4_calling == 1 && end_call_recieved1){
            end_call_recieved1 = 0;
            station4_calling = 0;
            station1_calling = 0;
            station1_busy = 0;
            end_call_1 = 1;
            station1_calltrgt = 0;
            station1_rcall = 0;
            station4_busy = 0;
        }
        if (station4_calling == 3 && end_call_recieved3){
            end_call_recieved3 = 0;
            station3_calling = 0;
            station4_calling = 0;
            station3_busy = 0;
            end_call_1 = 1;
            station3_calltrgt = 0;
            station3_rcall = 0;
            station4_busy = 0;
        }
        if (station4_calling == 2 && end_call_recieved2){
            end_call_recieved2 = 0;
            station4_calling = 0;
            station2_calling = 0;
            station2_busy = 0;
            end_call_1 = 1;
            station2_calltrgt = 0;
            station2_rcall = 0;
            station4_busy = 0;
        }
        if (station3_rcall && (station3_calltrgt == 4) && station4_accept){
            station3_rcall = 0;
            station3_calling = 4;
            station4_calling = 3;
            station3_busy = 1;
            send_accept_station3 = 1;
        }
        else if (station2_rcall && (station2_calltrgt == 4) && station4_accept){
            station2_rcall = 0;
            station2_calling = 4;
            station4_calling = 2;
            station2_busy = 1;
            send_accept_station2 = 1;
        }
        else if (station1_rcall && (station1_calltrgt == 4) && station4_accept){
            station1_rcall = 0;
            station1_calling = 4;
            station4_calling = 1;
            station1_busy = 1;
            send_accept_station1 = 1;
        }
        if (send_accept_station4){
            send_accept_station4 = 0;
            accept_1 = 1;
            if (station4_calltrgt == 1){
                station1_busy = 1;
                incoming_call = 0;
            }
            else if (station4_calltrgt == 2){
                station2_busy = 1;
            }
            else if (station4_calltrgt == 3){
                station3_busy = 1;
            }
        }
    }
    busy_1 = station1_busy;
    pack_poll_byte(call_1, call_target_1, end_call_1, busy_1, decline_1, accept_1);
    UART_Send(poll);
    if (UART_Read(15)){
        unpacker();
    } else {
        if (currently_polling == 2) {
            station2_on = 0;
        }
        if (currently_polling == 3){
            station3_on = 0;
        }
        if (currently_polling == 4){
            station4_on = 0;
        }
    }
    if (currently_polling == 2){
        currently_polling = 3;
    }
    else if (currently_polling == 3){
        currently_polling = 4;
    }
    else if (currently_polling == 4){
        currently_polling = 2;
    }
}
void unpacker(void){
    if (currently_polling == 2){
        station2_on = (recieved_message & 0x01);
        station2_rcall = (recieved_message >> 1) & 0x01;
        station2_calltrgt = ((recieved_message >> 2) & 0x03) + 1;
        end_call_recieved2 = (recieved_message >> 4) & 0x01;
        station2_busy = (recieved_message >> 5) & 0x01;
        station2_decline = (recieved_message >> 6) & 0x01;
        station2_accept = (recieved_message >> 7) &0x01;
    }
    if (currently_polling == 3){
        station3_on = (recieved_message & 0x01);
        station3_rcall = (recieved_message >> 1) & 0x01;
        station3_calltrgt = ((recieved_message >> 2) & 0x03) + 1;
        end_call_recieved3 = (recieved_message >> 4) & 0x01;
        station3_busy = (recieved_message >> 5) & 0x01;
        station3_decline = (recieved_message >> 6) & 0x01;
        station3_accept = (recieved_message >> 7) & 0x01;
    }
    if (currently_polling == 4){
        station4_on = (recieved_message & 0x01);
        station4_rcall = (recieved_message >> 1) & 0x01;
        station4_calltrgt = ((recieved_message >> 2) & 0x03) + 1;
        end_call_recieved4 = (recieved_message >> 4) & 0x01;
        station4_busy = (recieved_message >> 5) & 0x01;
        station4_decline = (recieved_message >> 6) & 0x01;
        station4_accept = (recieved_message >> 7) & 0x01; /* This bit is just a placeholder for incoming traffic. Only Matters for outgoing orders. */
    }
}
void pack_poll_byte(unsigned char call, unsigned char call_target, unsigned char end_call, unsigned char busy, unsigned char decline, unsigned char accept){
    unsigned char safe_target = 0;
    if (call_target > 0){
        safe_target = (call_target - 1) & 0x03;
    }
    poll =      
     (Is_Speaker_On & 0x01) | /* (this is the station's on/off flag) */
      ((call & 0x01) << 1) | /* whether I want to call the polled station or not */
       ( safe_target << 2) | /* If I do wanna call, who? */
        ((end_call & 0x01) << 4) | /* Gives the polled station a hangup order */
        ((busy & 0x01) << 5)  | /* The Station's busy flag */
        ((decline & 0x01) << 6) | 
        ((accept & 0x01) << 7);
}
void UART_Send(unsigned char data){
    SBUF = data;
    while (!TI);
    TI = 0;
}
unsigned char UART_Read(unsigned char timeout_limit){
    unsigned char s_time = timer;
    while (!RI){
        if ((unsigned char)(timer - s_time) >= timeout_limit){
            return 0;
        }
    }
    RI = 0;
    recieved_message = SBUF;
    return 1;
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
    ms_time++; 
    if (ms_time >= 1000){
        ms_time = 0;
        seconds++;
    }
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
    unsigned char i = 20;
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
void radio_registery(unsigned char upper, unsigned char lower){
    Radio_Start();
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
        LCD_Sentence("Radio:FM ");
    }
}
void Init_Display(void){
    cursor(1, 3);
    LCD_Sentence("=RetroCom=");
    cursor(2, 1);
    LCD_Sentence("St:0 Radio:OFF");
    LCD_CMD(0x0E);
    cursor(2, 1);
}
void calldisplay(void){
    cursor(2, 1);
    GREEN_LED = 1;
    if (selected_station == 2){
        LCD_Sentence("CAL1");
    }
    else if (selected_station == 3){
        LCD_Sentence("CAL2");
    }
    else if (selected_station == 4){
        LCD_Sentence("CAL3");
    }
}
void callpending(void){
    cursor(2, 1);
    if (selected_station == 2){
        LCD_Sentence("C1..");
    }
    else if (selected_station == 3){
        LCD_Sentence("C2..");
    }
    else if (selected_station == 4){
        LCD_Sentence("C3..");
    } 
}
void endcall(void){
    LISTEN_NONE();
    cursor(2, 1);
    LCD_Sentence("HANG");
    Busy_Display_Clock = timer;
    Is_Display_Busy = 1;
    incoming_call = 0;
    outgoing_call = 0;
    station1_calltrgt = 0;
    station1_rcall = 0;
}
void incoming(void){
    incoming_call = 1;
    buzzer_flag = 1;
    YELLOW_LED = 1;
    ring_start_time = seconds;
    LISTEN_NONE();
    if (selected_station == 2){
        cursor(2, 1);
        LCD_Sentence("IN1?");
    }
    else if (selected_station == 3){
        cursor(2, 1);
        LCD_Sentence("IN2?");
    }
    else if (selected_station == 4){
        cursor(2, 1);
        LCD_Sentence("IN3?");
    }
}
void main(void)
{
    unsigned char x = 0;
    unsigned char y = 0;
    unsigned char busy_display_clock = 0;
    unsigned char buzzer_clock = 0;
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
        if ((unsigned char)(timer - x) >= 20) {
            x = timer;
            station1_on = Is_Speaker_On;
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
            if (buzzer_flag){
                if ((unsigned char)(timer - buzzer_clock) >= 300){
                    buzzer_clock = timer; 
                    BUZZER = !BUZZER;
                }
            }
            else if (!buzzer_flag){
                BUZZER = 0;
            }
            if (Is_Display_Busy){
                if ((unsigned char)(timer - busy_display_clock) >= 75){
                    Is_Display_Busy = 0;
                    cursor(2, 1);
                    LCD_Sentence("St:0");
                    selected_station = 0;
                }
            }
            if ((station1_calling == 2 && station2_calling == 1) || 
            (station1_calling == 3 && station3_calling == 1) || 
            (station1_calling == 4 && station4_calling == 1)){
                station1_busy = 1;
                YELLOW_LED = 0;
            }
            if (station1_calltrgt) {
                if (station1_calltrgt == 2){
                    if (station2_accept){
                        station1_calling = 2;
                        station2_calling = 1;
                        station1_busy = 1;
                        station1_calltrgt = 0;
                        YELLOW_LED = 0;
                        GREEN_LED = 1;
                        LISTEN_STATION2();
                        calldisplay();
                    }
                    else if (station2_decline){
                        station1_calltrgt = 0;
                        station2_decline = 0;
                        YELLOW_LED = 0;
                        endcall();
                    }
                }
                else if (station1_calltrgt == 3){
                    if (station3_accept){
                        station1_calling = 3;
                        station3_calling = 1;
                        station1_busy = 1;
                        station1_calltrgt = 0;
                        YELLOW_LED = 0;
                        GREEN_LED = 1;
                        LISTEN_STATION3();
                        calldisplay();
                    }
                    else if (station3_decline){
                        station1_calltrgt = 0;
                        station3_decline = 0;
                        YELLOW_LED = 0;
                        endcall();
                    }
                }
                else if (station1_calltrgt == 4){
                    if (station4_accept){
                        station1_calling = 4;
                        station4_calling = 1;
                        station1_busy = 1;
                        station1_calltrgt = 0;
                        YELLOW_LED = 0;
                        GREEN_LED = 1;
                        LISTEN_STATION4();
                        calldisplay();
                    }
                    else if (station4_decline){
                        station1_calltrgt = 0;
                        station4_decline = 0;
                        YELLOW_LED = 0;
                        endcall();
                    }
                }
            }
            if (station1_calling){
                if (station1_calling == 2){
                    if (end_call_recieved2){
                        end_call_recieved2 = 0;
                        station1_calling = 0;
                        station2_calling = 0;
                        station1_busy = 0;
                        endcall();
                    }
                }
                else if (station1_calling == 3){
                    if (end_call_recieved3){
                        end_call_recieved3 = 0;
                        station1_calling = 0;
                        station3_calling = 0;
                        station1_busy = 0;
                        endcall();
                    }
                }
                else if (station1_calling == 4){
                    if (end_call_recieved4){
                        end_call_recieved4 = 0;
                        station1_calling = 0;
                        station4_calling = 0;
                        station1_busy = 0;
                        endcall();
                    }
                }
            }
            if (station2_calltrgt == 1 && station2_rcall && !station1_busy && !incoming_call){
                selected_station = 2;
                incoming();
            }
            else if (station3_calltrgt == 1 && station3_rcall && !station1_busy && !incoming_call){
                selected_station = 3;
                incoming();    
            }
            else if (station4_calltrgt == 1 && station4_rcall && !station1_busy && !incoming_call){
                selected_station = 4;
                incoming();
            }
                /* If all stations call station 1 
                (or station x later when I copy this to
                 the other statios local code) 
                 Priority is 1>2>3>4 */
            if (((station2_calltrgt == 1 && station2_rcall) ||
             (station3_calltrgt == 1 && station3_rcall) ||
             (station4_calltrgt == 1 && station4_rcall)) 
             && station1_accept){
                if (who_main_wants == 2){
                    station1_calling = 2;
                    station1_accept = 0;
                    station2_calling = 1;
                    send_accept_station2 = 1;
                    calldisplay();
                    LISTEN_STATION2();
                }
                else if (who_main_wants == 3){
                    station1_calling = 3;
                    station1_accept = 0;
                    station3_calling = 1;
                    send_accept_station3 = 1;
                    calldisplay();
                    LISTEN_STATION3();
                }
                else if (who_main_wants == 4){
                    station1_calling = 4;
                    station1_accept = 0;
                    station4_calling = 1;
                    send_accept_station4 = 1;
                    calldisplay();
                    LISTEN_STATION4();
                }
             }
            if (send_accept_station1){
                send_accept_station1 = 0;
                station1_rcall = 0;
                station1_calling = station1_calltrgt;
                GREEN_LED = 1;
                if (station1_calltrgt == 2){
                    LISTEN_STATION2();
                    calldisplay();
                }
                else if (station1_calltrgt == 3){
                    LISTEN_STATION3();
                    calldisplay();
                }
                else if (station1_calltrgt == 4){
                    LISTEN_STATION4();
                    calldisplay();
                }
            }
            if (station1_rcall && (!station1_busy || !station1_calling ) ){
                if ((unsigned char)(seconds - ring_start_time) >= 45){
                    station1_rcall = 0;
                    station1_calltrgt = 0;
                    YELLOW_LED = 0;
                    endcall();
                }
            }
            if (incoming_call && (!station1_busy || !station1_calling)){
                if ((unsigned char)(seconds - ring_start_time) >= 45){
                    incoming_call = 0;
                    buzzer_flag = 0;
                    YELLOW_LED = 0;
                    endcall();
                }
            }
            if (current_state_scroll_btn == 0 && last_state_scroll_btn == 1 && !station1_busy && !incoming_call && !outgoing_call) {
                if (selected_mode == 0) {
                    if (selected_station == 0){
                        selected_station = 2;
                        cursor(2, 4);
                        LCD_Sentence("2");
                    }
                    else if (selected_station == 2){
                        selected_station = 3;
                        cursor(2, 4);
                        LCD_Sentence("3");
                    }
                    else if (selected_station == 3){
                        selected_station = 4;
                        cursor(2, 4);
                        LCD_Sentence("4");
                    }
                    else if (selected_station == 4){
                        selected_station = 2;
                        cursor(2, 4);
                        LCD_Sentence("2");
                    } 
                }
                else if (selected_mode == 1){
                    if (selected_radio_station == 1){
                        selected_radio_station = 2;
                        cursor(2, 12);
                        Set_Radio(2);
                    }
                    else if (selected_radio_station == 2){
                        selected_radio_station = 1;
                        cursor(2, 12);
                        Set_Radio(1);
                    }
                }
            }
            last_state_scroll_btn = current_state_scroll_btn;
            if (current_state_call_btn == 0 && last_state_call_btn == 1) {
                if (incoming_call){
                        station1_calling = selected_station;
                        who_main_wants = selected_station; /*:wolf:*/
                        GREEN_LED = 1;
                        station1_accept = 1;
                        buzzer_flag = 0;
                        YELLOW_LED = 0;
                        calldisplay();
                        if (who_main_wants == 2){
                            LISTEN_STATION2();
                        }
                        else if (who_main_wants == 3){
                            LISTEN_STATION3();
                        }
                        else if (who_main_wants == 4){
                            LISTEN_STATION4();
                        }
                    }
                else if (!station1_busy && !incoming_call && !outgoing_call){
                    station1_rcall = 1;
                    station1_calltrgt = selected_station;
                    YELLOW_LED = 1;
                    outgoing_call = 1;
                    ring_start_time = seconds;
                    callpending();
                }
            }
            last_state_call_btn = current_state_call_btn;
            if (current_state_hang_btn == 0 && last_state_hang_btn == 1) {
                if (station1_busy || station1_calling) {
                    end_call_recieved1 = 1;
                    station1_busy = 0;
                    station1_calling = 0;
                    station1_calltrgt = 0;
                    station1_rcall = 0;
                    GREEN_LED = 0;
                    incoming_call = 0;
                    station1_accept = 0;
                    LISTEN_NONE();
                    endcall();
                }
                else if (incoming_call){
                    buzzer_flag = 0;
                    station1_decline = 1;
                    station1_busy = 0;
                    station1_calling = 0;
                    station1_calltrgt = 0;
                    station1_rcall = 0;
                    GREEN_LED = 0;
                    YELLOW_LED = 0;
                    incoming_call = 0;
                    station1_accept = 0;    
                    endcall();
                }
            }
            last_state_hang_btn = current_state_hang_btn;
            if (current_state_radio_btn1 == 0 && last_state_radio_btn1 == 1) {
                if (!station1_busy){
                    if (radio_on) {
                        radio_on = 0;
                        Set_Radio(0);
                        LISTEN_NONE();
                        selected_radio_station = 0;
                        selected_mode = 0;
                    }
                    else if (!radio_on) {
                        radio_on = 1;
                        Set_Radio(1);
                        LISTEN_RADIO();
                        selected_radio_station = 1;
                    }
                }
            }
            last_state_radio_btn1 = current_state_radio_btn1;
            if (current_state_radio_btn2 == 0 && last_state_radio_btn2 == 1) {
                if (radio_on) {
                    selected_mode = !selected_mode; /* Currently scrolling Radio/Stations */
                }
            }
            last_state_radio_btn2 = current_state_radio_btn2;
        }
        if ((unsigned char)(timer - y) >= 50){
            y = timer;
            Poller();
        }
    }
}

