/* 
  Demo program combining Multiplexed 7-segment LED Display and LCD Display.
  Push-button Switch input via Interrupt subroutines.
*/

#include "mbed.h"
#include "LCD_DISCO_F429ZI.h"
using namespace std;

// serial comms for debugging
Serial pc(USBTX, USBRX);  

LCD_DISCO_F429ZI lcd;

#define DISPLAY_DELAY 0.0001f   // 100 us 

static const int Digits[] = {
  //abcdefgp    // 7-segment display + decimal point
  0b11111100,   // 0
  0b01100000,   // 1
  0b11011010,   // 2
  0b11110010,   // 3
  0b01100110,   // 4
  0b10110110,   // 5
  0b10111110,   // 6
  0b11100000,   // 7
  0b11111110,   // 8
  0b11110110,   // 9
  0b11101110,   // A (10)
  0b00111110,   // B (11)
  0b10011100,   // C (12)
  0b01111010,   // D (13)
  0b10011110,   // E (14)
  0b10001110    // F (15)
};

/*
  NOTE: The weird ass and seemingly random pin assignments below is necessary because 
  we have USB Serial and LCD display enabled. Consult user manual for available pins. 
  */

DigitalOut Digit1(PC_11); // anode for Digit1 (ones)
DigitalOut Digit2(PC_12); // anode for Digit2 (tens)
DigitalOut Digit3(PC_13); // anode for Digit3 (hundreds)

DigitalOut SegmentA(PA_7);  // clockwise, starting from top segment
DigitalOut SegmentB(PA_8);
DigitalOut SegmentC(PA_15);
DigitalOut SegmentD(PB_4);  
DigitalOut SegmentE(PB_7);
DigitalOut SegmentF(PB_12);
DigitalOut SegmentG(PC_3);  // middle segment  
DigitalOut SegmentP(PC_8);  // decimal point 

void Display_Clear(){
  // reset all pins, clear display
  // common anode, so logic 1 for OFF 
  SegmentA = SegmentB = SegmentC = SegmentD = SegmentE = SegmentF = SegmentG = SegmentP = !0; 
  Digit3 = Digit2 = Digit1 = 0;
}

void Display_Digit(int DigitPosition, int Number){
    // common anode display, so invert logic to light up each segment
    SegmentA = !((Digits[Number] & 0b10000000) >> 7);
    SegmentB = !((Digits[Number] & 0b01000000) >> 6);
    SegmentC = !((Digits[Number] & 0b00100000) >> 5);
    SegmentD = !((Digits[Number] & 0b00010000) >> 4);
    SegmentE = !((Digits[Number] & 0b00001000) >> 3);
    SegmentF = !((Digits[Number] & 0b00000100) >> 2);
    SegmentG = !((Digits[Number] & 0b00000010) >> 1);
    SegmentP = !((Digits[Number] & 0b00000001) >> 0);

    // we need to clear out the other digits before displaying the new digit 
    // otherwise, the same number will be displayed in all the digits. 
    // common anode display, so logic 1 to light up the digit. 
    switch (DigitPosition) {
      case 1: Digit1 = 1;   // ones 
              Digit2 = 0;
              Digit3 = 0;
              break;
      case 2: Digit1 = 0;   // tens
              Digit2 = 1;
              Digit3 = 0;
              break;
      case 3: Digit1 = 0;   // hundreds
              Digit2 = 0;
              Digit3 = 1;
              break;                            
    }
    wait(DISPLAY_DELAY);
}

void Display_Number(int Number, uint32_t Duration_ms)
{
  int hundreds, tens, ones;
  uint32_t start_time_ms, elapsed_time_ms = 0;
  Timer t;

  // breakdown our Number into hundreds, tens and ones
  hundreds = Number / 100;
  tens = (Number % 100) / 10;
  ones = (Number % 100) % 10;

  t.start(); // start timer, we'll use this to setup elapsed display time
  start_time_ms = t.read_ms();
  while (elapsed_time_ms < Duration_ms)
  {
    Display_Digit(3, hundreds);
    Display_Digit(2, tens);
    Display_Digit(1, ones);
    elapsed_time_ms = t.read_ms() - start_time_ms;
  }
  t.stop(); // stop timer
}

int main() {
  // set usb serial
  pc.baud(115200);

  lcd.Clear(LCD_COLOR_RED);

  Display_Clear();
 	lcd.SetFont(&Font24);

  char buf[50];
  lcd.SetBackColor(LCD_COLOR_RED); // text background color
  lcd.SetTextColor(LCD_COLOR_WHITE); // text foreground color

  while(true){
    for (int i=0; i<1000; i++){
      sprintf(buf, "Counting %03d ", i);
      lcd.DisplayStringAt(10, 50, (uint8_t *) buf, CENTER_MODE);

      Display_Number(i, 100); // Number to display, Duration_ms
    }
  }

  return 0;  
}