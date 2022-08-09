/* Scott Burnett
 *
 * Motor functions for enable and disable. Error checking.
*/

#include "functions.h"
#include "msp430.h"
#include "macros.h"

int motors_enabled = FALSE;


void Motors_Enable(void) {
  if (!motors_enabled) {
    motors_enabled = TRUE;
    
    // Enable the PWM timer
    Init_Timer3_B7_PWM();
    
    Config_Motor_Ports_PWM();
    
    // Enable the interrupt synchronous to the PWM
    // So that we update PWM registers in sync to avoid shootthrough
    Enable_Timer3_B7_Sync_Interrupt();
  }
}



/*
 * Function: Motors_Disable
 * --------------------
 * sets port select to GPIO
 * sets port output low
 * sets direction to output
 *
 *  returns: void
 */
void Motors_Disable(void) {
  //if (motors_enabled) {
    motors_enabled = FALSE;
    
    Config_Motor_Ports_GPIO();
    
    // Don't disable the sync interrupt in case we still want to read the ADC
  //}
}

/*
 * Function: Check_Error
 * --------------------
 * Checks if a forward and reverse are on at the same time
 *
 *  returns: void
 */
void Check_Error(void){
  // note bitwise & vs logical &&
  if ((P6IN & L_FORWARD) && (P6IN & L_REVERSE) ||
      (P6IN & R_FORWARD) && (P6IN & R_REVERSE)) {
          // P1OUT |= RED_LED;
          // Require two in a row to shut off
          if ((P6IN & L_FORWARD) && (P6IN & L_REVERSE) ||
            (P6IN & R_FORWARD) && (P6IN & R_REVERSE)) {
                int remember_TB3R = TB3R;
                int remember_P6IN = P6IN;
                
                Motors_Disable();
                strcpy(display_line[0], "ERROR");
                strcpy(display_line[1], "SHOOT");
                strcpy(display_line[2], "THROUGH");
                display_change_pending = TRUE;
                display_change_ready = TRUE;
                Display_Process();
                while(TRUE) {Motors_Disable();}
          }
  }
}
