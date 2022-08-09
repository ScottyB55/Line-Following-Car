/* Scott Burnett
 *
 * Communication interrupts, buffers, functions, and initalizations
*/


#include "functions.h"
#include "msp430.h"
#include "macros.h"

// *Only use extern if we use it across multiple files!
// Can use the keyword extern volatile

char command_char = '^';
char temp;
int link_enabled = FALSE;

char TX_TO_PC[S_RING_SIZE]      ={0};
unsigned int pc_tx_txed         = 0;
unsigned int pc_tx_todo         = 0;
extern volatile int flag_pc_tx  = FALSE;// sets once the entire transmission is complete

char TX_TO_IOT[S_RING_SIZE]     ={0};
unsigned int iot_tx_txed        = 0;
unsigned int iot_tx_todo        = 0;    
extern volatile int flag_iot_tx = FALSE;

char RX_FROM_PC[S_RING_SIZE]    ={0};
unsigned int pc_rx_rxed         = 0;
unsigned int pc_rx_read         = 0;
unsigned int pc_commanding      = FALSE;        // is true during the duration of a command
unsigned int pc_prev_enter      = FALSE;
extern volatile int flag_pc_rx_end = FALSE;     // sets when the end character is received
extern volatile int flag_pc_rx_command = FALSE;// sets when the end character is received during a command

char RX_FROM_IOT[S_RING_SIZE]   ={0};
unsigned int iot_rx_rxed        = 0;
unsigned int iot_rx_read        = 0;
unsigned int iot_commanding     = FALSE;        // is true during the duration of a command
unsigned int iot_prev_enter     = FALSE;
extern volatile int flag_iot_rx_end = FALSE;    // sets when the end character is received
extern volatile int flag_iot_rx_command = FALSE;// sets when the end character is received during a command

// PC, J14
// UCA1RXD - P4.2, UCA1TXD - P4.3
#pragma vector=EUSCI_A1_VECTOR
__interrupt void eUSCI_A1_ISR(void){
switch(UCA1IV){ 
  case 2: // Receive buffer full, Vector 2 RXIFG
    
    temp = UCA1RXBUF;
    
    // Ignore excess \r and \n if the previous character was a \r or \n
    if ((temp == '\r') || (temp == '\n')) {
      if (!pc_prev_enter) {
        flag_pc_rx_end = TRUE;
        if (pc_commanding)
          flag_pc_rx_command = TRUE;
      }
      pc_prev_enter = TRUE;
    }
    else {
      if (pc_prev_enter)
        pc_commanding = FALSE;
      
      pc_prev_enter = FALSE;
    
      pc_rx_rxed++;
      if(pc_rx_rxed >= S_RING_SIZE)
        pc_rx_rxed = 0; // Circular buffer back to beginning
      
      RX_FROM_PC[pc_rx_rxed] = temp;
      
      if (temp == command_char)
        pc_commanding = TRUE;
    }
    
    // If we're not in a command and the link is enabled let it flow to the IOT 
    if ((pc_commanding == FALSE) && (link_enabled == TRUE))
      Transmit_To_IOT_Char(temp);
    
    break;
    
  case 4: // Transmit buffer empty, Vector 4 TXIFG
    pc_tx_txed++;
    if(pc_tx_txed >= S_RING_SIZE)
      pc_tx_txed = 0;
    
    UCA1TXBUF = TX_TO_PC[pc_tx_txed];
    
    if(pc_tx_txed == pc_tx_todo)
      UCA1IE &= ~UCTXIE; // Stop transmitting
    
    break;
  }
}

// IOT, J9
// UCA0RXD - P1.6, UCA0TXD - P1.7
#pragma vector=EUSCI_A0_VECTOR
__interrupt void eUSCI_A0_ISR(void){
switch(UCA0IV){ 
  case 2: // Receive buffer full, Vector 2 RXIFG
    
    temp = UCA0RXBUF;
    
    // Ignore excess \r and \n if the previous character was a \r or \n
    if ((temp == '\r') || (temp == '\n')) {
      if (!iot_prev_enter) {
        flag_iot_rx_end = TRUE;
        if (iot_commanding)
          flag_iot_rx_command = TRUE;
      }
      iot_prev_enter = TRUE;
    }
    else {
      if (iot_prev_enter)
        iot_commanding = FALSE;
      
      iot_prev_enter = FALSE;
    
      iot_rx_rxed++;
      if(iot_rx_rxed >= S_RING_SIZE)
        iot_rx_rxed = 0; // Circular buffer back to beginning
      
      RX_FROM_IOT[iot_rx_rxed] = temp;
      
      if (temp == command_char)
        iot_commanding = TRUE;
    }
    
    // If we're not in a command, let it flow to the PC 
    if ((iot_commanding == FALSE) && (link_enabled == TRUE))
      Transmit_To_PC_Char(temp);
    
    break;
    
  case 4: // Transmit buffer empty, Vector 4 TXIFG
    iot_tx_txed++;
    if(iot_tx_txed >= S_RING_SIZE)
      iot_tx_txed = 0;
    
    UCA0TXBUF = TX_TO_IOT[iot_tx_txed];
    
    if(iot_tx_txed == iot_tx_todo)
      UCA0IE &= ~UCTXIE; // Stop transmitting
    
    break;
  }
}

void Transmit_To_PC_Char(char character) {
  pc_tx_todo++;
    
  if(pc_tx_todo >= S_RING_SIZE)
    pc_tx_todo = 0;
    
  TX_TO_PC[pc_tx_todo] = character;
  
  UCA1IE |= UCTXIE; // Enable PC Tx interrupt
}

void Transmit_To_PC(char * string_pointer) {
  while (*string_pointer != NULL) {     // While the character at the address isn't null
    Transmit_To_PC_Char(*string_pointer);
    string_pointer++;                   // Increment the address
  }
}

void Transmit_To_IOT_Char(char character) {
  iot_tx_todo++;
    
  if(iot_tx_todo >= S_RING_SIZE)
    iot_tx_todo = 0;
    
  TX_TO_IOT[iot_tx_todo] = character;
  
  UCA0IE |= UCTXIE; // Enable IOT Tx interrupt
}

void Transmit_To_IOT(char * string_pointer) {
  while (*string_pointer != NULL) {     // While the character at the address isn't null
    Transmit_To_IOT_Char(*string_pointer);
    string_pointer++;                   // Increment the address
  }
}

// Puts the received string at string_pointer, with null at the end
void Get_PC_Received(char * string_pointer) {
  while (pc_rx_read != pc_rx_rxed) {
    pc_rx_read++;
    
    if(pc_rx_read >= S_RING_SIZE)
      pc_rx_read = 0;
    
    temp = RX_FROM_PC[pc_rx_read];
    *(string_pointer+0) = temp;
    
    string_pointer++;
  }
  
  *string_pointer = NULL;
}

// Puts the received string at string_pointer, with null at the end
void Get_IOT_Received(char * string_pointer) {
  while (iot_rx_read != iot_rx_rxed) {
    iot_rx_read++;
    
    if(iot_rx_read >= S_RING_SIZE)
      iot_rx_read = 0;
    
    *string_pointer = RX_FROM_IOT[iot_rx_read];
    
    string_pointer++;
  }
  
  *string_pointer = NULL;
}

// Puts the received command at string_pointer, with null at the end
void Get_IOT_Command(char * string_pointer) {
  int found_carrot = FALSE;
  
  while (iot_rx_read != iot_rx_rxed) {
    iot_rx_read++;
    
    if(iot_rx_read >= S_RING_SIZE)
      iot_rx_read = 0;
    
    *string_pointer = RX_FROM_IOT[iot_rx_read];
    
    if (found_carrot)
      string_pointer++;
    
    if (*string_pointer == '^')
      found_carrot = TRUE;
  }
  
  *string_pointer = NULL;
}


void Init_Serial_UCA0(void){
  //------------------------------------------------------------------------------
  //                                          TX error (%) RX error (%)
  // BRCLK   Baudrate UCOS16 UCBRx UCFx UCSx  neg   pos   neg   pos
  // 8000000 4800     1      104   2    0xD6  -0.08 0.04  -0.10 0.14
  // 8000000 9600     1      52    1    0x49  -0.08 0.04  -0.10 0.14
  // 8000000 19200    1      26    0    0xB6  -0.08 0.16  -0.28 0.20
  // 8000000 57600    1      8     10   0xF7  -0.32 0.32  -1.00 0.36
  // 8000000 115200   1      4     5    0x55  -0.80 0.64  -1.12 1.76
  // 8000000 460800   0      17    0    0x4A  -2.72 2.56  -3.76 7.28
  //------------------------------------------------------------------------------
  // Configure eUSCI_A0 for UART mode
  UCA0CTLW0 = 0;
  UCA0CTLW0 |= UCSWRST ; // Put eUSCI in reset
  UCA0CTLW0 |= UCSSEL__SMCLK; // Set SMCLK as fBRCLK
  UCA0CTLW0 &= ~UCMSB; // MSB, LSB select
  UCA0CTLW0 &= ~UCSPB; // UCSPB = 0(1 stop bit) OR 1(2 stop bits)
  UCA0CTLW0 &= ~UCPEN; // No Parity
  UCA0CTLW0 &= ~UCSYNC;
  UCA0CTLW0 &= ~UC7BIT;
  UCA0CTLW0 |= UCMODE_0;
  // BRCLK   Baudrate UCOS16 UCBRx UCFx UCSx neg  pos  neg  pos
  // 8000000 115200   1      4     5    0x55 0.80 0.64 1.12 1.76
  // UCA?MCTLW = UCSx + UCFx + UCOS16
  UCA0BRW = 4; // 115,200 baud
  UCA0MCTLW = 0x5551;
  
  UCA0CTLW0 &= ~UCSWRST ; // release from reset
  //UCA0TXBUF = 0x00; // Prime the Pump
  //UCA0RXBUF = 0x00; // Prime the Pump
  UCA0IE |= UCRXIE; // Enable RX interrupt
}

void Init_Serial_UCA1(void){
  //------------------------------------------------------------------------------
  //                                          TX error (%) RX error (%)
  // BRCLK   Baudrate UCOS16 UCBRx UCFx UCSx  neg   pos   neg   pos
  // 8000000 4800     1      104   2    0xD6  -0.08 0.04  -0.10 0.14
  // 8000000 9600     1      52    1    0x49  -0.08 0.04  -0.10 0.14
  // 8000000 19200    1      26    0    0xB6  -0.08 0.16  -0.28 0.20
  // 8000000 57600    1      8     10   0xF7  -0.32 0.32  -1.00 0.36
  // 8000000 115200   1      4     5    0x55  -0.80 0.64  -1.12 1.76
  // 8000000 460800   0      17    0    0x4A  -2.72 2.56  -3.76 7.28
  //------------------------------------------------------------------------------
  // Configure eUSCI_A1 for UART mode
  UCA1CTLW0 = 0;
  UCA1CTLW0 |= UCSWRST ; // Put eUSCI in reset
  UCA1CTLW0 |= UCSSEL__SMCLK; // Set SMCLK as fBRCLK
  UCA1CTLW0 &= ~UCMSB; // MSB, LSB select
  UCA1CTLW0 &= ~UCSPB; // UCSPB = 0(1 stop bit) OR 1(2 stop bits)
  UCA1CTLW0 &= ~UCPEN; // No Parity
  UCA1CTLW0 &= ~UCSYNC;
  UCA1CTLW0 &= ~UC7BIT;
  UCA1CTLW0 |= UCMODE_0;
  // BRCLK   Baudrate UCOS16 UCBRx UCFx UCSx neg  pos  neg  pos
  // 8000000 115200   1      4     5    0x55 0.80 0.64 1.12 1.76
  // UCA?MCTLW = UCSx + UCFx + UCOS16
  UCA1BRW = 4; // 115,200 baud
  UCA1MCTLW = 0x5551;
  
  UCA1CTLW0 &= ~UCSWRST ; // release from reset
  //UCA1TXBUF = 0x00; // Prime the Pump, for some reason this does wierd stuff
  //UCA1RXBUF = 0x00; // Prime the Pump
  UCA1IE |= UCRXIE; // Enable RX interrupt
}

void set_IOT_EN(int value) {
  if (value)
    P3OUT |= IOT_EN_CPU; // HIGH
  else
    P3OUT &= ~IOT_EN_CPU; // LOW
}

void set_Link_Enable(int value) {
  link_enabled = value;
}


/*
Example code for switches that change the baud rate

void Switch1_Process(void){ //Switch 1 Configuration
  if (okay_to_look_at_switch1 && sw1_position){
    if (!(P4IN & SW1)){
      sw1_position = PRESSED;
      okay_to_look_at_switch1 = NOT_OKAY;
      count_debounce_SW1 = DEBOUNCE_RESTART;
      //switch function
      baud_screen = INITIAL;
      UCA0BRW = 4; //115,200 baud //need to change with init function
      UCA0MCTLW = 0x5551;
      UCA1BRW = 4; // 115,200 baud
      UCA1MCTLW = 0x5551;
      //setting up to transmit
      milsec = INITIAL;
      seconds = INITIAL;
      transmit_flag = TRANSMIT;
      }
    }
  if (count_debounce_SW1 <= DEBOUNCE_TIME){ 
    count_debounce_SW1++;
  }
  else{
    okay_to_look_at_switch1 = OKAY;
    if (P4IN & SW1){
      sw1_position = RELEASED;
    }
  }
}
void Switch2_Process(void){ // Switch 2 Configurations
  if (okay_to_look_at_switch2 && sw2_position){
    if (!(P2IN & SW2)){
    sw2_position = PRESSED;
    okay_to_look_at_switch2 = NOT_OKAY;
    count_debounce_SW2 = DEBOUNCE_RESTART;
    //switch function
    baud_screen = BAUD2;
    UCA0BRW = 17; //460,800 baud
    UCA0MCTLW = 0x4A00;
    UCA1BRW = 17; //460,800 baud
    UCA1MCTLW = 0x4A00;
    //setting up to transmit
    milsec = INITIAL;
    seconds = INITIAL;
    transmit_flag = TRANSMIT;
    }
  }
  if (count_debounce_SW2 <= DEBOUNCE_TIME){
    count_debounce_SW2++;
  }
  else{
    okay_to_look_at_switch2 = OKAY;
    if (P2IN & SW2){
      sw2_position = RELEASED;
    }
  }
}*/
// Changing Baud Rate ^^