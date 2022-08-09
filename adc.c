/* Scott Burnett
 *
 * Initializations and interrupts for the ADC and DAC
*/

#include "functions.h"
#include  "msp430.h"
#include  "macros.h"

extern unsigned int ADC_Channel = 0;

extern unsigned int ADC_Thumb = 0;
extern unsigned int ADC_Right_Detect = 0;
extern unsigned int ADC_Left_Detect = 0;

extern unsigned int adc_total_count = 0;

void Setup_ADC(void){
  //------------------------------------------------------------------------------
  // V_DETECT_L (0x04) // Pin 2 A2
  // V_DETECT_R (0x08) // Pin 3 A3
  // V_THUMB (0x20) // Pin 5 A5
  //------------------------------------------------------------------------------
  
  // ADCCTL0 Register
  ADCCTL0 = 0; // Reset
  ADCCTL0 |= ADCSHT_2; // 16 ADC clocks
  ADCCTL0 |= ADCMSC; // MSC
  ADCCTL0 |= ADCON; // ADC ON
  
  // ADCCTL1 Register
  ADCCTL2 = 0; // Reset
  ADCCTL1 |= ADCSHS_0; // 00b = ADCSC bit
  ADCCTL1 |= ADCSHP; // ADC sample-and-hold SAMPCON signal from sampling timer.
  ADCCTL1 &= ~ADCISSH; // ADC invert signal sample-and-hold.
  ADCCTL1 |= ADCDIV_0; // ADC clock divider - 000b = Divide by 1
  ADCCTL1 |= ADCSSEL_0; // ADC clock MODCLK
  ADCCTL1 |= ADCCONSEQ_0; // ADC conversion sequence 00b = Single-channel single-conversion
  // ADCCTL1 & ADCBUSY identifies a conversion is in process
  
  // ADCCTL2 Register
  ADCCTL2 = 0; // Reset
  ADCCTL2 |= ADCPDIV0; // ADC pre-divider 00b = Pre-divide by 1
  ADCCTL2 |= ADCRES_2; // ADC resolution 10b = 12 bit (14 clock cycle conversion time)
  ADCCTL2 &= ~ADCDF; // ADC data read-back format 0b = Binary unsigned.
  ADCCTL2 &= ~ADCSR; // ADC sampling rate 0b = ADC buffer supports up to 200 ksps
  
  // ADCMCTL0 Register
  ADCMCTL0 |= ADCSREF_0; // VREF - 000b = {VR+ = AVCC and VR– = AVSS }
    // sets the highest channel for a sequence of conversions
  // ADCMCTL0 |= ADCINCH_5; // V_THUMB (0x20) Pin 5 A5
  ADCMCTL0 |= ADCINCH_3;
  
  ADCIE |= ADCIE0; // Enable ADC conv complete interrupt
  //ADCCTL0 |= ADCENC; // ADC enable conversion.
  //ADCCTL0 |= ADCSC; // ADC start conversion.
}

// ADC conversion complete interrupt
#pragma vector=ADC_VECTOR
__interrupt void ADC_ISR(void){
  adc_total_count++;
  //      adc interrupt vector,
  switch(__even_in_range(ADCIV,ADCIV_ADCIFG)){
    case ADCIV_ADCIFG: // ADCMEM0 memory register with the conversion result
      ADCCTL0 &= ~ADCENC; // Disable ENC bit, disable conversions
      
      switch (ADC_Channel++){
        case 0x00: // Channel A2 Interrupt (V_DETECT_L)
          ADCMCTL0 &= ~ADCINCH_2; // Disable Last channel A2 (V_DETECT_L)
          ADCMCTL0 |= ADCINCH_3;  // Enable Next channel A3 (V_DETECT_R)
          
          ADC_Left_Detect = ADCMEM0>>1; // Move result into Global
          
          // Signal to start the next conversion
          ADCCTL0 |= ADCENC; // Enable Conversions
          ADCCTL0 |= ADCSC;  // Start next conversion
          
          break;
          
        case 0x01: // Channel A3 Interrupt (V_DETECT_R)
          ADCMCTL0 &= ~ADCINCH_3; // Disable Last channel A3 (V_DETECT_R)
          ADCMCTL0 |= ADCINCH_2;  // Enable Next channel A2 (V_DETECT_L)
          
          ADC_Right_Detect = ADCMEM0>>1;//2; // Move result into Global
          
          // We have the ADC conversions now!
          // Turn off the IR LED
          //P6OUT &= ~LCD_IR;
          
          //if (Following_Line)
          //  update_control();
          
          flag_ADC_Conv_Complete = TRUE;
          
          ADC_Channel=0;        // Cycle to original channel
          
          break;
        default:
          // This shouldn't run
          ADC_Channel=0;
          break;
      }
      
      // Signal that we are ready to change the display
      //display_change_ready = TRUE;
      
      break;
    case ADCIV_NONE:
    case ADCIV_ADCOVIFG: // When a conversion result is written to the ADCMEM0
    // before its previous conversion result was read.
    case ADCIV_ADCTOVIFG: // ADC conversion-time overflow
    case ADCIV_ADCHIIFG: // Window comparator interrupt flags
    case ADCIV_ADCLOIFG: // Window comparator interrupt flag
    case ADCIV_ADCINIFG: // Window comparator interrupt flag
    default:
      //while(TRUE){}
      break;
  }
}
 

// DAC
void Init_REF(void){
  // Configure reference module
  PMMCTL0_H = PMMPW_H; // Unlock the PMM registers
  PMMCTL2 = INTREFEN; // Enable internal reference
  PMMCTL2 |= REFVSEL_2; // Select 2.5V reference
  while(!(PMMCTL2 & REFGENRDY)); // Poll till internal reference settles
  // Using a while statement is not usually recommended without an exit strategy.
  // This while statement is the suggested operation to allow the reference to settle.
}

// DAC
void Init_DAC(void){
  int DAC_data = 0x900; // Value between 0x000 and 0x0FFF
  SAC3DAT = DAC_data; // Initial DAC data
  SAC3DAC = DACSREF_1; // Select int Vref as DAC reference
  //SAC3DAC = DACSREF_0;  // Select AVCC as reference
  SAC3DAC |= DACLSEL_0; // DAC latch loads when DACDAT written
  
  // SAC3DAC |= DACIE; // generate an interrupt
  
  SAC3OA = NMUXEN; // SAC Negative input MUX controL
  SAC3OA |= PMUXEN; // SAC Positive input MUX control
  SAC3OA |= PSEL_1; // 12-bit reference DAC source selected
  SAC3OA |= NSEL_1; // Select negative pin input
  SAC3OA |= OAPM; // Select low speed and low power mode
  SAC3PGA = MSEL_1; // Set OA as buffer mode
  SAC3OA |= SACEN; // Enable SAC
  SAC3OA |= OAEN; // Enable OA
  
  P3SELC |= DAC_CNTL;
  P3OUT &= ~DAC_CNTL;//DAC_CNTL1;
  P3DIR &= ~DAC_CNTL;//DAC_CNTL1
  SAC3DAC |= DACEN; // Enable DAC
}
