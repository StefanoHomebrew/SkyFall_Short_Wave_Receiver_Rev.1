/**
  ******************************************************************************
  * @file    VFO_LED_DISPLAY-REV.1.ino
  * @author  Stefano Homebrew
  * @version V1.0
  * @date    22-11-2023
  *
  ******************************************************************************
  *
  *                     SI5351 VFO_LED_DISPLAY - PROJECT
  *
  * This project SI5351 VFO_LED_DISPLAY define an experimental open platform to build
  * a Vfo for Radio's Homebrewing 
  * 
  * Library: 
  * Led_Control https://www.pjrc.com/teensy/td_libs_LedControl.html
  * Seven_Seg  https://github.com/DeanIsMe/SevSeg
  * Rotary Encoder https://github.com/brianlow/Rotary
  * Si5351 https://github.com/etherkit/Si5351Arduino
  * 
  * My projects videos https://www.youtube.com/@stefanohomebrew
  *
  * NOTE: this is an experimental project and the functions can be changed
  * without any advise.
  * The SI5351 VFO_LED_DISPLAY uses examples and projects freeware and available in the opensource
  * community.
  *
  * The SI5351 VFO_LED_DISPLAY openSource software is released under the license:
  *              Common Creative - Attribution 3.0
  ******************************************************************************
*/
#include "SevSeg.h"
#include "LedControl.h"
#include <Rotary.h>
#include "Wire.h"
#include <si5351.h>

unsigned long freq_old = 0;
unsigned long freq = 9610000; 
unsigned long freq2 = 9610000;
unsigned long freq3 = (freq);
unsigned long freqSi = 455000;
volatile uint64_t freqPLL;

int passo = 10000;
byte pass= 1;
byte band = 0;
byte mdfrq = 0;
int var1= 0;
byte var3= 0;
byte var4= 0;

int bottoneStep = A0;
int bottoneBand = A1;
int bottoneMfrq = A2;
/*
 pin 1 is connected to the DataIn 6
 pin 2 is connected to the CLK 8
 pin 3 is connected to CS 7
*/
LedControl lc=LedControl(6,8,7,1);
Rotary r = Rotary(3, 2);
Si5351 si5351;
SevSeg sevseg;

void setup() {
    Serial.begin (9600);
    pinMode(bottoneStep,INPUT_PULLUP);
    pinMode(bottoneBand,INPUT_PULLUP);
    pinMode(bottoneMfrq,INPUT_PULLUP);
    
    PCICR |= (1 << PCIE2);
    PCMSK2 |= (1 << PCINT18) | (1 << PCINT19);
    sei();
///////////////////////////////////////////////////////////////////
    si5351.init(SI5351_CRYSTAL_LOAD_8PF, 0,0);
    si5351.set_correction(100000, SI5351_PLL_INPUT_XO);
    si5351.drive_strength(SI5351_CLK2, SI5351_DRIVE_6MA);
    si5351.output_enable(SI5351_CLK1, 1);
///////////////////////////////////////////////////////////////////     
    lc.shutdown(0,false);// turn off power saving, enables display
    lc.setIntensity(0,10);// sets brightness (0~15 possible values)
    lc.clearDisplay(0);// clear screen

    byte numDigits = 2;   
    byte digitPins[] = {4, 5}; //Digits: 1,2,3,4 <--put one resistor (ex: 220 Ohms, or 330 Ohms, etc, on each digit pin)
    byte segmentPins[] = {6, 7, 8, 9, 10, 11, 12}; //Segments: A,B,C,D,E,F,G,Period
    sevseg.begin(COMMON_ANODE, numDigits, digitPins, segmentPins);
    printNumber(00000001);
    delay(2000);
    printNumber(freq3);  
    sevseg.refreshDisplay();
    inviapll();  
}
 
/////////////////////////////////////////////////////  
ISR(PCINT2_vect) {
  unsigned char result = r.process();
  if (result == DIR_NONE) {
    // do nothing
  }
  else if (result == DIR_CW) {  
    freq = (freq + passo);
    freq3 = (freq);
   printNumber(freq3);
   sevseg.refreshDisplay();


  }
  else if (result == DIR_CCW) {   
    freq = (freq - passo);
    freq3 = (freq);
   printNumber(freq3);
   sevseg.refreshDisplay();

  } 
  }
//////////////////////////////////////////////////////////////////
void loop() {
    if (digitalRead(bottoneStep) == LOW) {
    delay(200); 
    Step();
    }
    if (digitalRead(bottoneBand) == LOW) {
    delay(200); 
    Bands();
    }
    if (digitalRead(bottoneMfrq) == LOW) {
    delay(200); 
    MediaFrq();
    }         
     if (freq != freq_old)  {   
    inviapll();
     freq_old = freq;
     }
  }

void inviapll() {
  //  Serial.println(freq);
    freqPLL = freq - freqSi;
    si5351.set_freq ((freqPLL * 100), SI5351_CLK2);
}

void printNumber(long num)
{
  long terzo;
  long quinto;
  long quarto;
  long sesto;
  long settimo;
  long ottavo;
  long var4=(long)num;
  long secondo,primo; 
  primo=var4%10;
  var4=var4/10;
  secondo=var4%10;
  var4=var4/10;
  terzo=var4%10;
  var4=var4/10;
  quarto=var4%10;
  var4=var4/10;
  quinto=var4%10; 
  var4=var4/10; 
   sesto=var4%10; 
  var4=var4/10; 
  settimo=var4%10; 
  var4=var4/10; 
  ottavo=var4%10; 
  var4=var4/10; 
  
  //Now print the number digit by digit
  lc.setDigit(0,7,(byte)ottavo,false);
  lc.setDigit(0,6,(byte)settimo,true);
  lc.setDigit(0,5,(byte)sesto,false);
  lc.setDigit(0,4,(byte)quinto,false);
  lc.setDigit(0,3,(byte)quarto,true);
  lc.setDigit(0,2,(byte)terzo,false);
  lc.setDigit(0,1,(byte)secondo,false);
  lc.setDigit(0,0,(byte)primo,false);
  delay (300);  
}
////////////////////////////////STEP     
void Step(){   
    pass++;
    if (pass > 3) {
      (pass = 1);
    }
    if (pass < 1) {
      (pass = 1);
    }
   if (pass == 1) {
   printNumber(1000);
   sevseg.refreshDisplay();      
      passo = 1000;
    }
   if (pass == 2) {
   printNumber(10000);
   sevseg.refreshDisplay();      
      passo = 10000;
    }
   if (pass == 3) {
   printNumber(100);
   sevseg.refreshDisplay();      
      passo = 100;    
    }        
 Serial.println(passo);   
}
/////////////////////////////////BAND
void Bands(){  
   band++;
   if (band == 1) {
    freq = 3660000;
    inviapll();
    freq3 = (freq);
   printNumber(freq3);
   sevseg.refreshDisplay();      
      passo = 1000;
    }
    if (band == 2) {
    freq = 6000000;
    inviapll();
    freq3 = (freq);
   printNumber(freq3);
   sevseg.refreshDisplay();      
      passo = 1000;
    }   
   if (band == 3) {
    freq = 7100000;
    inviapll();
    freq3 = (freq);
   printNumber(freq3);
   sevseg.refreshDisplay();      
      passo = 1000;
    }   
   if (band == 4) {
    freq = 9610000;
    inviapll();
    freq3 = (freq);
   printNumber(freq3);
   sevseg.refreshDisplay();      
      passo = 1000;
    }
       if (band == 4) {
        band = 0;
       }
       Serial.println(band);
}
/////////////////////////////////MEDIA FRQ
void MediaFrq(){  
   mdfrq++;
   if (mdfrq == 1) {
    freqSi = 0;
    inviapll();
   
   printNumber(0);
   sevseg.refreshDisplay();      

    }   
   if (mdfrq == 2) {
    freqSi = 455000;
    inviapll();
   printNumber(freqSi);
   sevseg.refreshDisplay();      
      passo = 1000;
    }   
   if (mdfrq == 3) {
    freqSi = 450000;
    inviapll();

   printNumber(freqSi);
   sevseg.refreshDisplay();      

    }
       if (mdfrq == 3) {
        mdfrq = 0;
       }
      // Serial.println(mdfrq);
}
