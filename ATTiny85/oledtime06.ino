// there are other libraries around, but these ones worked for me

#include <TinyI2CMaster.h>  // https://github.com/technoblogy/tiny-i2c
#include <Tiny4kOLED.h>     // https://github.com/datacute/Tiny4kOLED
#include <TimeLib.h>        // https://github.com/PaulStoffregen/Time

// two buttons for timesetting! It seems the external clock signal takes out
// both xtal1 and xtal2 for use unless CKSEL is programmed to 00

#define setplus PCINT1
#define setminus PCINT4

// some starting values for date and time
int myyear = 20;
byte mymonth = 1;
byte myday = 1;
byte myhour = 12;
byte myminute = 0;

// worth playing with if you don't like the setting buttons timing
const unsigned long longbuttonpress = 400;
byte debouncetime = 10;

// have the buttons been pressed - set these as false initially
// need to be volatile as they are in the ISR
volatile boolean pushplus = false;
volatile boolean pushminus = false;

void setup() {

    // initialise oled using library calls
    oled.begin();
    oled.setFont(FONT8X16);
    oled.clear();
    oled.on();
    oled.switchRenderFrame();

    // buttons are input
    pinMode(setplus, INPUT_PULLUP);
    pinMode(setminus, INPUT_PULLUP);

    // values for the interrupt pin (datasheet)
    MCUCR = 0b00000011;    // rising edge
    GIMSK = 0b00100000;    // turns on pin change interrupts
    PCMSK = 0b00000010;    // turn on interrupts on pins PB1

    // start the clock running
    setTime(myhour, myminute , 00, myday, mymonth, myyear + 2000);

    // interrupts on - listen for button
    sei();
}

void loop() {

    // if no button pressed then just keep displaying time
    while (!pushplus) {
        updateDisplay();
    }

    // but if button is pushed let's set the clock
    oled.clear();
    oled.setCursor(0, 0);

    // flashing a message to indicate we are about to set clock
    for (byte count = 0; count < 4; count++) {
        oled.print("**Setting**");
        oled.switchFrame();
        delay(250);
        oled.clear();
        oled.switchFrame();
        delay(100);
    }

    // and let's go off to the set clock routine
    setmytime();
}

// set the clock in order year, month, day, hour, minute - the 
// seconds can be zero at the start

void setmytime() {

    myyear = setthisunit("Year: ", myyear, 40);
    delay(debouncetime);
    mymonth = setthisunit("Month: ", mymonth, 13);
    delay(debouncetime);

    // stupid months with different days!
    if ((mymonth == 1) || (mymonth == 3) || (mymonth == 5) || (mymonth == 7)
            || (mymonth == 8) || (mymonth == 10) || (mymonth == 12)) {
        myday = setthisunit("Day: ", myday, 32);
        delay(debouncetime);
    }
    else if ((mymonth == 4) || (mymonth == 6) || (mymonth == 9)
             || (mymonth == 11)) {
        myday = setthisunit("Day: ", myday, 31);
        delay(debouncetime);
    }
    else if (mymonth == 2) {
        // and crude leap year adjustment (I figure it won't matter to me in 2400)
        if (myyear % 4 == 0) {
            myday = setthisunit("Day: ", myday, 29);
        }
        else {
            myday = setthisunit("Day: ", myday, 30);
        }
    }

    myhour = setthisunit("Hour: ", myhour, 24);
    delay(debouncetime);
    myminute = setthisunit("Minutes: ", myminute, 60);
    delay(debouncetime);

    // done so set that time
    setTime(myhour, myminute , 00, myday, mymonth, myyear + 2000);
    delay(debouncetime);

    // button pressing is false, and turn on interrupts again
    pushplus = false;
    sei();
}

void settingtimedisplay(String settinghow, int settingwhat) {
    oled.clear();
    oled.setCursor(0, 0);
    oled.print("**Setting**");
    oled.setCursor(0, 2);
    oled.print(settinghow);
    printDigits(settingwhat);
    oled.switchFrame();
}

// reusing this code to set each unit - modulus arithmetic to click over hours, mins, etc.,
// there are some obvious compromises, but it all works
int setthisunit(String whattoset, int thisunit, int thismod) {

    boolean longpress = false;

    while (!longpress) {

        // sometimes the rollover is "1" (e.g. month) and sometimes
        // it needs to be "0" (e.g. minutes)
        if (thisunit % thismod == 0) {
            if ((thismod == 40) || (thismod == 60) || (thismod == 24)) {
                thisunit = 0;
            }
            else {
                thisunit = 1;
            }
        }

        // display current unit
        settingtimedisplay(whattoset, thisunit);

        // Is it a short press or a long press?
        if (digitalRead(setplus)) {
            delay(debouncetime);
            if (digitalRead(setplus)) {
                pushplus = true;
                unsigned long startpush = millis();
                while (pushplus) {
                    if (!digitalRead(setplus)) {
                        pushplus = false;
                        if ((millis() - startpush) > longbuttonpress) {
                            longpress = true;
                        }
                        if ((millis() - startpush) < longbuttonpress) {
                            longpress = false;
                        }
                    }
                }

                // not a long press so increment the unit
                if (!longpress) {
                    thisunit++;
                }
            }
        }
        // it's sometimes quicker (e.g. minutes) to set going backwards!
        if (digitalRead(setminus)) {
          delay(debouncetime);
          if (digitalRead(setminus)) {
            if (thisunit>0) {
              thisunit--;
            }
            else {
              thisunit = thismod-1;
            }
          }
        }
    }

    // long press so return the unit
    return thisunit;
}

// interrupt service routine which disables interrupts but has a sneaky debounce as well
ISR(PCINT0_vect) {
    cli();
    delay(50);
    if (digitalRead(setplus)) {
        pushplus = true;
    }
    else {
        pushplus = false;
        sei();
    }
}

// write the time to the OLED (library routines)
void updateDisplay() {
    oled.clear();
    oled.setCursor(0, 0);
    oled.print(day());
    oled.print("/");
    oled.print(month());
    oled.print("/");
    oled.print(year());
    oled.setCursor(0, 2);
    printDigits(hour());
    oled.print(":");
    printDigits(minute());
    oled.print(":");
    printDigits(second());
    oled.switchFrame();
}

// keeping two digits for hours, mins and secs so it looks nice
void printDigits(byte digits) {
    // utility function for digital clock display: prints preceding colon and leading 0
    if (digits < 10)
        oled.print('0');
    oled.print(digits);
}

// end of code
