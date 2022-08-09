/* Scott Burnett
 *
 * Project 10
 * 
 * Receive commands from IOT to navigate a course and follow a line
*/

#include "functions.h" // covers ports.c and ports.h, etc
#include "msp430.h"
#include "macros.h"
#include <stdlib.h>

// Should actually replace this with a get and set function

void Handle_Switch_Presses(void);
void Handle_PC_Rx(void);
void Handle_IOT_Rx(void);

// For some reason we can't use the extern keywords for char arrays
// Maybe this is good practice in general
// https://stackoverflow.com/questions/7670816/create-extern-char-array-in-c
char pc_rx_string[];
char iot_rx_string[];

#define COMMAND_ARR_LENGTH (6)
char last_received_command[COMMAND_ARR_LENGTH];


void Main_Init(void) {
  // Initializations
  PM5CTL0 &= ~LOCKLPM5;

  Init_Ports(/*USE_SMCLK*/USE_GPIO, 0);// Initialize Ports, with SMCLK
  Init_Clocks();                       // Initialize Clock System
  
  Init_Conditions();                   // Initialize Variables and Initial Conditions
  Init_LCD();                          // Initialize LCD
  //lcd_BIG_mid();                     // Big font
  
  P6OUT &= ~LCD_IR;  // IR LED Off
  P1OUT &= ~RED_LED; // Red LED off
  
  Motors_Disable();  //gpio rather than pwm out, a lot of these calls are already true by default
  
  // DAC
  Init_REF();
  Init_DAC();
  
  Init_Timer0_B3();// initialize timer for time sequence, display update, and switch debounce
  
  // Setup the ADC
  Setup_ADC();
  
  // Get the IR readings flowing
  Init_Timer3_B7_PWM();
  Enable_Timer3_B7_Sync_Interrupt();
  set_LCD_IR(TRUE);
  
  set_Link_Enable(FALSE);
  set_IOT_EN(RESET);
  // Will be set by the first timer event
  
  // Initialize communications
  Init_Serial_UCA0();
  Init_Serial_UCA1();
}

void main(void){
  Main_Init();
  
  // https://cboard.cprogramming.com/c-programming/60525-malloc-strings-vs-array-strings.html
  //char * test_tx_string = "HELLO\n";
  
  // char chararray[10];
  // test_rx_string = chararray;
  // test_rx_string = (char *) malloc(sizeof(char) * 10);
  // Malloc doesn't work here (returns null for error) why is this??
  
  // Operating System
  while(ALWAYS) {
    
    Handle_IOT_Rx();
    
    Handle_PC_Rx();
    
    Handle_Switch_Presses();
    
    Handle_Time_Sequence();
    
    if (flag_ADC_Conv_Complete) {
      flag_ADC_Conv_Complete = FALSE;
      State_Machine_ADC_Complete();
    }
    
    Check_Error();
    
  }
}

// Functions that execute if we entered a new time sequence
// 20hz
void Handle_Time_Sequence(void) {
  if(event_Time_Sequence){
    event_Time_Sequence = FALSE;
    // Address who new time sequences may concern
    Display_Process();
    Update_Startup_Sequence(NULL);
    Update_State_Machine(NULL);
    
    //Set_Display_Int(2, 0, common_mode_error);
    //Set_Display_Char(2, 9, State);
    //Set_Display_Int(3, 0, ADC_Right_Detect);
    //Set_Display_Int(3, 5, ADC_Left_Detect);
  }
}

void Handle_IOT_Rx(void) {
  
  if (flag_iot_rx_command) {
    flag_iot_rx_command = FALSE;
    flag_iot_rx_end = FALSE;
    
    Get_IOT_Command(iot_rx_string);
    
    last_received_command[0] = iot_rx_string[0];
    last_received_command[1] = iot_rx_string[1];
    last_received_command[2] = iot_rx_string[2];
    last_received_command[3] = iot_rx_string[3];
    last_received_command[4] = iot_rx_string[4];
    
    //if (black_line_executing)
    //  black_line_executing = 2;
      
    // Address who iot rx commands may concern
    //Set_Display_String(0, 0, iot_rx_string);
    Update_State_Machine(iot_rx_string);
  }
  
  if (flag_iot_rx_end) {
    flag_iot_rx_command = FALSE;
    flag_iot_rx_end = FALSE;
    
    Get_IOT_Received(iot_rx_string);      
    
    // Address who iot rx non-commands may concern
    //Set_Display_String(1, 0, iot_rx_string);
    Update_Startup_Sequence(iot_rx_string);
  }
}

void Handle_PC_Rx(void) {
  if (flag_pc_rx_end) {
    flag_pc_rx_end = FALSE;
    
    Get_PC_Received(pc_rx_string);
    
    // Address who pc rx may concern
    //Set_Display_String(2, 0, pc_rx_string);
  }
}

void Handle_Switch_Presses(void) {
  if (event_SW1_PRESSED || event_SW2_PRESSED){
    event_SW1_PRESSED = FALSE;
    event_SW2_PRESSED = FALSE;
    // Address who switch presses may concern
  }
}