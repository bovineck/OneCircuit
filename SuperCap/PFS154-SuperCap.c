/*
  Super Capacitor Candle with Three PWM

  Pseudo-random flickering to simulate a candle. Output is
  via 3xPWM channels, variables can be changed to
  alter the simulation

  Code includes checking to see if there is light and
  sleeping during the day while the capacitor is charging

  Tue 27 May 2025 14:13:42 AEST

  DEVICE = PFS154
  F_CPU = 50000 Hz
  TARGET_VDD = 3.8 V
       _________
      /         |
  1--|VCC    GND|--8
  2--|PA7    PA0|--7
  3--|PA6    PA4|--6
  4--|PA5    PA3|--5
     |__________| 


PAC = 0b00000000 all inputs as standard

                output           off              on          pullup    
                ------           ---              --          ------
pin 2 PA7 PAC = 0b10000000, PA = 0b10000000, PA = 0b00000000, PAPH = 0b10000000
pin 3 PA6 PAC = 0b01000000, PA = 0b01000000, PA = 0b00000000, PAPH = 0b01000000
pin 4 PA5 PAC = 0b00100000, PA = 0b00100000, PA = 0b00000000, PAPH = 0b00100000
pin 5 PA3 PAC = 0b00001000, PA = 0b00001000, PA = 0b00000000, PAPH = 0b00001000
pin 6 PA4 PAC = 0b00010000, PA = 0b00010000, PA = 0b00000000, PAPH = 0b00010000
pin 7 PA0 PAC = 0b00000001, PA = 0b00000001, PA = 0b00000000, PAPH = 0b00000001

*/

#include <stdint.h>
#include <stdlib.h>
#include "../device.h"
#include "../easy-pdk/calibrate.h"
#include "../auto_sysclock.h"
#include "../delay.h"
#include <stdbool.h>

uint16_t myrand = 2901;  // happy birthday
const uint8_t cyclemax = 100;
uint8_t cyclecount = 0;

uint8_t slowcounter = 0;
uint8_t medcounter = 0;
uint8_t fastcounter = 0;
uint8_t slowstart = 0;
uint8_t slowend = 0;
uint8_t medstart = 0;
uint8_t medend = 0;
uint8_t faststart = 0;
uint8_t fastend = 0;
uint8_t faster = 0;

// cutoff percentages for types of flicker
const uint8_t percentnormal = 82;  // cutoff for normal/calm
const uint8_t percentsputter = 20; // cutoff for sputtering/normal

uint8_t flickdelay = 40;        // initial speed of flicker
const uint8_t flickdelaysputter = 9;  // "sputtering" activity
const uint8_t flickdelaynormal = 40;  // "normal" activity
const uint8_t flickdelaycalm = 95;    // "calm" activity
uint8_t choosearray = 1;        // normal waves

uint8_t delaycounter = 50;
uint8_t delaydelay = 20;

bool sunshine = false; // is the sun shining?

uint8_t waves[9][4] = {
  {4, 6, 40, 50} ,    // sputter flicker waveslow
  {6, 8, 50, 80} ,   // sputter flicker wavemed
  {8, 10, 110, 130} ,   // sputter flicker wavefast
  {15, 25, 60, 100} ,    // normal flicker waveslow
  {10, 25, 110, 140} ,   // normal flicker wavemed
  {20, 25, 100, 120} ,   // normal flicker wavefast
  {40, 60, 100, 140} ,    // calm flicker waveslow
  {50, 70, 120, 160} ,   // calm flicker wavemed
  {70, 80, 140, 180}   // calm flicker wavefast
};

bool fastup = true;
bool slowup = true;
bool medup = true;

void mydelay(uint8_t counter) {

  for (uint8_t thiscount = 0; thiscount <= counter; thiscount++) {
    _delay_us(1);
  }
}

uint16_t gimmerand(uint16_t small, uint16_t big) {
  myrand ^= (myrand << 13);
  myrand ^= (myrand >> 9);
  myrand ^= (myrand << 7);
  if (abs(myrand) % 13 == 0) {
    myrand = myrand - 23;
  }
  if (abs(myrand) % 17 == 0) {
    myrand = myrand + 11;
  }
  return abs(myrand) % 23 * (big - small) / 23 + small;
}

void getnewslow(uint8_t whicharray) {
  slowstart = gimmerand(waves[whicharray][0], waves[whicharray][1]);
  slowend = gimmerand(waves[whicharray][2], waves[whicharray][3]);
}

void getnewmed(uint8_t whicharray) {
  medstart = gimmerand(waves[whicharray+1][0], waves[whicharray+1][1]);
  medend = gimmerand(waves[whicharray+1][2], waves[whicharray+1][3]);
}

void getnewfast(uint8_t whicharray) {
  faststart = gimmerand(waves[whicharray+2][0], waves[whicharray+2][1]);
  fastend = gimmerand(waves[whicharray+2][2], waves[whicharray+2][3]);
  faster = gimmerand(2, 6);
}


// interrupt triggered when the sun goes away, voltage of small
// capacitor is drained
void Interrupt(void) {
  __disgint();                // disable global interrupts
  INTEN = 0;                  // disable all interrupts
  INTRQ = 0;
  sunshine = false;
  __engint();   // enable global interrupts
}

// compares the cap voltage with the internal voltage
bool checksolar(void)
{
  uint8_t compresult = 0; // initially a byte
  compresult = GPCC & 0b01000000; // mask the result output
  compresult = compresult >> 6;   // shift it to the least significant bit
  sunshine = (bool)compresult;    // cast result as a boolean
  return sunshine;
}

// this is where the candle code will go - for now, turn on LED

void candlingon() {
  PAC = 0b00110001;
  PA = 0b00000000;

  // see datasheet
  PWMG1DTL = 0x00;
  PWMG1DTH = 0x00;
  PWMG1CUBL = 0xff;
  PWMG1CUBH = 0xff;
  PWMG1C = 0b10100110;
  PWMG1S = 0b00000000;

  PWMG0DTL = 0x00;
  PWMG0DTH = 0x00;
  PWMG0CUBL = 0xff;
  PWMG0CUBH = 0xff;
  PWMG0C = 0b10100110;
  PWMG0S = 0b00000000;

  PWMG2DTL = 0x00;
  PWMG2DTH = 0x00;
  PWMG2CUBL = 0xff;
  PWMG2CUBH = 0xff;
  PWMG2C = 0b10101010;
  PWMG2S = 0b00000000;

  getnewfast(choosearray);
  getnewslow(choosearray);
  getnewmed(choosearray);
  slowcounter = slowstart;
  fastcounter = faststart;
  medcounter = medstart;

  while(!sunshine) {

    // ramp up slow
    if (slowup) {
      slowcounter++;
      if (slowcounter > slowend) { // ramp finished so switch boolean
        slowup = !slowup;
      }
    }
    else {
      // ramp down slow
      slowcounter--;
      if (slowcounter < slowstart) { // ramp finished so switch boolean
        slowup = !slowup;
        getnewslow(choosearray);
      }
    }

    // ramp up med
    if (medup) {
      medcounter++;
      if (medcounter > medend) { // ramp finished so switch boolean
        medup = !medup;
      }
    }
    else {
      // ramp down med
      medcounter--;
      if (medcounter < medstart) { // ramp finished so switch boolean
        medup = !medup;
        getnewmed(choosearray);
      }
    }

    // ramp up fast
    if (fastup) {
      fastcounter = fastcounter + faster;
      if (fastcounter > fastend) { // ramp finished so switch boolean
        fastup = !fastup;
      }
    }
    else {
      // ramp down fast
      fastcounter = fastcounter - faster;
      if (fastcounter < faststart) { // ramp finished so switch boolean
        fastup = !fastup;
        getnewfast(choosearray);
      }
    }
    // delay + a re-purposed random for ramp speeds
    mydelay(flickdelay + faster);
    delaycounter = delaycounter - 1;

    if (delaycounter == 0) {
      delaycounter = gimmerand(1, 100);
      if (delaycounter > percentnormal) { // calm
        flickdelay = flickdelaycalm;
        choosearray = 6;
        delaycounter = 100-delaycounter;
      }
      else if (delaycounter > percentsputter) { // "normal"
        flickdelay = flickdelaynormal;
        choosearray = 3;
      }
      else { // sputtering
        flickdelay = flickdelaysputter;
        choosearray = 0;
      }
    sunshine = (bool)checksolar();
    delaycounter = delaycounter * delaydelay;
    }

    PWMG2DTL = slowcounter & 255;
    PWMG2DTH = slowcounter;
    PWMG0DTL = fastcounter & 255;
    PWMG0DTH = fastcounter;
    PWMG1DTL = medcounter & 255;
    PWMG1DTH = medcounter;

}  

}

void candlingoff() {

  PWMG2DTL = 0;
  PWMG2DTH = 0;
  PWMG0DTL = 0;
  PWMG0DTH = 0;
  PWMG1DTL = 0;
  PWMG1DTH = 0;
  PWMG0C = 0b00100000;
  PWMG1C = 0b00100000;
  PWMG2C = 0b00100000;

}

// here is where the uC goes to sleep
void sleepnow() {

  __disgint();  // disable global interrupts
  MISC |= MISC_FAST_WAKEUP_ENABLE;  // fast wakeup
  PAC = 0;
  PA = 0;
  PAPH = 0xFF; 
  PBDIER = 0;  /* there is no port B on the -S08 package, 
                   without setting this to 0 the uC will wake unexpectedly */
  INTEN = 0b00010000;  // enable comparator interrupt
  INTRQ = 0b00010000; 
  __engint();   // enable global interrupts
  __stopsys();  // go to sleep
}

void main() {

  // page 66 datasheet
  GPCC = 0b10010000;
  // bit 7 enable comparator
  // bit 6 plus input < minus input
  // bit 5 result output NOT sampled by TM2_CLK
  // bit 4 polarity is NOT inversed
  // bit 3-1 000 : PA3 selected as -ve input
  // bit 0 internal voltage set as +ve input

  GPCS = 0b00000011;
  // bit 7 output to PA0 disabled
  // bit 6 reserved
  // bit 5 high range selected
  // bit 4 low range selected
  // bit 3-0 Selection the internal reference voltage level of comparator 
  // 0000 (lowest) ~ 1111 (highest) as a fraction of vdd

  _delay_ms(100);  // small settle time delay


  while (1) {

    if (!sunshine) {
      candlingon();  // it's dark, start candling action
    }
    else
    {
      candlingoff();
      sleepnow();    // it's light, go to sleep
      __reset();
    }
  }
}

// Startup code - Setup/calibrate system clock
unsigned char _sdcc_external_startup(void) {
  /* Set the system clock 
  note it is necessary to enable IHRC
  clock while updating clock settings
  or CPU will hang  */
  PDK_USE_ILRC_SYSCLOCK();          /* use ILRC 55kHz clock as sysclock */
  PDK_DISABLE_IHRC();               /* disable IHRC to save power */
  EASY_PDK_CALIBRATE_ILRC(F_CPU, TARGET_VDD_MV);

  return 0;   // Return 0 to inform SDCC to continue with normal initialization.
}

