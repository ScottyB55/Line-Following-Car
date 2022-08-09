/* Scott Burnett
 *
 * Creates switch pressed events and handles switch debouncing
 *
 * The switch interrupt initialzation is handled in ports.c
*/

#include "functions.h"
#include  "msp430.h"
#include  "macros.h"


extern unsigned int sw1_position = RELEASED;
extern unsigned int sw2_position = RELEASED;

extern unsigned int event_SW1_PRESSED = FALSE;
extern unsigned int event_SW2_PRESSED = FALSE;

// Set true in main to show we want to generate the change, bootleg
//extern unsigned int event_SWITCH_SHAPE = FALSE;
//extern unsigned int event_RUN_SHAPE = FALSE;


// Switch 1 Interrupt
#pragma vector=PORT4_VECTOR
__interrupt void switchP4_interrupt(void){
  // Switch 1 flag
  if (P4IFG & SW1) {
    // Clear interrupt flag
    P4IFG &= ~SW1;
    // Disable interrupt
    P4IE &= ~SW1;
    // Update current switch state
    sw1_position = (P4IN & SW1) ? RELEASED : PRESSED;
    // Start debounce time
      // Set time
      TB0CCR1 = TB0R + TB0CCR1_INTERVAL;
      // Start time (enable interrupt)
      TB0CCTL1 |= CCIE;
    
    // @ end debounce time 
      // Update current switch state
      // If high, select high to low interrupt 1
      // If low, select low to high interrupt 0
      // Enable interrupt
    
    // Generate events on switch press
    if (sw1_position == PRESSED)
      event_SW1_PRESSED = TRUE;
  }
}

// Switch 2 Interrupt
#pragma vector=PORT2_VECTOR
__interrupt void switchP2_interrupt(void){
  // Switch 2 flag
  if (P2IFG & SW2) {
    // Clear interrupt flag
    P2IFG &= ~SW2;
    // Disable interrupt
    P4IE &= ~SW2;
    // Update current switch state
    sw2_position = (P2IN & SW2) ? RELEASED : PRESSED;
    // Start debounce time
      // Set time
      TB0CCR1 = TB0R + TB0CCR1_INTERVAL;
      // Start time (enable interrupt)
      TB0CCTL1 |= CCIE;
    
    // @ end debounce time 
      // Update current switch state
      // If high, select high to low interrupt 1
      // If low, select low to high interrupt 0
      // Enable interrupt
    
    // Generate events on switch press
    if (sw2_position == PRESSED)
      event_SW2_PRESSED = TRUE;
  }
}

//P1OUT ^= RED_LED;
//event_SWITCH_SHAPE = TRUE;    // eventually abstract to sw1 and get with pointer
// If we run the came code on sw1 and sw2, we could abstract this with pointers
// rather than copy pasting
//event_RUN_SHAPE = TRUE;