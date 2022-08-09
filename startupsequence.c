/* Scott Burnett
 * 
 * State machine to handle the startup of the IOT
 * 
 * Call the Update_Startup_Sequence, passing a string if there is any received
 * data or just passing null in the event of a time sequence
*/

#include "functions.h"
#include "msp430.h"
#include "macros.h"

#define IOT_RESET_DELAY (4)
#define IOT_RESET_STATE (6)

extern int IOT_Startup_State = IOT_RESET_STATE;
int time_sequence_delay;
char IP_address_1[11];
char IP_address_2[11];

void Update_Startup_Sequence(char * iot_rx_string){
  switch (IOT_Startup_State) {
  case 0:
    if (iot_rx_string == NULL)
      return;
    if (strcmp(iot_rx_string, "WIFI DISCONNECT") == 0)
      IOT_Startup_State = IOT_RESET_STATE;
    break;
    
  case IOT_RESET_STATE: // Send reset pulse and set up delay
    set_Link_Enable(FALSE);
    set_IOT_EN(RESET);
    time_sequence_delay = Time_Sequence + IOT_RESET_DELAY;
    IOT_Startup_State--;
    break;
      
  case 5: // once the reset delay is over, enable the IOT to run
    if (Time_Sequence >= time_sequence_delay) {
      IOT_Startup_State--;
      set_Link_Enable(TRUE);
      set_IOT_EN(SET);
    }
    break;
      
  case 4: // wait for the final ready signal, then get the IP address
    if (strcmp(iot_rx_string, "WIFI GOT IP") == 0) {
      Transmit_To_IOT("AT+CIFSR\r\n");
      IOT_Startup_State--;
    }
    break;
  
  case 3: // wait for us to receive the IP address
    if (iot_rx_string[10] == 'I') {
      // Get the second half of the IP address
      // Hard coded string parsing
      strcpy(IP_address_2+2, iot_rx_string + 20);
      IP_address_2[0] = ' ';
      IP_address_2[1] = ' ';
      IP_address_2[8] = ' ';
      IP_address_2[9] = ' ';
      iot_rx_string[20] = NULL;
      strcpy(IP_address_1+1, iot_rx_string + 13);
      IP_address_1[0] = ' ';
      IP_address_1[1] = ' ';
      IP_address_1[8] = ' ';
      IP_address_1[9] = ' ';
      IOT_Startup_State--;
    }
    break;
  
  case 2: // wait for us to receive the IP address
    if (strcmp(iot_rx_string, "OK") == 0) {
      Transmit_To_IOT("AT+CIPMUX=1\r\n");
      IOT_Startup_State--;
    }
    break;
      
  case 1: // wait for OK signal, then send another command
    if (strcmp(iot_rx_string, "OK") == 0) {
      // Port 8888
      Transmit_To_IOT("AT+CIPSERVER=1,8888\r\n");
      IOT_Startup_State--;
    }
    break;
  } 
  //Set_Display_Char(3, 9, '0' + IOT_Startup_State);
}