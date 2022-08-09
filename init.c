/* Scott Burnett
 *
 * Initializations for the display and interrupt enable
*/

#include "functions.h"
#include "msp430.h"
#include "macros.h"

extern unsigned int initial_reset_complete = FALSE;

// Initialization
void Init_Conditions(void){
//------------------------------------------------------------------------------
  int i;
  for(i=0;i<11;i++){
    display_line[0][i] = RESET_STATE;
    display_line[1][i] = RESET_STATE;
    display_line[2][i] = RESET_STATE;
    display_line[3][i] = RESET_STATE;
  }
  display_line[0][10] = 0;
  display_line[1][10] = 0;
  display_line[2][10] = 0;
  display_line[3][10] = 0;

  display[0] = &display_line[0][0];
  display[1] = &display_line[1][0];
  display[2] = &display_line[2][0];
  display[3] = &display_line[3][0];
  //update_display = 0;


// Interrupts are disabled by default, enable them.
  enable_interrupts();
//------------------------------------------------------------------------------
}

//------------------------------------------------------------------------------
// System Configurations

// Tells the compiler to provide the value in reg as an input to an
// inline assembly block.  Even though the block contains no instructions,
// the compiler may not optimize it away, and is told that the value
// may change and should not be relied upon.

//inline void READ_AND_DISCARD(unsigned int reg) __attribute__((always_inline));
//void READ_AND_DISCARD(unsigned int reg){
//  asm volatile ("" : "=m" (reg) : "r" (reg));
//}

//inline void enable_interrupts(void) __attribute__((always_inline));
void enable_interrupts(void){
  __bis_SR_register(GIE);     // enable interrupts
//  asm volatile ("eint \n");
}

//inline void disable_interrupts(void) __attribute__((always_inline));
//void disable_interrupts(void){
//  asm volatile ("dint \n");
//}