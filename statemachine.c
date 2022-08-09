/* Scott Burnett
 *
 * State machine for the remote execution of commands.
 * Has a queue of commands enabling the execution of
 * multiple commands in a row.
 * 
 * Call Update_State_Machine upon new input data or time sequence
 * State_Machine_ADC_Complete is periodically called, allowing 
 * states to have periodic functionality (for a control system)
*/

#include "functions.h"
#include "msp430.h"
#include "macros.h"

// Default to the last shape in the sequence


// Some general state ideas
// Motor_State (driving or off)
// IR_LED_State (on or off)
// Line_Following (on of off)


int first_number;
extern char State = '*';
int time_sequence_delay_sm;

char next_commands[S_RING_SIZE + S_RING_SIZE];

char state_name[6];
char * state_name_ptr;

//char bl_step_name[11];
char * bl_step_name_ptr;

extern char pad_num_char = '0';

extern int black_line_executing = 0;


// One section handles the output

// Another section handles the transitions
  // Transitions include initizliations that must happen once 


void Get_First_Number(char * iot_rx_string) {
  first_number = (iot_rx_string[0] - '0') * 10 + iot_rx_string[1] - '0';
}

void Set_Time_Delay(char * iot_rx_string) {
  int time = (iot_rx_string[3] - '0') * 10;
  time += iot_rx_string[4] - '0';
  
  time_sequence_delay_sm = Time_Sequence + time + time;
}

// State Transitions and initializations (for output)
void Update_State_Machine(char * iot_rx_string) {
  // If it's a fresh command, str_ptr is a pointer to a string
  // If it's just a time sequence update, str_ptr is NULL
  if (iot_rx_string == NULL) {
    // timer countdown
    if (time_sequence_delay_sm == Time_Sequence)
      if (next_commands[0] == NULL){
        //State = '-';
        Motors_Disable();
        //if (black_line_executing)
        //  black_line_executing = 2;
      }
      else{
        Update_State_Machine(next_commands);
      }
  }
  else {
    // Default and shared parameters
    Get_First_Number(iot_rx_string);
    State = iot_rx_string[2];
    Set_Time_Delay(iot_rx_string);
    top_speed = WHEEL_MIDDLE;
    K_gain = K_GAIN_DEFAULT;
    state_name_ptr = state_name;
    //bl_step_name_ptr = bl_step_name;
    
    // Check to see if we received multiple commands
    // Keep in mind that * iot_rx_string could point to the same location as *next_commands
    // This is why we saved the first number, state, and time delay
    if (iot_rx_string[5] == '^')
      strcpy(next_commands, iot_rx_string+6);
    else
      next_commands[0] = NULL;
    
    switch (State) {
    case 'f':   // Go forwards with PWM
      Motors_Enable();
      velocity2pwm(first_number * 40, 0);
      state_name_ptr = "Fwrd";
      break;
      
    case 'r':   // Go right with PWM, full speed
      Motors_Enable();
      velocity2pwm(MAX_VELOCITY, first_number * 40);
      state_name_ptr = "Right";
      break;
    
    case 'l':   // Go left with PWM, full speed
      Motors_Enable();
      velocity2pwm(MAX_VELOCITY, -first_number * 40);
      state_name_ptr = "Left";
      break;
      
    case 'b':   // Go backwards with PWM
      Motors_Enable();
      velocity2pwm(-first_number * 40, 0);
      state_name_ptr = "Bkwd";
      break;

    case 's':   // Stop with PWM
      Motors_Enable();
      velocity2pwm(0, 0);
      state_name_ptr = "Halt";
      break;
  
    case 'F':   // Go forwards with GPIO
      Motors_Disable();
      P6OUT |= R_FORWARD;
      P6OUT |= L_FORWARD;
      state_name_ptr = "Fwrd";
      break;
    
    case 'B':   // Go reverse with GPIO
      Motors_Disable();
      P6OUT |= R_REVERSE;
      P6OUT |= L_REVERSE; 
      state_name_ptr = "Bkwd";
      break;
    
    case '>':   // Stationary (or inch) right with PWM
      Motors_Enable();
      velocity2pwm(0, first_number * 40);
      state_name_ptr = "Right";
      break;
    
    case '<':   // Stationary (or inch) left with PWM
      Motors_Enable();
      velocity2pwm(0, - first_number * 40);
      state_name_ptr = "Left";
      /*Motors_Disable();
      P6OUT |= R_REVERSE;
      P6OUT |= L_FORWARD;*/
      break;
      
    case 'X':   // Turn off motors with GPIO, freewheeling
      Motors_Disable();
      state_name_ptr = "Off";
      switch(first_number){
      case 10:
        bl_step_name_ptr = "BL START";
        break;
      case 20:
        bl_step_name_ptr = "INTERCEPT";
        break;
      case 30:
        bl_step_name_ptr = "BL TURN";
        break;
      case 40:
        bl_step_name_ptr = "BL TRAVEL";
        break;
      case 50:
        bl_step_name_ptr = "BL CIRCLE";
        break;
      case 60:
        bl_step_name_ptr = "BL EXIT";
        break;
      case 70:
        bl_step_name_ptr = "BL STOP";
        black_line_executing = 2;
        break;
      }
      
      break;
      
    case 'R':
      strcpy(next_commands, "10X50^04r06^20r10^04r10^20r10^04r03^04E01");
      black_line_executing = 1;
      Update_State_Machine(next_commands);
      state_name_ptr = "Exec";
      break;
      
    case 'L':
      break;
    
    case 'Z':   // Exit
      strcpy(next_commands, "60X50^60l04^04r20^70X01");
      state_name_ptr = "Exit";
      //black_line_executing = 2;
      
      Update_State_Machine(next_commands);
      break;
    
    case 'E':   // Execute a series of commands to find the line
      // Find_Line1
        black_line_executing = 1;
        // Turn right a little bit
        velocity2pwm(MAX_VELOCITY, first_number * 40);
        // Go straight. Sample ADC. Have a fast average_cm_update_speed
        average_cm_update_speed = 7;
        // Move onto Find_Line2 after a specified amount of time
        set_LCD_IR(TRUE);
        Motors_Enable();
          
        state_name_ptr = "Sampl";
      
      // Next commands:
      // Find_Line2
        // Go straight. Sample ADC. Have a slow average_cm_update_speed
        // Move onto Orient_Perpendicular once the reading falls below a threshold of average_cm_update_speed 

      // Orient_Perpendicular
        // Enable PID control. Have a high k_speed_devalue
        // Move onto Turn_90 once the error is approximately zero

      // Turn_90
        // 

      // Follow_Line
        // Enable PID control. Have a lower k_speed_devaule
      //strcpy(next_commands, "00d99^00p99^00s01");//"00d99^00p99^60l04^00V99");
      strcpy(next_commands, "04d99^04p99^20X50^30X50^64l04^00s01^40X50^04V50^00s01^50X50^04V99");
      break;
    
    case 'd':   // Detect or find the line
      // Find_Line2
        // Go straight. Sample ADC. Have a slow average_cm_update_speed
        top_speed = 2000;//2500;  
        k_speed_devalue = 20;//20;
        set_LCD_IR(TRUE);
        Motors_Enable();
        average_cm_update_speed = 0;
        state_name_ptr = "Detec";
        // Move onto Orient_Perpendicular once the reading falls below a threshold of average_cm_update_speed 
      break;
    
    case 'p':   // orient perpendicular to the line
      // Orient_Perpendicular
        // Enable PID control. Have a high k_speed_devalue
      top_speed = 1300;//1500;
      k_speed_devalue = 3;//3
      //K_gain = K_GAIN_DEFAULT / 2;
      I_Reading = 0;
      set_LCD_IR(TRUE);
      Motors_Enable();
      state_name_ptr = "Orien";
        // Move onto Turn_90 once the error is approximately zero
      break;
    
    case 'V':   // Follow the line
      k_speed_devalue = 4;
      I_Reading = 0;
      set_LCD_IR(TRUE);
      Motors_Enable();
      state_name_ptr = "Folow";
      break;
    
    case 'P':   // Pad
      state_name_ptr = "Pad";
      Motors_Disable();// Turn off motors with GPIO
      if (first_number)
        pad_num_char = '0' + first_number;
      else
        pad_num_char++;
      break;
    }
  }
}

// Output
void State_Machine_ADC_Complete(void) {
  perform_control_calculations();
  switch (State) {
    case 'E':
      //perform_control_calculations();
      I_Reading = 0;
      break;
      
    case 'd':
      // Find_Line2
        // Go straight. Sample ADC. Have a slow average_cm_update_speed
        // Move onto Orient_Perpendicular once the reading falls below a threshold of average_cm_update_speed
        //perform_control_calculations();
        I_Reading = 0;
        velocity2pwm(car_speed, first_number * 40);
        if (car_speed < 0) {
          car_speed = 9999;//prevent potential shoot through of the car speed
          Update_State_Machine(next_commands);
        }
      break;
      
    case 'p':
      // Orient_Perpendicular
        // Enable PID control. Have a high k_speed_devalue
        // Move onto Turn_90 once the error is approximately zero
        //perform_control_calculations();
        velocity2pwm(car_speed, PID_Feedback);  // Update the control system
        if ((car_speed <= 500/*700*/) && (abs(P_Reading) < 10))
          Update_State_Machine(next_commands);
      break;
      
    case 'V':
      // Enable PID control. Have a lower k_speed_devaule
      //perform_control_calculations();
      velocity2pwm(car_speed, PID_Feedback);  // Update the control system
      break;
  }
}

//
void Display_By_State(void) {
  if (IOT_Startup_State != 0) {
    Set_Display_String(0, 0, "Connecting");
    Set_Display_String(1, 0, iot_rx_string);
    Set_Display_String(2, 0, pc_rx_string);
    Set_Display_Int(3, 0, ADC_Right_Detect);
    Set_Display_Int(3, 5, ADC_Left_Detect);
  }
  else if (black_line_executing == 0){
    switch (State) {
      case '*':
        Set_Display_String(0, 0, " Waiting  ");
        //Set_Display_String(1, 0, "For Input ");
        Set_Display_String(1, 0, IP_address_1);
        Set_Display_String(2, 0, IP_address_2);
        Set_Display_Int(3, 0, ADC_Right_Detect);
        Set_Display_Int(3, 5, ADC_Left_Detect);
        // Enable the timer and keep it at zero
        Run_Timer_Enabled = TRUE;
        run_time_decimal = 0;
        break;
      
      case 'P':// on a pad
        Set_Display_String(0, 0, "Arrived 0X");
        Set_Display_Char(0, 9, pad_num_char);
        Set_Display_String(1, 0, IP_address_1);
        Set_Display_String(2, 0, IP_address_2);
        Set_Display_String(3, 0, last_received_command);
        Set_Display_Int(3, 5, run_time_seconds);
        break;
        
      default:
        Set_Display_String(0, 0, "          ");
        Set_Display_String(1, 0, IP_address_1);
        Set_Display_String(2, 0, IP_address_2);
        Set_Display_String(3, 0, last_received_command);
        Display_Time(3, 6, run_time_seconds);
        break;
    }
  }
  // black line executing
  else {
    if (black_line_executing == 1) { // executing
      Set_Display_String(0, 0, bl_step_name_ptr);
      Set_Display_String(1, 0, IP_address_1);
      Set_Display_String(2, 0, IP_address_2);
      Set_Display_Char(3, 0, State);
      Display_Time(3, 6, run_time_seconds);
    }
    else { // done
      Set_Display_String(0, 0, " BL STOP  ");
      Set_Display_String(1, 0, " THAT WAS ");
      Set_Display_String(2, 0, "EASY!! ;-)");
      Set_Display_String(3, 0, "Time:");
      Run_Timer_Enabled = FALSE;
      Display_Time(3, 6, run_time_seconds);
    }
  }
}