/**
 * CPU: 89C52
 * Freq: 11.0592Mhz
*/



/* /// Unmodified copy of station2.c. Not Usable until all "station2" specific variables are swapped. /// */

#include <reg52.h>
#include <stdio.h>
#include <intrins.h>

sbit CDBUS1 = P1^0;
sbit CDBUS2 = P1^1;
sbit CDBUS3 = P1^2;
sbit CALL_BTN = P1^3;
sbit HANG_BTN = P1^4;
sbit SCROLL_BTN = P1^5;
sbit BUZZER = P1^7;
sbit YELLOW_LED = P2^0;
sbit GREEN_LED = P2^1;
sbit SDA_DISPLAY = P2^2;
sbit SCL_DISPLAY = P2^3;
sbit SPK_SWITCH_STATUS = P2^6;
sbit ORANGE_LED = P2^7;
sbit RX_PIN = P3^0;
sbit TX_PIN = P3^1;

#define I2C_DISPLAY_ADDR 0x4E /*I gotta change this based on my I2C Address*/

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
void Side_Packer(void);
void Listen_Master(void);
void unpacker(void);
void pack_poll_byte(unsigned char call, unsigned char call_target, unsigned char end_call, unsigned char busy, unsigned char decline, unsigned char trgt_busy);
void UART_Send(unsigned char data);
unsigned char UART_Read(unsigned char timeout_limit);
void Init_UART(void);
void Init_System(void);
void Init_Display(void);
void LISTEN_STATION1(void);
void LISTEN_STATION3(void);
void LISTEN_STATION4(void);
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
bit declined_call = 0;
bit accepted_call = 0;
unsigned char Busy_Display_Clock = 0;
unsigned char who_station2_wants = 0;
unsigned char ring_start_time = 0;
unsigned char seconds = 0;
volatile unsigned int ms_time = 0;

bit call_request = 0;
bit end_call_recieved = 0;
bit end_this_call = 0;
bit decline_this_call = 0;
bit accept_this_call = 0;
bit station1_on = 0;
bit station1_busy = 0;
bit master_knocked = 0;
unsigned char calltrgt = 0;

unsigned char station1_calltrgt = 0;
unsigned char station2_calltrgt = 0;
unsigned char station3_calltrgt = 0;
unsigned char station4_calltrgt = 0;

unsigned char station1_calling = 0;
unsigned char station2_calling = 0;
unsigned char station3_calling = 0;
unsigned char station4_calling = 0;


bit station2_on = 1;
bit station1_rcall = 0;
bit station2_rcall = 0;
bit station2_busy = 0;

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
void checker(void){
    if (call_request){
        selected_station = calltrgt;
        call_request = 0;
        incoming();
    }
    if (station2_calling || station2_busy){
        if (end_call_recieved){
            endcall();
            station2_busy = 0;
            station2_calling = 0;
        }
    }
    if (outgoing_call || station2_rcall){
        if (declined_call){
            endcall();
            outgoing_call = 0;
            station2_rcall = 0;
            station2_busy = 0;
        }
        else if (accepted_call){
            calldisplay();
            if (who_station2_wants == 1){
                LISTEN_STATION1();
            }
            else if (who_station2_wants == 3){
                LISTEN_STATION3();
            }
            else if (who_station2_wants == 4){
                LISTEN_STATION4();
            }
            station2_busy = 1;
            outgoing_call = 0;
            incoming_call = 0;
        }
    }
}
void Listen_Master(void){
    station1_on = (recieved_message & 0x01);
    call_request = (recieved_message >> 1) & 0x01;
    calltrgt = ((recieved_message >> 2) & 0x03) + 1;
    end_call_recieved = (recieved_message >> 4) & 0x01;
    station1_busy = (recieved_message >> 5) & 0x01;
    declined_call = (recieved_message >> 6) & 0x01;
    accepted_call = (recieved_message >> 7) &0x01;
    Side_Packer();
    end_this_call = 0;
    decline_this_call = 0;
    accept_this_call = 0;
}
void Side_Packer(void){
    unsigned char call_1 = 0;
    unsigned char call_trgt_1 = 0;
    unsigned char end_call_1 = 0;
    unsigned char busy_1 = 0;
    unsigned char decline_1 = 0;
    unsigned char accept_1 = 0;
    if (station2_rcall){
        call_1 = 1;
        if (station2_calltrgt == 1){
            call_trgt_1 = 1;
        }
        else if (station2_calltrgt == 3){
            call_trgt_1 = 3;
        }
        else if (station2_calltrgt == 4){
            call_trgt_1 = 4;
        }
    }
    if (end_this_call){
        end_call_1 = 1;
    }
    if (station2_busy){
        busy_1 = 1;
    }
    if (decline_this_call){
        decline_1 = 1;
    }
    if (accept_this_call){
        accept_1 = 1;
    }
    pack_poll_byte(call_1, call_trgt_1, end_call_1, busy_1, decline_1, accept_1);
}
void pack_poll_byte(unsigned char call, unsigned char call_target, unsigned char end_call, unsigned char busy, unsigned char decline, unsigned char accept){
    unsigned char safe_target = 0;
    if (call_target > 0){
        safe_target = (call_target - 1) & 0x03;
    }
    poll =      
     (Is_Speaker_On & 0x01) | /* (this is the station's on/off flag) */
      ((call & 0x01) << 1) | /* whether I want to call or not */
       ( safe_target << 2) | /* If I do wanna call, who? */
        ((end_call & 0x01) << 4) | /* Hang Up */
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
    ES = 1;
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
    SCROLL_BTN = 1;
    HANG_BTN = 1;
}
void LISTEN_STATION1(void){ /* Listens to port 1 */
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
void Init_Display(void){
    cursor(1, 3);
    LCD_Sentence("=RetroCom=");
    cursor(2, 1);
    LCD_Sentence("St:0 Current:2");
    LCD_CMD(0x0E);
    cursor(2, 1);
}
void calldisplay(void){
    cursor(2, 1);
    GREEN_LED = 1;
    if (selected_station == 1){
        LCD_Sentence("CAL1");
    }
    else if (selected_station == 3){
        LCD_Sentence("CAL3");
    }
    else if (selected_station == 4){
        LCD_Sentence("CAL4");
    }
}
void callpending(void){
    cursor(2, 1);
    if (selected_station == 1){
        LCD_Sentence("C1..");
    }
    else if (selected_station == 3){
        LCD_Sentence("C3..");
    }
    else if (selected_station == 4){
        LCD_Sentence("C4..");
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
    station2_calltrgt = 0;
    station2_rcall = 0;
}
void incoming(void){
    incoming_call = 1;
    buzzer_flag = 1;
    YELLOW_LED = 1;
    ring_start_time = seconds;
    LISTEN_NONE();
    if (selected_station == 1){
        cursor(2, 1);
        LCD_Sentence("IN1?");
    }
    else if (selected_station == 3){
        cursor(2, 1);
        LCD_Sentence("IN3?");
    }
    else if (selected_station == 4){
        cursor(2, 1);
        LCD_Sentence("IN4?");
    }
}
void Serial_ISR(void) interrupt 4 {
    if (RI){
        RI = 0;
        recieved_message = SBUF;
        master_knocked = 1;
    }
}
void main(void)
{
    unsigned char x = 0;
    unsigned char buzzer_clock = 0;
    bit last_state_scroll_btn = 1;
    bit last_state_call_btn = 1;
    bit last_state_hang_btn = 1;
    bit current_state_scroll_btn = 1;
    bit current_state_call_btn = 1;
    bit current_state_hang_btn = 1;

    Init_System();
    Init_UART();
    Init_Display();

    while(1) {
        if (master_knocked){
            master_knocked = 0;
            Listen_Master();
            UART_Send(poll);
        }
        if ((unsigned char)(timer - x) >= 20) {
            x = timer;
            station2_on = Is_Speaker_On;
            current_state_scroll_btn = SCROLL_BTN;
            current_state_call_btn = CALL_BTN;
            current_state_hang_btn = HANG_BTN;
            if (SPK_SWITCH_STATUS == 0) {
                ORANGE_LED = 1;
                Is_Speaker_On = 1;
            }
            else if (SPK_SWITCH_STATUS == 1) {
                ORANGE_LED = 0;
                Is_Speaker_On = 0;
            }
            checker();
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
                if ((unsigned char)(timer - Busy_Display_Clock) >= 75){
                    Is_Display_Busy = 0;
                    cursor(2, 1);
                    LCD_Sentence("St:0");
                    selected_station = 0;
                }
            }
            if (current_state_scroll_btn == 0 && last_state_scroll_btn == 1 && !station2_busy && !incoming_call && !outgoing_call) {
                if (selected_mode == 0) {
                    if (selected_station == 0){
                        selected_station = 1;
                        cursor(2, 4);
                        LCD_Sentence("1");
                    }
                    else if (selected_station == 1){
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
                        selected_station = 1;
                        cursor(2, 4);
                        LCD_Sentence("1");
                    } 
                }
            }
            last_state_scroll_btn = current_state_scroll_btn;
            if (current_state_call_btn == 0 && last_state_call_btn == 1) {
                if (incoming_call){
                        station2_calling = selected_station;
                        who_station2_wants = selected_station; /*:beta wolf:*/
                        GREEN_LED = 1;
                        accept_this_call = 1;
                        buzzer_flag = 0;
                        YELLOW_LED = 0;
                        calldisplay();
                        if (who_station2_wants == 1){
                            LISTEN_STATION1();
                        }
                        else if (who_station2_wants == 3){
                            LISTEN_STATION3();
                        }
                        else if (who_station2_wants == 4){
                            LISTEN_STATION4();
                        }
                    }
                else if (!station2_busy && !incoming_call && !outgoing_call){
                    station2_rcall = 1;
                    station2_calltrgt = selected_station;
                    who_station2_wants = selected_station;
                    YELLOW_LED = 1;
                    outgoing_call = 1;
                    ring_start_time = seconds;
                    callpending();
                }
            }
            last_state_call_btn = current_state_call_btn;
            if (current_state_hang_btn == 0 && last_state_hang_btn == 1) {
                if (station2_busy || station2_calling) {
                    end_this_call = 1;
                    station2_busy = 0;
                    station2_calling = 0;
                    station2_calltrgt = 0;
                    station2_rcall = 0;
                    GREEN_LED = 0;
                    incoming_call = 0;
                    LISTEN_NONE();
                    endcall();
                    end_call_recieved = 0;
                }
                else if (incoming_call){
                    buzzer_flag = 0;
                    decline_this_call = 1;
                    station2_busy = 0;
                    station2_calling = 0;
                    station2_calltrgt = 0;
                    station2_rcall = 0;
                    GREEN_LED = 0;
                    YELLOW_LED = 0;
                    incoming_call = 0;
                    endcall();
                }
            }
            last_state_hang_btn = current_state_hang_btn;
        }
    }
}

