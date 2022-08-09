/* Scott Burnett
 *
 * Initializations for ports
 * Initializations for the motor drive and infrared
*/


/*
All of the functions headers are defined in fuction.h
*/
#include "functions.h"
#include "msp430.h"
#include "macros.h"

void Init_Ports(char smclk, char ir_led_on){
  Init_Port1();
  Init_Port2(1);
  Init_Port3(smclk);
  Init_Port4(1);
  Init_Port5();
  Init_Port6(ir_led_on);
}

// Initializes all pins in port 1
void Init_Port1(){
  P1DIR = 0xFF; // Set P1 direction to output
  P1OUT = 0x00; // P1 set Low
  
  // Pin 0 GP I/O
  #define RED_LED (0x01)//1<<0
  P1SEL0 &= ~RED_LED; // Set RED_LED as GP I/O
  P1SEL1 &= ~RED_LED; // Set RED_LED as GP I/O
  P1OUT |= RED_LED; // Set Red LED On
  P1DIR |= RED_LED; // Set Red LED to output
  
  // Pin 1 FUNCTION
  #define A1_SEED (0x02)//1<<1
  P1SEL0 |= A1_SEED; // ADC input for A1_SEEED
  P1SEL1 |= A1_SEED; // ADC input for A1_SEEED
  // SEL bits allow disabling of standard I/O buffers
  
  // Pin 2 FUNCTION
  #define V_DETECT_L (0x04)//1<<2
  P1SEL0 |= V_DETECT_L; // ADC input for V_DETECT_L
  P1SEL1 |= V_DETECT_L; // ADC input for V_DETECT_L
  
  // Pin 3 FUNCTION
  #define V_DETECT_R (0x08)//1<<3
  P1SEL0 |= V_DETECT_R; // ADC input for V_DETECT_R
  P1SEL1 |= V_DETECT_R; // ADC input for V_DETECT_R
  
  // Pin 4 GP I/O
  // #define SMCLK_2476 (0x10)//1<<4
  #define V_BAT (0x10) // 4 A4
  P1SEL0 |= V_BAT; // ADC input for V_BAT
  P1SEL1 |= V_BAT; // ADC input for V_BAT
  /*
  // Pin 4 GP I/O
  #define SMCLK_2476 (0x10)//1<<4
  P1SEL0 &= ~SMCLK_2476;
  P1SEL1 &= ~SMCLK_2476;
  P1DIR &= ~SMCLK_2476;  // Set pin function INPUT
  */
  // Pin 5 FUNCTION
  #define V_THUMB (0x20)//1<<5
  P1SEL0 |= V_THUMB; // ADC input for V_THUMB
  P1SEL1 |= V_THUMB; // ADC input for V_THUMB
  
  // Pin 6 FUNCTION
  #define UCA0TXD (0x40)//1<<6
  P1SEL0 |= UCA0TXD; // UCA0TXD pin
  P1SEL1 &= ~UCA0TXD; // UCA0TXD pin
  
  // Pin 7 FUNCTION
  #define UCA0RXD (0x80)//1<<7
  P1SEL0 |= UCA0RXD; // UCA0RXD pin
  P1SEL1 &= ~UCA0RXD; // UCA0RXD pin
}

// Initializes all pins in port 2
void Init_Port2(char interrupt){
  // Pin 0 GP I/O
  #define RESET_LCD (0x01)////1<<0
  P2SEL0 &= ~RESET_LCD;
  P2SEL1 &= ~RESET_LCD;
  P2OUT &= ~RESET_LCD; // Initial value LOW
  P2DIR |= RESET_LCD;  // Set pin function OUTPUT
  
  // Pin 1 GP I/O
  #define L_REVERSE_2476 (0x02)//1<<1
  P2SEL0 &= ~L_REVERSE_2476;
  P2SEL1 &= ~L_REVERSE_2476;
  P2DIR &= ~L_REVERSE_2476;  // Set pin function INPUT
  
  // Pin 2 GP I/O
  #define P2_2 (0x04)//1<<2
  P2SEL0 &= ~P2_2;
  P2SEL1 &= ~P2_2;
  P2DIR &= ~P2_2;  // Set pin function INPUT
  
  // SWITCH 2
  
  // Pin 3 GP I/O, Pullup Resistor
  #define SW2 (0x08)//1<<3
  P2SEL0 &= ~SW2;
  P2SEL1 &= ~SW2;
  P2DIR &= ~SW2;  // Set pin function INPUT
  P2OUT |= SW2;   // Configure pullup resistor
  P2REN |= SW2;   // Enable pullup resistor
  
  switch (interrupt){
    case 0: break;
    default:
      P2IES |= SW2; // SW2 Hi to Lo edge interrupt
      // P2IES &= ~SW2; // SW2 Lo to Hi edge interrupt
      P2IFG &= ~SW2; // IFG SW1 cleared
      P2IE |= SW2; // SW2 interrupt Enabled
  }
  
  // Pin 4 GP I/O
  #define IOT_RUN_CPU (0x10)//1<<4
  P2SEL0 &= ~IOT_RUN_CPU;
  P2SEL1 &= ~IOT_RUN_CPU;
  P2OUT &= ~IOT_RUN_CPU;  // Initial value LOW
  P2DIR |= IOT_RUN_CPU;  // Set pin function OUTPUT
  
  // Pin 5 GP I/O
  #define DAC_ENB (0x20)//1<<5
  P2SEL0 &= ~DAC_ENB;
  P2SEL1 &= ~DAC_ENB;
  P2OUT |= DAC_ENB;  // Initial value HIGH
  P2DIR |= DAC_ENB;  // Set pin function OUTPUT
  
  // Pin 6 FUNCTION
  #define LFXOUT (0x40)//1<<6
  P2SEL0 &= ~LFXOUT;
  P2SEL1 |= LFXOUT;
  
  // Pin 7 FUNCTION
  #define LFXIN (0x80)//1<<7
  P2SEL0 &= ~LFXIN;
  P2SEL1 |= LFXIN;
}

// Initializes all pins in port 3
void Init_Port3(char smclk){
  // Pin 0 GP I/O
  #define TEST_PROBE (0x01)//1<<0
  P3SEL0 &= ~TEST_PROBE;
  P3SEL1 &= ~TEST_PROBE;
  P3OUT &= ~TEST_PROBE; // Initial value LOW
  P3DIR |= TEST_PROBE;  // Set pin function OUTPUT
  
  // Pin 1 FUNCTION
  #define DAC_CNTL1 (0x02)//1<<1
  P3SEL0 &= ~DAC_CNTL1;
  P3SEL1 &= ~DAC_CNTL1;
  P3OUT &= ~DAC_CNTL1; // Initial value LOW
  P3DIR |= DAC_CNTL1;  // Set pin function OUTPUT
  
  // Pin 2 FUNCTION
  #define OA2N (0x04)//1<<2
  P3SEL0 |= OA2N;
  P3SEL1 |= OA2N;
  
  // Pin 3 GP I/O
  #define LCD_BACKLITE (0x08)//1<<3
  P3SEL0 &= ~LCD_BACKLITE;
  P3SEL1 &= ~LCD_BACKLITE;
  //P3OUT &= ~LCD_BACKLITE; // Initial value LOW
  P3OUT |= LCD_BACKLITE; // Initial value HIGH
  P3DIR |= LCD_BACKLITE;  // Set pin function OUTPUT
  
  // Pin 4
  // @Param smclk : USE_SMCLK, USE_GPIO
  #define PIN3_4 (0x10)//1<<4
  switch(smclk){
    case USE_SMCLK:
      // Pin 4 FUNCTION (SMCLK_2355)
      P3SEL0 |= PIN3_4;
      P3SEL1 &= ~PIN3_4;
      P3DIR |= PIN3_4;  // Set pin function OUTPUT
      break;
    case USE_GPIO:
    default:
      // Pin 4 GP I/O
      P3SEL0 &= ~PIN3_4;
      P3SEL1 &= ~PIN3_4;
      P3OUT &= ~PIN3_4; // Initial value LOW
      P3DIR |= PIN3_4;  // Set pin function OUTPUT
  }
  
  // Pin 5 GPIO vs DAC
  //#define DAC_CNTL (0x20)//1<<5
  P3SEL0 &= ~DAC_CNTL;
  P3SEL1 &= ~DAC_CNTL;
  P3OUT |= DAC_CNTL; // Initial value HIGH
  P3DIR |= DAC_CNTL;  // Set pin function OUTPUT
  /*P3SELC |= DAC_CNTL;
  P3OUT &= ~DAC_CNTL1;
  P3DIR &= ~DAC_CNTL1;
  SAC3DAC |= DACEN;*/
  
  // Pin 6 GP I/O
  #define IOT_LINK_CPU (0x40)//1<<6
  P3SEL0 &= ~IOT_LINK_CPU;
  P3SEL1 &= ~IOT_LINK_CPU;
  P3OUT &= ~IOT_LINK_CPU; // Initial value LOW
  P3DIR |= IOT_LINK_CPU;// Set pin function OUTPUT
  
  // Pin 7 GP I/O
  P3SEL0 &= ~IOT_EN_CPU;
  P3SEL1 &= ~IOT_EN_CPU;
  P3OUT &= ~IOT_EN_CPU; // Initial value LOW
  P3DIR |= IOT_EN_CPU;  // Set pin function OUTPUT
}

// Initializes all pins in port 4
void Init_Port4(char interrupt){
  // Pin 0 GP I/O
  #define P4_0 (0x01)//1<<0
  P4SEL0 &= ~P4_0;
  P4SEL1 &= ~P4_0;
  P4DIR &= ~P4_0;  // Set pin function INPUT
  
  // SWITCH 1
  
  // Pin 1 GP I/O, Pullup Resistor
  #define SW1 (0x02)//1<<1
  P4SEL0 &= ~SW1;
  P4SEL1 &= ~SW1;
  P4DIR &= ~SW1;  // Set pin function INPUT
  P4OUT |= SW1;   // Configure pullup resistor
  P4REN |= SW1;   // Enable pullup resistor
  
  switch (interrupt){
    case 0: break;
    default:
      P4IES |= SW1; // SW1 Hi to Lo edge interrupt
      // P4IES &= ~SW1; // SW1 Lo to Hi edge interrupt
      P4IFG &= ~SW1; // IFG SW1 cleared
      P4IE |= SW1; // SW1 interrupt Enabled
  }
  
  // Pin 2 FUNCTION
  #define UCA1RXD (0x04)//1<<2
  P4SEL0 |= UCA1RXD;
  P4SEL1 &= ~UCA1RXD;
  
  // Pin 3 FUNCTION
  #define UCA1TXD (0x08)//1<<3
  P4SEL0 |= UCA1TXD;
  P4SEL1 &= ~UCA1TXD;
  
  // Pin 4 GP I/O
  #define UCB1_CS_LCD (0x10)//1<<4
  P4SEL0 &= ~UCB1_CS_LCD;
  P4SEL1 &= ~UCB1_CS_LCD;
  P4OUT |= UCB1_CS_LCD;  // Initial value HIGH
  P4DIR |= UCB1_CS_LCD;  // Set pin function OUTPUT
  
  // Pin 5 FUNCTION
  #define UCB1CLK (0x20)//1<<5
  P4SEL0 |= UCB1CLK;
  P4SEL1 &= ~UCB1CLK;
  
  // Pin 6 FUNCTION
  #define UCB1SIMO (0x40)//1<<6
  P4SEL0 |= UCB1SIMO;
  P4SEL1 &= ~UCB1SIMO;
  
  // Pin 7 FUNCTION
  #define UCB1SOMI (0x80)//1<<7
  P4SEL0 |= UCB1SOMI;
  P4SEL1 &= ~UCB1SOMI;
}

// Initializes all pins in port 5
void Init_Port5(){
  // Pin 0 FUNCTION
  #define CHECK_BAT (0x01)//1<<0
  P5SEL0 |= CHECK_BAT;
  P5SEL1 |= CHECK_BAT;
  
  // Pin 1 FUNCTION
  #define V_BATx (0x02)//1<<1
  P5SEL0 |= V_BATx;
  P5SEL1 |= V_BATx;
  
  // Pin 2 FUNCTION
  #define V_DAC (0x04)//1<<2
  P5SEL0 |= V_DAC;
  P5SEL1 |= V_DAC;
  
  // Pin 3 FUNCTION
  #define V_3_3 (0x08)//1<<3
  P5SEL0 |= V_3_3;
  P5SEL1 |= V_3_3;
  
  // Pin 4 GP I/O
  P5SEL0 &= ~IOT_BOOT_CPU;
  P5SEL1 &= ~IOT_BOOT_CPU;
  P5OUT |= IOT_BOOT_CPU; // Initial value HIGH
  P5DIR |= IOT_BOOT_CPU;  // Set pin function OUTPUT
}

// Initializes all pins in port 6
void Init_Port6(char ir_led_on){
  Config_Motor_Ports_GPIO();
  
    // Pin 4 GP I/O
  #define LCD_IR (0x10)//1<<4
  P6SEL0 &= ~LCD_IR;
  P6SEL1 &= ~LCD_IR;
  set_LCD_IR(ir_led_on);
  P6DIR |= LCD_IR;  // Set pin function OUTPUT
  
    // Pin 5 GP I/O
  #define P6_5 (0x20)//1<<5
  P6SEL0 &= ~P6_5;
  P6SEL1 &= ~P6_5;
  P6OUT &= ~P6_5; // Initial value LOW
  P6DIR |= P6_5;  // Set pin function OUTPUT
  
    // Pin 6 GP I/O
  #define GRN_LED (0x40)//1<<6
  P6SEL0 &= ~GRN_LED;
  P6SEL1 &= ~GRN_LED;
  P6OUT &= ~GRN_LED; // Initial value LOW
  P6DIR |= GRN_LED;  // Set pin function OUTPUT
}

void Config_Motor_Ports_PWM(void){
  
  // Pin 0 PWM
  #define R_FORWARD (0x01)//1<<0
  P6SEL0 |= R_FORWARD;
  P6SEL1 &= ~R_FORWARD;
  P6OUT &= ~R_FORWARD; // Initial value LOW
  P6DIR |= R_FORWARD;  // Set pin function OUTPUT
  
  // Pin 1 PWM
  #define L_FORWARD (0x02)//1<<1
  P6SEL0 |= L_FORWARD;
  P6SEL1 &= ~L_FORWARD;
  P6OUT &= ~L_FORWARD; // Initial value LOW
  P6DIR |= L_FORWARD;  // Set pin function OUTPUT
  
    // Pin 2 PWM
  #define R_REVERSE (0x04)//1<<2
  P6SEL0 |= R_REVERSE;
  P6SEL1 &= ~R_REVERSE;
  P6OUT &= ~R_REVERSE; // Initial value LOW
  P6DIR |= R_REVERSE;  // Set pin function OUTPUT
  
    // Pin 3 PWM
  #define L_REVERSE (0x08)//1<<3
  P6SEL0 |= L_REVERSE;
  P6SEL1 &= ~L_REVERSE;
  P6OUT &= ~L_REVERSE; // Initial value LOW
  P6DIR |= L_REVERSE;  // Set pin function OUTPUT

}
void Config_Motor_Ports_GPIO(void){
  
  // GP I/O
  P6SEL0 &= ~R_FORWARD;
  P6SEL1 &= ~R_FORWARD;
  P6SEL0 &= ~L_FORWARD;
  P6SEL1 &= ~L_FORWARD;
  P6SEL0 &= ~R_REVERSE;
  P6SEL1 &= ~R_REVERSE;
  P6SEL0 &= ~L_REVERSE;
  P6SEL1 &= ~L_REVERSE;
  
  // OFF
  P6OUT &= ~R_FORWARD;
  P6OUT &= ~L_FORWARD;
  P6OUT &= ~R_REVERSE;
  P6OUT &= ~L_REVERSE;  
  
  // Set pin function OUTPUT
  P6DIR |= R_FORWARD;
  P6DIR |= L_FORWARD;
  P6DIR |= R_REVERSE;
  P6DIR |= L_REVERSE;
  
}

// If the LCD IR is set, on timers triggers the ADC in sync with the PWM
void set_LCD_IR(char ir_led_on){
  if (ir_led_on)
    P6OUT |= LCD_IR; // Initial value HIGH
  else
    P6OUT &= ~LCD_IR; // Initial value LOW
}