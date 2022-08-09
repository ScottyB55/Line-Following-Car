/*
 * Description: This file contains the macro 
 * or global variables used accross multiple files
 * --------------------
 * Scott Burnett
 * ECE 306 Spring 2022
 * 
 * Defines global variables sorted in alphabetical order by 
 * the file in which they are initialized and dominantly used
 *
 * Variables only used within a single .c file don't need to be extern
 * Variables used in multiple .c files must be extern and declared here
 */

#include <stdlib.h>
#include <stdio.h>

   
// general
#define TRUE            (0x01)
#define FALSE           (0x00)
#define SET             (1)
#define RESET           (0)
#define ALWAYS          (1)
#define RESET_STATE     (0)
   

// adc.c
extern unsigned int ADC_Channel;

extern unsigned int ADC_Thumb;
extern unsigned int ADC_Right_Detect;
extern unsigned int ADC_Left_Detect;

extern unsigned int adc_total_count;
// extern unsigned int flag_ADC_Conv_Complete;
#define DAC_CNTL (0x20)//1<<5


// auxiliary.c


// clocks.c


// comm.c
#define S_RING_SIZE (50)
extern volatile int flag_pc_tx;
extern volatile int flag_iot_tx;
extern volatile int flag_pc_rx_end;
extern volatile int flag_pc_rx_command;
extern volatile int flag_iot_rx_end;
extern volatile int flag_iot_rx_command;


// controlsystem.c
extern unsigned long int average_cm_reading;
extern unsigned int average_cm_update_speed;

extern int common_mode_error;
extern int k_speed_devalue;
extern int car_speed;
extern int speed_devalue;
extern int top_speed;

extern int common_mode_reading;
extern int P_Reading;
extern int P_Reading_Prev;
extern int I_Reading;
extern int D_Reading;
extern int PID_Feedback;

extern int flag_ADC_Conv_Complete;

#define K_POLE (32)             // = 200hz / 6hz = FREQ / POLE_FREQ
#define K_POLE_DECAY (32)       //10hz half life//(64)//5hz half life
  // X = FREQ / POLE_DECAY_FREQ
  // (1 - 1/K_POLE_DECAY)^X = 0.5, choose K_POLE_DECAY that satisfies this
#define K_ZERO (8)              // = 200hz / 24hz = FREQ / ZERO_FREQ
extern int K_gain;// (16), see statemachine.c
#define K_GAIN_DEFAULT (16)
  // K_ZERO * K_GAIN controls the crossover frequeny and unity gain bandwidth
  // Keep K_ZERO * K_GAIN ~ 64 or 128 to prevent oscillation @ high speed
  // In general, keep K_GAIN higher and K_ZERO lower

#define MAX_I_READING (250)     // = 4000 / K_GAIN. This keeps the net integral
                                // after factoring in gain from exceeding 4000
//#define K_SPEED_DEVALUE (8)    // Affects how much common mode and differential
                                // errors devalue the speed


// displays.c
extern char display_line[4][11];
extern char *display[4];
extern unsigned char display_mode;
extern volatile unsigned int display_change_pending;
extern volatile unsigned int display_change_ready;
extern unsigned int update_display_count_timer;

#define LCD_BACKLITE    (0x08) //1<<3
#define RED_LED         (0x01) // RED LED 0
#define GRN_LED         (0x40) // GREEN LED 1


// datalog.c
#define LOG_WIDTH       (20)
#define HISTORIC_LOG_WIDTH (10)

extern unsigned int Log_Current_NextIndex;
extern unsigned int Log_Historic_NextIndex;

extern unsigned int Current_Interval_Sum;
extern unsigned int Historic_Interval_Sum;

extern unsigned int Log[LOG_WIDTH];     // initialize to zeros

extern unsigned int data_valid;         // initialize to false
extern unsigned int event_Hit_Black_Line;


// init.c


// main.c
extern char pc_rx_string[S_RING_SIZE];
extern char iot_rx_string[S_RING_SIZE];
extern char last_received_command[6];


// motors.c
extern int r_fwd_speed;
extern int l_fwd_speed;

void Motors_Disable(void);
void Motors_Enable(void);

#define R_FORWARD       (0x01) //1<<0
#define L_FORWARD       (0x02) //1<<1
#define R_REVERSE       (0x04) //1<<2
#define L_REVERSE       (0x08) //1<<3

#define RIGHT_FORWARD_SPEED     (TB3CCR1)
#define LEFT_FORWARD_SPEED      (TB3CCR2)
#define RIGHT_REVERSE_SPEED     (TB3CCR3)
#define LEFT_REVERSE_SPEED      (TB3CCR4)

#define WHEEL_PERIOD    (8008)// =(8e6)/1000hz
#define WHEEL_MIDDLE    (4000)// 50% duty cycle
#define MAX_VELOCITY    (4000)

#define FORWARD         (0)
#define REVERSE         (1)


// ports.c
#define USE_GPIO        (0x00)
#define USE_SMCLK       (0x01)
#define TEST_PROBE      (0x01) // 0 TEST PROBE
#define LCD_IR          (0x10) //1<<4
#define IOT_BOOT_CPU    (0x10)//1<<4
#define IOT_EN_CPU      (0x80)//1<<7


// startupsequence.c
extern int IOT_Startup_State;
extern char IP_address_1[11];// extra for null termination
extern char IP_address_2[11];


// statemachine.c
extern char State;
extern char pad_num_char;
extern char state_name[6];
extern int black_line_executing;

//extern char bl_step_name[11];
extern char * bl_step_name_ptr;


// switches.c
extern unsigned int sw1_position;
extern unsigned int sw2_position;

extern unsigned int event_SW1_PRESSED;
extern unsigned int event_SW2_PRESSED;

// For switch debouncing
#define TB0CCR1_INTERVAL (6250) // = (8Mhz/8/8)/20hz

#define SW1             (0x02) //1<<1
#define SW2             (0x08) //1<<3
#define PRESSED         (0)
#define RELEASED        (1)
#define NOT_OKAY        (0)
#define OKAY            (1)
#define DEBOUNCE_RESTART (0)
#define DEBOUNCE_TIME   (150)


// system.c


// timers.c
extern unsigned int sync_pwm_count;
extern volatile unsigned int Time_Sequence;
extern unsigned int event_Time_Sequence;

extern unsigned int Run_Timer_Enabled;
extern unsigned int run_time_decimal;
extern unsigned int run_time_seconds;
// see statemachine.c
// extern unsigned int state_countdown;
// see display.c
// extern unsigned int update_display_count_timer;

// Timer 0 CCR0 for 20hz general timer. CCR1 for switch debouncing
#define TB0CCR0_INTERVAL (6250) // =(8Mhz/8/8)/20hz