/*
 * Description: This file contains the function prototypes
 * --------------------
 * Scott Burnett
 * ECE 306 Spring 2022
 * 
 * Check earlier versions for additional .r43 functions
 */

#include <string.h>


// adc.c
void Setup_ADC(void);
// DAC
void Init_REF(void);
void Init_DAC(void);
// #pragma vector=ADC_VECTOR


// clocks.c
void Init_Clocks(void);
void Software_Trim(void);


// controlsystem.c
void perform_control_calculations(void);
void velocity2pwm(int velocity, int velocity_diff);
void update_pwm_registers(void);


// datalog.c
void Update_History(void);


// comm.c
void Transmit_To_PC_Char(char character);
void Transmit_To_PC(char * string_pointer);
void Transmit_To_IOT_Char(char character);
void Transmit_To_IOT(char * string_pointer);
void Get_PC_Received(char * string_pointer);
void Get_IOT_Received(char * string_pointer);
void Get_IOT_Command(char * string_pointer);
void Init_Serial_UCA0(void);
void Init_Serial_UCA1(void);
void set_IOT_EN(int value);
void set_Link_Enable(int value);


// displays.c
void Set_Display_String(unsigned int row, unsigned int col, char * str_ptr);
void Set_Display_Char(unsigned int row, unsigned int col, char set_char);
void Set_Display_Int(unsigned int row, unsigned int col, int disp_int);
void Init_LEDs(void);
void IR_LED_control(char selection);
void Backlite_control(char selection);
void Display_Process(void);


// init.c
void Init_Conditions(void);
void enable_interrupts(void);


// main.c
void main(void);
void Handle_Time_Sequence(void);


// movement.c
void straight(void);
void stop(void);
void Movement_Off(void);
void Check_Error(void);


// ports.c
void Init_Ports(char smclk, char ir_led_on);
void Init_Port1();
void Init_Port2(char interrupt);
void Init_Port3(char smclk);
void Init_Port4(char interrupt);
void Init_Port5();
void Init_Port6(char ir_led_on);
void Config_Motor_Ports_PWM(void);
void Config_Motor_Ports_GPIO(void);
void set_LCD_IR(char ir_led_on);


// startupsequence.c
void Update_Startup_Sequence(char * iot_rx_string);


// statemachine.c
void Set_Time_Delay(char * iot_rx_string);
void Update_State_Machine(char * iot_rx_string);
void State_Machine_ADC_Complete(void);
void Display_By_State(void);


// switches.c
// #pragma vector=PORT4_VECTOR
// #pragma vector=PORT2_VECTOR


// timers.c
void Init_Timer0_B3(void);
void Init_Timer3_B7_PWM(void);

void Enable_Timer3_B7_Sync_Interrupt(void);
void Disable_Timer3_B7_Sync_Interrupt(void);
  
// #pragma vector = TIMER0_B0_VECTOR
// #pragma vector = TIMER0_B1_VECTOR
// #pragma vector = TIMER3_B0_VECTOR



// LCD.r43
// LCD
void Display_Process(void);
void Display_Update(char p_L1,char p_L2,char p_L3,char p_L4);
void enable_display_update(void);
void update_string(char *string_data, int string);
void Init_LCD(void);
void lcd_clear(void);
void lcd_putc(char c);
void lcd_puts(char *s);

void lcd_power_on(void);
void lcd_write_line1(void);
void lcd_write_line2(void);
//void lcd_draw_time_page(void);
//void lcd_power_off(void);
void lcd_enter_sleep(void);
void lcd_exit_sleep(void);
//void lcd_write(unsigned char c);
//void out_lcd(unsigned char c);

void Write_LCD_Ins(char instruction);
void Write_LCD_Data(char data);
void ClrDisplay(void);
void ClrDisplay_Buffer_0(void);
void ClrDisplay_Buffer_1(void);
void ClrDisplay_Buffer_2(void);
void ClrDisplay_Buffer_3(void);

void SetPostion(char pos);
void DisplayOnOff(char data);
void lcd_BIG_mid(void);
void lcd_BIG_bot(void);
void lcd_120(void);

void lcd_4line(void);
void lcd_out(char *s, char line, char position);
void lcd_rotate(char view);

//void lcd_write(char data, char command);
void lcd_write(unsigned char c);
void lcd_write_line1(void);
void lcd_write_line2(void);
void lcd_write_line3(void);

void lcd_command( char data);
void LCD_test(void);
void LCD_iot_meassage_print(int nema_index);



// SPI
void Init_SPI_B1(void);
void SPI_B1_write(char byte);
void spi_rs_data(void);
void spi_rs_command(void);
void spi_LCD_idle(void);
void spi_LCD_active(void);
void SPI_test(void);
void WriteIns(char instruction);
void WriteData(char data);

