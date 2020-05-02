// Charlieplexing 12 LEDs with an Attiny13
// A N Peck May 2020

const byte looper = 5;         // how many times for each effect?
const byte betweenies = 1000;  // delay time between effects
uint16_t myrand = 2901;        // initial value for random seed

// the LEDs port direction and portstate 12 x 2 array
const byte leds12 [12][2] =

{
    {0b00000110, 0b00000010}, // LED a
    {0b00000110, 0b00000100}, // LED b
    {0b00001100, 0b00000100}, // LED c
    {0b00001100, 0b00001000}, // LED d
    {0b00011000, 0b00001000}, // LED e
    {0b00011000, 0b00010000}, // LED f
    {0b00001010, 0b00000010}, // LED g
    {0b00001010, 0b00001000}, // LED h
    {0b00010100, 0b00000100}, // LED i
    {0b00010100, 0b00010000}, // LED j
    {0b00010010, 0b00000010}, // LED k
    {0b00010010, 0b00010000}, // LED l
};

// take the port direction, port state and delay to make the LEDs light up
void lightemup (byte portdirection, byte portstate, int mydelay) {
    DDRB = portdirection;
    PORTB = portstate;
    delay(mydelay);
}

// a little bit of zero led activity if needed
void deadled (int howlongdead) {
    lightemup(0b00000000, 0b00000000, howlongdead);
}

void setup() {

    // a memory blowout in the next two lines to initialise random number
    pinMode(PB1, INPUT);
    myrand = analogRead(PB1);
}

// linear feedback shift register "randomiser"
uint16_t gimmerand(uint16_t small, uint16_t big) {
    myrand ^= (myrand << 13);
    myrand ^= (myrand >> 9);
    myrand ^= (myrand << 7);
    return abs(myrand) % 23 * (big - small) / 23 + small;
}

// random light effect
void randomleds() {

    for (byte theloops = 0; theloops < 100; theloops++) {
        byte thislight = gimmerand(0, 12);
        lightemup(leds12[thislight][0], leds12[thislight][1], 50);

        deadled(50);
    }
}

// moving single led backwards and forwards
void cylons() {

    for (byte theloops = 0; theloops < looper; theloops++) {
        for (byte outercount = 0; outercount < 12; outercount++) {
            lightemup(leds12[outercount][0], leds12[outercount][1], 25);
            deadled(25);
        }
        for (byte outercount = 10; outercount > 0; outercount--) {
            lightemup(leds12[outercount][0], leds12[outercount][1], 25);
            deadled(25);
        }
    }
    lightemup(leds12[0][0], leds12[0][1], 25);
    deadled(25);
}

// moving two leds backwards and forwards together
void twobytwo() {
    for (byte theloops = 0; theloops < looper; theloops++) {
        for (byte outercount = 0; outercount < 10; outercount++) {
            for (byte innercount = 0; innercount < 40; innercount++) {
                lightemup(leds12[outercount][0], leds12[outercount][1], 1);
                lightemup(leds12[outercount + 2][0], leds12[outercount + 2][1], 1);
            }
        }
        for (byte outercount = 8; outercount > 1; outercount--) {
            for (byte innercount = 0; innercount < 40; innercount++) {
                lightemup(leds12[outercount][0], leds12[outercount][1], 1);
                lightemup(leds12[outercount + 2][0], leds12[outercount + 2][1], 1);
            }
        }
    }
    for (byte innercount = 0; innercount < 40; innercount++) {
        lightemup(leds12[0][0], leds12[0][1], 1);
        lightemup(leds12[2][0], leds12[2][1], 1);
        deadled(1);
    }
}

// two leds moving from side to side
void backandforthone() {
    for (byte theloops = 0; theloops < looper; theloops++) {
        for (byte outercount = 0; outercount < 12; outercount++) {
            for (byte innercount = 0; innercount < 40; innercount++) {
                lightemup(leds12[outercount][0], leds12[outercount][1], 1);
                lightemup(leds12[11 - outercount][0], leds12[11 - outercount][1], 1);
            }
        }
        deadled(1);
    }
}

// four leds moving from side to side
void backandforthtwo() {
    for (byte theloops = 0; theloops < looper; theloops++) {
        for (byte outercount = 0; outercount < 11; outercount++) {
            for (byte innercount = 0; innercount < 20; innercount++) {
                lightemup(leds12[outercount][0], leds12[outercount][1], 1);
                lightemup(leds12[outercount + 1][0], leds12[outercount + 1][1], 1);
                lightemup(leds12[10 - outercount][0], leds12[10 - outercount][1], 1);
                lightemup(leds12[11 - outercount][0], leds12[11 - outercount][1], 1);
            }

        }
        deadled(1);
    }
}

// choose effect
void loop() {
    randomleds();
    delay(betweenies);
    cylons();
    delay(betweenies);
    twobytwo();
    delay(betweenies);
    backandforthone();
    delay(betweenies);
    backandforthtwo();
    delay(betweenies);
}
