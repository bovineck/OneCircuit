// -----------------------------------------------------------------
// Description: A fake candle <sigh> running on an attiny13a μC
// connected to a 3 led A128 5050 led, with two leds connected to 
// one hardware pwm (OCR0A) on a "random" slow wave, and the third
// led connected to the other hardware pwm (OCR0B) on a "random"
// fast wave. The candle has a LDR which means it only runs in the
// dark. The attiny13a runs at 1 MHz, sleeping most of the time!
// 
// Author: A N Peck                    Date: 23/07/2017
// -----------------------------------------------------------------
// 
// ATMEL ATTINY13a μC
// 
//                                   +-\/-+
//  RESET--ACD0--5/A0--PCINT5--PB5  1|    |8  VCC
//   CLKI--ACD3--3/A3--PCINT3--PB3  2|    |7  PB2--PCINT2--2/A1--SCK--ADC1
//         ACD2--4/A2--PCINT4--PB4  3|    |6  PB1--PCINT1---1---MISO--OCOB--INT0*
//                             GND  4|    |5  PB0--PCINT0---0---MOSI--OCOA*
//                                   +----+
//  * indicates PWM port
// 

#include <avr/sleep.h>        // routines for sleepy heads
#include <avr/pgmspace.h>     // for reading the progmem values

//  array for leds contains [lowest low pwm value, highest low pwm value,
//  lowest high pwm value, highest high pwm value]
//  fiddle with these to get different effects

const PROGMEM uint8_t waveslow[] = {50, 100, 170, 200}; // 2 leds on PB0
const PROGMEM uint8_t wavefast[] = {30, 80, 150, 230};  // 1 led on PB1

// A2 analog read pin - when I label it PB4 or PCINT4 it does not work! Dunno why...
#define photoresistor A2 

uint16_t myrand = 2901;  // happy birthday

// global variables randomised later for flickering, using the
// "waveslow" and "wavefast" arrays
uint8_t slowcounter = 0;
uint8_t fastcounter = 0;
uint8_t slowstart = 0;
uint8_t slowend = 0;
uint8_t faststart = 0;
uint8_t fastend = 0;
uint8_t faster = 0;

// booleans to keep track of "fading up" or "fading down"
// in each of the slow and fast cycles
boolean fastup = true;
boolean slowup = true;

// Watchdog Timer interrupt = "time to wake up"
ISR(WDT_vect) { 
}


// generate a "random" number between small and big
uint16_t gimmerand(uint16_t small, uint16_t big) {
  myrand ^= (myrand << 13);
  myrand ^= (myrand >> 9);
  myrand ^= (myrand << 7);
  return abs(myrand)%23*(big-small)/23+small;
}

// initialise a new slow cycle
void getnewslow() {
slowstart = gimmerand(pgm_read_byte(&waveslow[0]), pgm_read_byte(&waveslow[1])); 
slowend = gimmerand(pgm_read_byte(&waveslow[2]), pgm_read_byte(&waveslow[3])); 
}

// initialise a new fast cycle including the new speed of cycle
void getnewfast() {
faststart = gimmerand(pgm_read_byte(&wavefast[0]), pgm_read_byte(&wavefast[1])); 
fastend = gimmerand(pgm_read_byte(&wavefast[2]), pgm_read_byte(&wavefast[3]));
faster = gimmerand(1, 4);
}
 
void setup()
{
  
  // approx 8s watchdog
  WDTCR |= (1<<WDP3 )|(0<<WDP2 )|(0<<WDP1)|(1<<WDP0); 

  // hardware pwm on channel A and hardware pwm on 
  // channel B no prescaler
  TCCR0A = 2<<COM0A0 | 2<<COM0B0 | 3<<WGM00; 
  TCCR0B = (0<<WGM02) | (0<<CS02) | (0<<CS01) | (1<<CS00);

  //initialise, randomise some starting globals
  getnewfast();
  getnewslow();
  slowcounter = slowstart;
  fastcounter = faststart;
}

void mydelay(byte amount) {
  for (byte count = 1; count <= amount; count++) {
    _delay_ms(1);
  }
}
  
void loop()
{

  // read the A2 input pin - is it dark?
  uint16_t val = analogRead(photoresistor);    

  if (val < 250) {  // yes it's dark

  // light em up - all pins to input except PB0 and PB1
  DDRB = 0b00000011;      

  // these two loops give about 5 minutes of flicker before light level is rechecked
  for (uint16_t count1 = 500; count1 > 0; count1--) {
  for (uint8_t count2 = 100; count2 > 0; count2--) {

  // load the duty cycles into the hardware pwm channels
  OCR0A = slowcounter;
  OCR0B = fastcounter;

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
    getnewslow();  
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
    getnewfast();  
  }
  }

  // delay + a re-purposed random for ramp speeds
    mydelay(6  + faster);

}
  }
  }

  // it's dark so let's sleep for a bit
   else {

    // some watchdog stuff, and shutdown everything
    WDTCR |= (1<<WDTIE);
    WDTCR |= (0<<WDE);
    ADCSRA &= ~(1<<ADEN); // turn off ADC
    ACSR|=(1<<ACD);       // turn off Analog comparator.
    DDRB = 0b00000000;    // all pins to input
    
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
    sei(); // enable global interrupts
    
    // 8x32 = 600, around 5 mins of sleep
    // System sleeps here
    for (uint8_t count = 32; count > 0; count--) { 
        sleep_mode();                           
    }
    
    // now we wake the μC 
    sleep_disable();                     
    ADCSRA |= (1 << ADEN);  // turn on ADC
    ACSR = (0<<ACD);        // Turn on Analog comparator.
    _delay_ms(50);          // take a breath before the action
    
    }
}
