// -----------------------------------------------------------------
// Description: An LED and BUZZER enabled fridge sensor based on a
// description at https://www.youtube.com/watch?v=Belaszrz11s
//
// Sketch uses 452 bytes (44%) of program storage space.
// Global variables use 5 bytes (7%) of dynamic memory, leaving 59 bytes for local variables.
// Use of assembly would reduce code size and increase speed
// Circuit consumes 170μA when sleeping
//
// Author: A N Peck                    Date: Tuesday 7 January
// -----------------------------------------------------------------
//
// ATMEL ATTINY13 μC
//
//                                   +-\/-+
//  RESET--ACD0--5/A0--PCINT5--PB5  1|    |8  VCC
//   CLKI--ACD3--3/A3--PCINT3--PB3  2|    |7  PB2--PCINT2--2/A1--SCK--ADC1
//         ACD2--4/A2--PCINT4--PB4  3|    |6  PB1--PCINT1---1---MISO--OCOB--INT0*
//                             GND  4|    |5  PB0--PCINT0---0---MOSI--OCOA*
//                                   +----+
//  * indicates PWM port
//

#include <avr/interrupt.h>              // for interrupt routines
#include <avr/sleep.h>                  // the sleep routines

#define LED PB3                         // led pin
#define BUZZ PB0                        // buzzer pin
#define InterruptPin PB2                // interrupt pin

// sleep routine to save power
void powerDown()
{
  pinMode(LED, INPUT);                  // set output pins to input for sleep
  pinMode(BUZZ, INPUT);                 //
  GIMSK |= (1 << PCIE);                 // activate Pin change interrupts
  PCMSK |= (1 << InterruptPin);         // sets the Pin change interrupt mask
  ADCSRA &= ~(1 << ADEN);               // turn off ADC
  ACSR |= (1 << ACD);                   // turn off Analog comparator.
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);  // sleep deeply little one
  sleep_enable();                       // enable sleep mode
  cli();                                // timed sequence
  BODCR = (1 << BODS) | (1 << BODSE);   // turn off brown out detector
  BODCR = (1 << BODS);
  sei();                                // enable global interrupts
  sleep_cpu();                          // system sleeps here

  sleep_disable();                      // ISR routine returns here so wake up
  GIMSK &= ~(1 << PCIE);                // deactivate Pin change interrupts
  ADCSRA |= (1 << ADEN);                // turn on ADC
  ACSR = (0 << ACD);                    // turn on Analog comparator.
  delay(50);                            // settle time then ready for action
}

// pin change interrupt service routine
ISR(PCINT0_vect)
{
}

void setup ()
{
  // configure interrupt pin as input
  pinMode(InterruptPin, INPUT_PULLUP);
}

// fridge is open so let it be for a few counts
void flashabit(int flashes) {
  // start the count
  int countflashes = 0;
  // fridge door still open AND we are timing flashes
  while ((digitalRead(InterruptPin) == HIGH) && (countflashes < flashes)) {
    tone(BUZZ, 8000, 40);
    digitalWrite(LED, HIGH);
    delay(200);
    digitalWrite(LED, LOW);
    delay(200);
    countflashes++;
  }
}

// fridge has been open too long now
void getupset() {
  while (digitalRead(InterruptPin) == HIGH) {
    tone(BUZZ, 1500, 500);
    delay(600);
    tone(BUZZ, 1000, 500);
    delay(600);
  }
}

void loop () {
 
  // call the function that sleeps
  powerDown();

  // set pins to output and let user know fridge is open
  pinMode(LED, OUTPUT);
  pinMode(BUZZ, OUTPUT);
  flashabit(15);

  // fridge still open?? Sound the alarm!
  if (digitalRead(InterruptPin) == HIGH) {
    digitalWrite(LED, HIGH);
    getupset();
  }

  digitalWrite(LED, LOW);
}
