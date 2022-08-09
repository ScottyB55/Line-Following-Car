/* Scott Burnett
 *
 * Control System for following the line
*/

#include "functions.h"
#include "msp430.h"
#include "macros.h"

#define MIN_PWM (-3000)         // Minimum PWM that prevents shoot-through
#define INT_RESET_THRESH (32)   // Threshold to reset the integral in PID

extern unsigned long int average_cm_reading = 0;
extern unsigned int average_cm_update_speed = 0;   // 0 is update slow. 8 is update instantly

extern int common_mode_error = 0;
extern int k_speed_devalue = 8;
extern int car_speed = 0;
extern int speed_devalue = 0;
extern int top_speed = WHEEL_MIDDLE;

extern int K_gain = K_GAIN_DEFAULT;

extern int common_mode_reading = 0;
extern int P_Reading = 0;
extern int P_Reading_Prev = 0;
extern int I_Reading = 0;
extern int D_Reading = 0;
extern int PID_Feedback = 0;

extern int flag_ADC_Conv_Complete = FALSE;


unsigned int incoming_right_forward_pwm;
unsigned int incoming_left_forward_pwm;
unsigned int incoming_right_reverse_pwm;
unsigned int incoming_left_reverse_pwm;


/*
 * Function: update_control
 * --------------------
 * calculates PID
 * devalue the car speed depending on differential error and common mode error from average
 *
 *  returns: void
 */
void perform_control_calculations(void) {
  P_Reading_Prev = P_Reading;
  
  P_Reading = ADC_Right_Detect - ADC_Left_Detect;
  
  D_Reading = P_Reading - P_Reading_Prev;
  
  // Can use longs!
  I_Reading += P_Reading / K_POLE;
  
  if (I_Reading > MAX_I_READING)
    I_Reading = MAX_I_READING;
  else if (I_Reading < -MAX_I_READING)
    I_Reading = -MAX_I_READING;
  

  // (1-1/K_POLE_DECAY)^X = 0.5, solve for X
  // 50hz / X = HALF_LIFE_FREQ
  
  // Reset the integral if it peaks (if we made it back to the center)
  if ((I_Reading > 0) && (P_Reading < -INT_RESET_THRESH) ||
      (I_Reading < 0) && (P_Reading > INT_RESET_THRESH)) {
        I_Reading -= I_Reading / K_POLE_DECAY;
        // Halfway reset in (7/8)^5        -> 10hz
        //                (15/16)^10 or 11 -> 5hz
        // Should cancel out the position pole
      }
    
  
  
  // We can easily flip the polarity here
  // Completley new readings at x hz -> standard zero frequency (increase K_ZERO to divide down)
  // 100hz / 2 = 50hz completley new readings
  // 50hz / 5hz = 10, zero @ 5hz

  // X_Readings are on a scale of +-2^10 +-2^10
  // PID_Feedback is on a scale of +=2^12
  // PID_Feedback is on a scale of WHEEL_MIDDLE
  PID_Feedback = (P_Reading + K_ZERO * D_Reading + I_Reading) * K_gain;
  
  // Phase reversal due to overflow would begin at +- 32000
  // Clamp to provide saturation and to know how much we chopped off (to potentially roll over for next time)
  // velocity assumed to be within += WHEEL_MIDDLE
  
  //Plant_Rollover = 0; // disable plant rollover
  //PID_Feedback += Plant_Rollover;
  
  if (PID_Feedback > top_speed) {
    // indicate saturation, keep in mind that this is also set by shoot through
    P1OUT |= RED_LED;
    //Plant_Rollover = PID_Feedback - WHEEL_MIDDLE;
    PID_Feedback = top_speed;
  }
  else if (PID_Feedback < -top_speed) {
    P1OUT |= RED_LED;
    //Plant_Rollover = PID_Feedback + WHEEL_MIDDLE;
    PID_Feedback = -top_speed;
  }
  else
    P1OUT &= ~RED_LED;
    //Plant_Rollover = 0;
  
  // We will set the velocity on the count of the timer
  // set_velocity(K_SPEED, PID_Feedback);
  
  // Compare the average 
  common_mode_reading = ADC_Right_Detect + ADC_Left_Detect;
  average_cm_reading -= average_cm_reading >> (8 - average_cm_update_speed);
  average_cm_reading += (long) common_mode_reading << (average_cm_update_speed);
  
  common_mode_error = (common_mode_reading) - (int)(average_cm_reading >> 8);
  
  /*if (relative_darkness > 0)
    relative_darkness = -relative_darkness;//0
  
  // Calculate the speed based on the error
  int speed_devalue = P_Reading;
  if (speed_devalue < 0)
    speed_devalue = -speed_devalue;*/
  
  speed_devalue = (abs(common_mode_error) + abs(P_Reading)) * k_speed_devalue;
  car_speed = top_speed - speed_devalue;
  
  if (car_speed < -1300)
    car_speed = -1300;
  
  // velocity2pwm(car_speed, PID_Feedback);
}


/*
 * Function: set_velocity
 * --------------------
 * center left and right speeds around center velocity
 * prevent either speeds being less than -3000
 * Set pwm to WHEEL_MIDDLE - speed
 *
 * Check out what we did here for renaming a register to RIGHT_FORWARD_SPEED, etc
 *
 *  returns: void
 */
// used to be set_velocity
void velocity2pwm(int velocity, int velocity_diff) {
  
  // Handle overflows for positive velocity
  // has a gain twice as high as the upper block
  int r_fwd_speed = velocity + velocity_diff;
  int l_fwd_speed = velocity - velocity_diff;
  
  if (r_fwd_speed > WHEEL_MIDDLE) {
    l_fwd_speed -= r_fwd_speed - WHEEL_MIDDLE;
    r_fwd_speed = WHEEL_MIDDLE;
  }
  else if (l_fwd_speed > WHEEL_MIDDLE) {
    r_fwd_speed -= l_fwd_speed - WHEEL_MIDDLE;
    l_fwd_speed = WHEEL_MIDDLE;
  }
  
  // Prevent values less than 1000, can cause shootthrough 
  // due to invalid PWM
  if (l_fwd_speed < MIN_PWM)
    l_fwd_speed = MIN_PWM;
  if (r_fwd_speed < MIN_PWM)
    r_fwd_speed = MIN_PWM;
  
  // Set new PWM forward value
  incoming_right_forward_pwm = WHEEL_MIDDLE + r_fwd_speed; // P6.0 Right Forward PWM duty cycle
  incoming_left_forward_pwm = WHEEL_MIDDLE + l_fwd_speed; // P6.1 Left Forward PWM duty cycle
  
  // Set new PWM reverse value
  incoming_right_reverse_pwm = incoming_right_forward_pwm+8; // P6.2 Right Reverse PWM duty cycle
  incoming_left_reverse_pwm = incoming_left_forward_pwm+8; // P6.3 Left Reverse PWM duty cycle
}

/*
 * Call this function in sync with the PWM so that we don't get 
 * shoot through!
 */
void update_pwm_registers(void) {
  RIGHT_FORWARD_SPEED = incoming_right_forward_pwm;
  LEFT_FORWARD_SPEED = incoming_left_forward_pwm;
  RIGHT_REVERSE_SPEED = incoming_right_reverse_pwm;
  LEFT_REVERSE_SPEED = incoming_left_reverse_pwm;
}