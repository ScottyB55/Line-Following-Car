/* Scott Burnett
 *
 * Not used. Saves a record of the most recent ADC readings, with analytics
*/

#include "functions.h"
#include "msp430.h"
#include "macros.h"

// ADC Logging Black Line Values
extern unsigned int Log_Current_NextIndex = 0;
extern unsigned int Log_Historic_NextIndex = HISTORIC_LOG_WIDTH;

extern unsigned int Current_Interval_Sum = 0;
extern unsigned int Historic_Interval_Sum = 0;

// Global. Set to 0 by defualt
extern unsigned int Log[LOG_WIDTH] = {0};
// Could also use memset(Log, 0, LOG_WIDTH);

extern unsigned int data_valid = FALSE;
extern unsigned int event_Hit_Black_Line = FALSE;


// Store the history of detected IR values in a memory circle
void Update_History(void){

  // Log_Current_NextIndex points to the location where the new current set of readings will be loaded
  //                                  that exits from the historic interval and enters the current interval
  Historic_Interval_Sum -= Log[Log_Current_NextIndex]; // (Old)
  Log[Log_Current_NextIndex] = ADC_Left_Detect + ADC_Right_Detect;
  Current_Interval_Sum += Log[Log_Current_NextIndex]; // (new)

  // (Log_index - 10 + 20) % 20 points to the location
  //                                  that exits from the current interval and enters the historic interval
  Log_Historic_NextIndex = (Log_Current_NextIndex + HISTORIC_LOG_WIDTH) % 20;
  Current_Interval_Sum -= Log[Log_Historic_NextIndex];
  Historic_Interval_Sum += Log[Log_Historic_NextIndex];

  if (data_valid && (Current_Interval_Sum > Historic_Interval_Sum + 250)) {
    event_Hit_Black_Line = TRUE;
  }

  // Recycle the history storage

  Log_Current_NextIndex++;
  if (Log_Current_NextIndex >= LOG_WIDTH) {
    Log_Current_NextIndex = 0;
    // signal that the data is completley valid now
    data_valid = TRUE;
  }
}