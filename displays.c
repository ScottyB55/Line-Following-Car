/* Scott Burnett
 *
 * Functions for displaying on the LCD and LED
*/

// An example of commenting notation:
// 2/10/22
// Scott Burnett
// MSP430 7.21.1
// Defines xxx global variables
// Description of file and contents
// Description of file and contents


// For Each Function
// Globals used or changed, passed variables, local variables, returned values
// Various options for each port pin and what can be configured
// List of ports being configured b the function, what they relate to,
// (General I/O, switches, etc)
// Description of function and contents


/*
All of the functions headers are defined in fuction.h

//extern char display_line[4][11];
//extern char *display[4];

*/
#include "functions.h"
#include "msp430.h"
#include "macros.h"

extern unsigned volatile int display_change_pending = FALSE;
extern unsigned volatile int display_change_ready = FALSE;

extern unsigned int update_display_count_timer = 0;

/*
 * Function: Reset_Display_Buffer
 * --------------------
 * Loads a preset string into the display buffer
 * Signals a change in the display
 *
 *  returns: void
 */

void Set_Display_String(unsigned int row, unsigned int col, char * str_ptr) {
  while ((*str_ptr != NULL) && (col < 10)) {
    display_line[row][col] = *str_ptr;
    str_ptr++;
    col++;
  }
  while (col < 10) {
    display_line[row][col] = ' ';
    col++;
  }
  // strcpy(display_line[i], str_ptr);
  display_change_pending = TRUE;
}

void Set_Display_Char(unsigned int row, unsigned int col, char set_char) {
  display_line[row][col] = set_char;
  display_change_pending = TRUE;
}

void Set_Display_Int(unsigned int row, unsigned int col, int disp_int) {
  if(disp_int < 0){
    disp_int = -disp_int;
    display_line[row][col] = '-';
  }
  else
    display_line[row][col] = '+';
  
  disp_int = disp_int % 10000;
  
  int value = disp_int / 1000;
  display_line[row][col+1] = '0' + value;
  disp_int -= value * 1000;
  
  value = disp_int / 100;
  display_line[row][col+2] = '0' + value;
  disp_int -= value * 100;
  
  value = disp_int / 10;
  display_line[row][col+3] = '0' + value;
  disp_int -= value * 10;
  
  display_line[row][col+4] = '0' + disp_int;
  
  display_change_pending = TRUE;
}

void Display_Time(unsigned int row, unsigned int col, int disp_int) {
  int value = disp_int / 100;
  display_line[row][col] = '0' + value;
  disp_int -= value * 100;
  
  value = disp_int / 10;
  display_line[row][col+1] = '0' + value;
  disp_int -= value * 10;
  
  display_line[row][col+2] = '0' + disp_int;
  
  display_line[row][col+3] = 's';
  
  display_change_pending = TRUE;
}

/*
 * Function: Init_LEDs
 * --------------------
 * Turns off both LEDs (should actually use a get/set or on/off function)
 *
 *  returns: void
 */
void Init_LEDs(void){
  P1OUT &= ~RED_LED;
  P6OUT &= ~GRN_LED;
}

// I should actually fill out these functions!
void IR_LED_control(char selection){}
void Backlite_control(char selection){}

/*
 * Function: Display_Process
 * --------------------
 * Checks to see if there's a pending change and the display is ready to be updated
 * Updates the display if this is true
 *
 *  returns: void
 */
void Display_Process(void){
  display_change_pending = TRUE;
  if(display_change_pending && display_change_ready){
    display_change_pending = FALSE;
    display_change_ready = FALSE;
    Display_By_State();
    Display_Update(0,0,0,0);
  }
}

//void Display_By_State(void)
// See statemachine.c
