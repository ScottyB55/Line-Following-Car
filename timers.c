/* Scott Burnett
 *
 * Initializes timers for periodic time sequence, switch debouncing
 * PWM, PWM-control sequencing, and automatic ADC conversion
*/

#include  "functions.h"
#include  "msp430.h"
#include  "macros.h"

extern unsigned int sync_pwm_count = 0;
extern volatile unsigned int Time_Sequence = 0;
extern unsigned int event_Time_Sequence = FALSE;

extern unsigned int Run_Timer_Enabled = FALSE;
extern unsigned int run_time_decimal = 0;
extern unsigned int run_time_seconds = 0;

// See Timer0_B3 Registers in datasheet
void Init_Timer0_B3(void) {
  // See Pg 409 Family Users Guide
  
  TB0CTL = TBSSEL__SMCLK; // SMCLK source
  
  TB0CTL |= TBCLR;        // Resets TB0R, clock divider, count direction
  
  TB0CTL |= MC__CONTINOUS;// Continuous up (counts up to CNTL value, default 16 bit reset)
  TB0CTL |= ID__8;        // Divide input clock by 8
  
  TB0EX0 = TBIDEX__8;     // Divide input clock by an additional 8
  
  // Capture compare register (we reset the timer at 0, add intervals to this)
  TB0CCR0 = TB0CCR0_INTERVAL; // CCR0
  TB0CCTL0 |= CCIE;           // CCR0 enable interrupt
  
  TB0CCTL1 &= ~CCIE;  // Disable CCR1 enable interrupt, to be enabled in switches
  
  TB0CTL &= ~TBIE;  // Disable Overflow Interrupt
  TB0CTL &= ~TBIFG; // Clear Overflow Interrupt flag
}

// Configure Timer B3 for PWM
// Note how we defined LEFT_FORWARD_SPEED, etc
// See Timer3_B7 Registers in datasheet
// see macros for definitions
//#define WHEEL_PERIOD (8008)// 8000=(8e6)/1000hz
//#define WHEEL_MIDDLE (4000)// 

void Init_Timer3_B7_PWM(void) {
  // SMCLK source, up count mode, PWM Right Side
  // TB3.1 P6.0 R_FORWARD
  // TB3.2 P6.1 L_FORWARD
  // TB3.3 P6.2 R_REVERSE
  // TB3.4 P6.3 L_REVERSE
  
  // TB3 control 
  TB3CTL = TBSSEL__SMCLK; // SMCLK
  TB3CTL |= MC__UP;       // Up Mode (count to TB3CCR0)
  TB3CTL |= TBCLR;        // Clear TAR

  // Capture/compare 0 
  TB3CCR0 = WHEEL_PERIOD; // PWM Period

  // Capture/compare 1 thru 4
  RIGHT_FORWARD_SPEED = WHEEL_MIDDLE; // P6.0 Right Forward PWM duty cycle
  LEFT_FORWARD_SPEED = WHEEL_MIDDLE;  // P6.1 Left Forward PWM duty cycle
  RIGHT_REVERSE_SPEED = RIGHT_FORWARD_SPEED + 8; // P6.2 Right Reverse PWM duty cycle
  LEFT_REVERSE_SPEED = RIGHT_FORWARD_SPEED + 8; // P6.3 Left Reverse PWM duty cycle

  // Capture/compare control 1 thru 4
  TB3CCTL1 = OUTMOD_7; // CCR1 reset/set
  TB3CCTL2 = OUTMOD_7; // CCR2 reset/set
  TB3CCTL3 = OUTMOD_3; // CCR3 set/reset
  TB3CCTL4 = OUTMOD_3; // CCR4 set/reset
  
  // OUTMOD_7 // CCRx reset/set
  // The output is reset when the timer counts to the TAxCCRn value.
  // It is set when the timer counts to the TAxCCR0 value.
  
  // OUTMOD_3 // CCRx set/reset
  // The output is set when the timer counts to the TAxCCRn value.
  // It is reset when the timer counts to the TAxCCR0 value.
  
  // * we want to reset the OUTMOD_3s a hair in advance before we reach the TAxCCR0 value
  // In this configuration, the shoot through occurs Fwd set and Rev reset
}

void Enable_Timer3_B7_Sync_Interrupt(void) {
  // Enable rollover interrupt to allow syncing
  TB3CCTL0 |= CCIE;       // CCR0 enable interrupt
}

void Disable_Timer3_B7_Sync_Interrupt(void) {
  // Enable rollover interrupt to allow syncing
  TB3CCTL0 &= ~CCIE;       // CCR0 enable interrupt
}



// 50ms sample
// 20hz
#pragma vector = TIMER0_B0_VECTOR
__interrupt void Timer0_B0_ISR(void){
  // TimerB0 0 Interrupt handler
  TB0CCR0 += TB0CCR0_INTERVAL; // Add Offset to TBCCR0
  
  // Update the time sequence and signal a time sequence event
  // every 50ms (20hz)
  Time_Sequence++;
  event_Time_Sequence = TRUE;
  
  // TAKE THIS OUT
  // TEMPORARILY CONVERT
  /*if (Time_Sequence <= 1) {
    ADCCTL0 |= ADCENC; // Enable Conversions
    ADCCTL0 |= ADCSC;  // Start next conversion
  }*/
  
  // update display every 200ms (5hz)
  if (++update_display_count_timer >= 4) {
    update_display_count_timer = 0;
    display_change_ready = TRUE;  // Signal that we are ready to change the display
    
    if (Run_Timer_Enabled) {
      run_time_decimal += 2;
      if (run_time_decimal >= 10) {
        run_time_decimal = 0;
        run_time_seconds++;
        if (run_time_seconds >= 1000)
          run_time_seconds -= 1000;
      }
    }
  }
}

// Handles switch debounce timing for both switches
#pragma vector = TIMER0_B1_VECTOR
__interrupt void Timer0_B1_ISR(void){

  // Switch 1
  // @ end debounce time 
    // Update current switch state
    sw1_position = (P4IN & SW1) ? RELEASED : PRESSED;
    // If high, select high to low interrupt 1
    if (sw1_position)
      P4IES |= SW1;
    // If low, select low to high interrupt 0
    else 
      P4IES &= ~SW1;
    // Enable switch interrupt
    P4IE |= SW1;
    // Disable timer interrupt
    TB0CCTL1 &= ~CCIE;
    // Clear timer interrupt flag
    TB0CCTL1 &= ~CCIFG;
  
  // Switch 2
  // @ end debounce time 
    // Update current switch state
    sw2_position = (P2IN & SW2) ? RELEASED : PRESSED;
    // If high, select high to low interrupt 1
    if (sw2_position)
      P2IES |= SW2;
    // If low, select low to high interrupt 0
    else 
      P2IES &= ~SW2;
    // Enable switch interrupt
    P2IE |= SW2;
    // Disable timer interrupt  (already done)
    // Clear timer interrupt flag
}
// P1OUT ^= RED_LED;


// Syncing Interrupt:
// In datasheet (less specific than family user guide)
// see Interrupt Vector Addresses: Interrupt Sources, Flags, and Vectors
// It seems that interrupts with only one flag that triggers it will automatically
// reset the flag. If there are multiple flags that can trigger the interrupt, it
// won't reset the flags and we have to reset them
// 1000hz, in sync with the PWM
#pragma vector = TIMER3_B0_VECTOR
__interrupt void Timer3_B0_ISR(void){
  switch(++sync_pwm_count){
    case 4:
      // TODO: replace Following_Line with a variable about the ADC conversions
      if (P6OUT & LCD_IR) {
        ADCCTL0 |= ADCENC; // Enable Conversions
        ADCCTL0 |= ADCSC;  // Start next conversion
      }
      // Signal to retrieve the IR signals with the ADC
      // Turn off the IR sensor
      // Calculate the result
      break;
    case 5:
      // Update the pwm registers in sync with the PWM to avoid shoot through
      sync_pwm_count = 0;
      update_pwm_registers();
      break;
  }
  
  // Clear the interrupt flag (optional since this is the only flag that trips this interrupt)
  //TB3CCTL0 &= ~CCIFG;
}

// 10ms interval -> 20ms sample period -> 50hz  @ 30 lines / sec -> 3/5 lines per sample
// 5ms interval  -> 10ms sample period -> 100hz @ 30 lines / sec -> 1/3 lines per sample