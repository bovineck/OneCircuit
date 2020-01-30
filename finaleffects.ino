#include "FastLED.h"
#include <EEPROM.h>
#define NUM_LEDS 100
CRGB leds[NUM_LEDS];
#define PIN 4
uint8_t gHue = 0;
#define FORWARD 0
#define BACKWARD 1
#define SLOW 250
#define MEDIUM 50
#define FAST 5

void setup()
{
    FastLED.addLeds<WS2811, PIN, RGB>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
}

void loop() {

    showeyes(2);
    RGBLoop();
    confetti();
    FadeInOut(0xff, 0x00, 0x00); // red
    for (int i = 0; i < 2; i++) {
        cylon(randomColor(), 10, FAST);
    }
    FadeInOut(0xff, 0xff, 0xff); // white
    Strobe(0xff, 0xff, 0xff, 10, 50, 1000);
    CylonBounce(0xff, 0x00, 0x00, 4, 10, 50);
    NewKITT(0xff, 0x00, 0x00, 8, 10, 50);
    Twinkle(0xff, 0x00, 0x00, 10, 100, false);
    TwinkleRandom(20, 100, false);
    FadeInOut(0x00, 0xff, 0x00); // white
    sinelon();
    allRandom();
    bpm();
    showeyes(3);
    juggle();
    Sparkle(0xff, 0xff, 0xff, 0);
    SnowSparkle(0x10, 0x10, 0x10, 20, random(100, 1000));
    RunningLights(0xff, 0x00, 0x00, 80); // red
    RunningLights(0xff, 0xff, 0xff, 80); // white
    lightning(NULL, 15, 50, MEDIUM);
    FadeInOut(0x00, 0x00, 0xff); // blue
    colorWipe(0x00, 0xff, 0x00, 50);
    colorWipe(0x00, 0x00, 0x00, 80);
    rainbowCycle(20);
    for (int i = 0; i < 3; i++) {
        CRGB c1 = randomColor();
        CRGB c2 = randomColor();
        stripes(c1, c2, 5);
        delay(2000);
        stripes(c2, c1, 5);
        delay(2000);
    }
    RunningLights(0x00, 0xff, 0x00, 80); // blue
    theaterChase(0xff, 0, 0, 60);
    theaterChaseRainbow(50);
    Fire(55, 120, 15);
    byte onecolor[1][3] = { {0xff, 0x00, 0x00} };
    BouncingColoredBalls(1, onecolor, false);
    lightning(CRGB::White, 20, 50, MEDIUM);
    RunningLights(0x00, 0x00, 0xff, 80); // blue
    showeyes(1);
    byte colors[3][3] = { {0xff, 0x00, 0x00},
        {0xff, 0xff, 0xff},
        {0x00, 0x00, 0xff}
    };
    BouncingColoredBalls(3, colors, false);
    meteorRain(0xff, 0xff, 0xff, 10, 64, true, 30);
}

// *************************
// ** LEDEffect Functions **
// *************************

void lightning(CRGB c, int simultaneous, int cycles, int speed) {
    int flashes[simultaneous];

    for (int i = 0; i < cycles; i++) {
        for (int j = 0; j < simultaneous; j++) {
            int idx = random(NUM_LEDS);
            flashes[j] = idx;
            leds[idx] = c ? c : randomColor();
        }
        FastLED.show();
        delay(speed);
        for (int s = 0; s < simultaneous; s++) {
            leds[flashes[s]] = CRGB::Black;
        }
        delay(speed);
    }
}

void allRandom() {
    for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = randomColor();
    }
    FastLED.show();
}

void showeyes(byte colour) {
    int timeshown = random(50, 200);
    int numbershown = random(5, 15);

    for (int showthem = 0; showthem < numbershown; showthem++) {
        if (colour == 1) {
            HalloweenEyes(0xff, 0x00, 0x00,
                          1, 4,
                          true, random(5, 40), random(50, 100),
                          random(50, 200));
        }
        if (colour == 2) {
            HalloweenEyes(0xff, 0xff, 0x00,
                          1, 4,
                          true, random(5, 40), random(50, 100),
                          random(50, 200));
        }
        if (colour == 3) {
            HalloweenEyes(0x00, 0x00, 0xff,
                          1, 4,
                          true, random(5, 40), random(50, 100),
                          random(50, 200));
        }
    }
}

void RGBLoop() {
    for (int j = 0; j < 3; j++ ) {
        // Fade IN
        for (int k = 0; k < 256; k++) {
            switch (j) {
                case 0: setAll(k, 0, 0); break;
                case 1: setAll(0, k, 0); break;
                case 2: setAll(0, 0, k); break;
            }
            showStrip();
            delay(3);
        }
        // Fade OUT
        for (int k = 255; k >= 0; k--) {
            switch (j) {
                case 0: setAll(k, 0, 0); break;
                case 1: setAll(0, k, 0); break;
                case 2: setAll(0, 0, k); break;
            }
            showStrip();
            delay(3);
        }
    }
}

void FadeInOut(byte red, byte green, byte blue) {
    float r, g, b;

    for (int k = 0; k < 256; k = k + 1) {
        r = (k / 256.0) * red;
        g = (k / 256.0) * green;
        b = (k / 256.0) * blue;
        setAll(r, g, b);
        showStrip();
    }

    for (int k = 255; k >= 0; k = k - 2) {
        r = (k / 256.0) * red;
        g = (k / 256.0) * green;
        b = (k / 256.0) * blue;
        setAll(r, g, b);
        showStrip();
    }
}

void Strobe(byte red, byte green, byte blue, int StrobeCount, int FlashDelay, int EndPause) {
    for (int j = 0; j < StrobeCount; j++) {
        setAll(red, green, blue);
        showStrip();
        delay(FlashDelay);
        setAll(0, 0, 0);
        showStrip();
        delay(FlashDelay);
    }

    delay(EndPause);
}

void HalloweenEyes(byte red, byte green, byte blue,
                   int EyeWidth, int EyeSpace,
                   boolean Fade, int Steps, int FadeDelay,
                   int EndPause) {
    randomSeed(analogRead(0));

    int i;
    int StartPoint  = random( 0, NUM_LEDS - (2 * EyeWidth) - EyeSpace );
    int Start2ndEye = StartPoint + EyeWidth + EyeSpace;

    for (i = 0; i < EyeWidth; i++) {
        setPixel(StartPoint + i, red, green, blue);
        setPixel(Start2ndEye + i, red, green, blue);
    }

    showStrip();

    if (Fade == true) {
        float r, g, b;

        for (int j = Steps; j >= 0; j--) {
            r = j * (red / Steps);
            g = j * (green / Steps);
            b = j * (blue / Steps);

            for (i = 0; i < EyeWidth; i++) {
                setPixel(StartPoint + i, r, g, b);
                setPixel(Start2ndEye + i, r, g, b);
            }

            showStrip();
            delay(FadeDelay);
        }
    }

    setAll(0, 0, 0); // Set all black

    delay(EndPause);
}

void cylon(CRGB c, int width, int speed) {
    // First slide the leds in one direction
    for (int i = 0; i <= NUM_LEDS - width; i++) {
        for (int j = 0; j < width; j++) {
            leds[i + j] = c;
        }

        FastLED.show();

        // now that we've shown the leds, reset to black for next loop
        for (int j = 0; j < 5; j++) {
            leds[i + j] = CRGB::Black;
        }
        delay(speed);
    }
}

void CylonBounce(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {

    for (int i = 0; i < NUM_LEDS - EyeSize - 2; i++) {
        setAll(0, 0, 0);
        setPixel(i, red / 10, green / 10, blue / 10);
        for (int j = 1; j <= EyeSize; j++) {
            setPixel(i + j, red, green, blue);
        }
        setPixel(i + EyeSize + 1, red / 10, green / 10, blue / 10);
        showStrip();
        delay(SpeedDelay);
    }

    delay(ReturnDelay);

    for (int i = NUM_LEDS - EyeSize - 2; i > 0; i--) {
        setAll(0, 0, 0);
        setPixel(i, red / 10, green / 10, blue / 10);
        for (int j = 1; j <= EyeSize; j++) {
            setPixel(i + j, red, green, blue);
        }
        setPixel(i + EyeSize + 1, red / 10, green / 10, blue / 10);
        showStrip();
        delay(SpeedDelay);
    }

    delay(ReturnDelay);
}

void NewKITT(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {
    RightToLeft(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
    LeftToRight(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
    OutsideToCenter(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
    CenterToOutside(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
    LeftToRight(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
    RightToLeft(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
    OutsideToCenter(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
    CenterToOutside(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
}

void CenterToOutside(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {
    for (int i = ((NUM_LEDS - EyeSize) / 2); i >= 0; i--) {
        setAll(0, 0, 0);

        setPixel(i, red / 10, green / 10, blue / 10);
        for (int j = 1; j <= EyeSize; j++) {
            setPixel(i + j, red, green, blue);
        }
        setPixel(i + EyeSize + 1, red / 10, green / 10, blue / 10);

        setPixel(NUM_LEDS - i, red / 10, green / 10, blue / 10);
        for (int j = 1; j <= EyeSize; j++) {
            setPixel(NUM_LEDS - i - j, red, green, blue);
        }
        setPixel(NUM_LEDS - i - EyeSize - 1, red / 10, green / 10, blue / 10);

        showStrip();
        delay(SpeedDelay);
    }
    delay(ReturnDelay);
}

void OutsideToCenter(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {
    for (int i = 0; i <= ((NUM_LEDS - EyeSize) / 2); i++) {
        setAll(0, 0, 0);

        setPixel(i, red / 10, green / 10, blue / 10);
        for (int j = 1; j <= EyeSize; j++) {
            setPixel(i + j, red, green, blue);
        }
        setPixel(i + EyeSize + 1, red / 10, green / 10, blue / 10);

        setPixel(NUM_LEDS - i, red / 10, green / 10, blue / 10);
        for (int j = 1; j <= EyeSize; j++) {
            setPixel(NUM_LEDS - i - j, red, green, blue);
        }
        setPixel(NUM_LEDS - i - EyeSize - 1, red / 10, green / 10, blue / 10);

        showStrip();
        delay(SpeedDelay);
    }
    delay(ReturnDelay);
}

void LeftToRight(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {
    for (int i = 0; i < NUM_LEDS - EyeSize - 2; i++) {
        setAll(0, 0, 0);
        setPixel(i, red / 10, green / 10, blue / 10);
        for (int j = 1; j <= EyeSize; j++) {
            setPixel(i + j, red, green, blue);
        }
        setPixel(i + EyeSize + 1, red / 10, green / 10, blue / 10);
        showStrip();
        delay(SpeedDelay);
    }
    delay(ReturnDelay);
}

void RightToLeft(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {
    for (int i = NUM_LEDS - EyeSize - 2; i > 0; i--) {
        setAll(0, 0, 0);
        setPixel(i, red / 10, green / 10, blue / 10);
        for (int j = 1; j <= EyeSize; j++) {
            setPixel(i + j, red, green, blue);
        }
        setPixel(i + EyeSize + 1, red / 10, green / 10, blue / 10);
        showStrip();
        delay(SpeedDelay);
    }
    delay(ReturnDelay);
}

void Twinkle(byte red, byte green, byte blue, int Count, int SpeedDelay, boolean OnlyOne) {
    setAll(0, 0, 0);

    for (int i = 0; i < Count; i++) {
        setPixel(random(NUM_LEDS), red, green, blue);
        showStrip();
        delay(SpeedDelay);
        if (OnlyOne) {
            setAll(0, 0, 0);
        }
    }

    delay(SpeedDelay);
}

void TwinkleRandom(int Count, int SpeedDelay, boolean OnlyOne) {
    setAll(0, 0, 0);

    for (int i = 0; i < Count; i++) {
        setPixel(random(NUM_LEDS), random(0, 255), random(0, 255), random(0, 255));
        showStrip();
        delay(SpeedDelay);
        if (OnlyOne) {
            setAll(0, 0, 0);
        }
    }

    delay(SpeedDelay);
}

void Sparkle(byte red, byte green, byte blue, int SpeedDelay) {
    int Pixel = random(NUM_LEDS);
    setPixel(Pixel, red, green, blue);
    showStrip();
    delay(SpeedDelay);
    setPixel(Pixel, 0, 0, 0);
}

void SnowSparkle(byte red, byte green, byte blue, int SparkleDelay, int SpeedDelay) {
    setAll(red, green, blue);

    int Pixel = random(NUM_LEDS);
    setPixel(Pixel, 0xff, 0xff, 0xff);
    showStrip();
    delay(SparkleDelay);
    setPixel(Pixel, red, green, blue);
    showStrip();
    delay(SpeedDelay);
}

void RunningLights(byte red, byte green, byte blue, int WaveDelay) {
    int Position = 0;

    for (int i = 0; i < NUM_LEDS * 2; i++)
    {
        Position++; // = 0; //Position + Rate;
        for (int i = 0; i < NUM_LEDS; i++) {
            // sine wave, 3 offset waves make a rainbow!
            //float level = sin(i+Position) * 127 + 128;
            //setPixel(i,level,0,0);
            //float level = sin(i+Position) * 127 + 128;
            setPixel(i, ((sin(i + Position) * 127 + 128) / 255)*red,
                     ((sin(i + Position) * 127 + 128) / 255)*green,
                     ((sin(i + Position) * 127 + 128) / 255)*blue);
        }
        showStrip();
        delay(WaveDelay);
    }
}

void colorWipe(byte red, byte green, byte blue, int SpeedDelay) {
    for (uint16_t i = 0; i < NUM_LEDS; i++) {
        setPixel(i, red, green, blue);
        showStrip();
        delay(SpeedDelay);
    }
}

void rainbowCycle(int SpeedDelay) {
    byte *c;
    uint16_t i, j;

    for (j = 0; j < 256 * 5; j++) { // 5 cycles of all colors on wheel
        for (i = 0; i < NUM_LEDS; i++) {
            c = Wheel(((i * 256 / NUM_LEDS) + j) & 255);
            setPixel(i, *c, *(c + 1), *(c + 2));
        }
        showStrip();
        delay(SpeedDelay);
    }
}

byte * Wheel(byte WheelPos) {
    static byte c[3];

    if (WheelPos < 85) {
        c[0] = WheelPos * 3;
        c[1] = 255 - WheelPos * 3;
        c[2] = 0;
    } else if (WheelPos < 170) {
        WheelPos -= 85;
        c[0] = 255 - WheelPos * 3;
        c[1] = 0;
        c[2] = WheelPos * 3;
    } else {
        WheelPos -= 170;
        c[0] = 0;
        c[1] = WheelPos * 3;
        c[2] = 255 - WheelPos * 3;
    }
    return c;
}

void theaterChase(byte red, byte green, byte blue, int SpeedDelay) {
    for (int j = 0; j < 10; j++) { //do 10 cycles of chasing
        for (int q = 0; q < 3; q++) {
            for (int i = 0; i < NUM_LEDS; i = i + 3) {
                setPixel(i + q, red, green, blue);  //turn every third pixel on
            }
            showStrip();

            delay(SpeedDelay);

            for (int i = 0; i < NUM_LEDS; i = i + 3) {
                setPixel(i + q, 0, 0, 0);    //turn every third pixel off
            }
        }
    }
}

void theaterChaseRainbow(int SpeedDelay) {
    byte *c;

    for (int j = 0; j < 256; j++) {   // cycle all 256 colors in the wheel
        for (int q = 0; q < 3; q++) {
            for (int i = 0; i < NUM_LEDS; i = i + 3) {
                c = Wheel( (i + j) % 255);
                setPixel(i + q, *c, *(c + 1), *(c + 2)); //turn every third pixel on
            }
            showStrip();

            delay(SpeedDelay);

            for (int i = 0; i < NUM_LEDS; i = i + 3) {
                setPixel(i + q, 0, 0, 0);    //turn every third pixel off
            }
        }
    }
}

void Fire(int Cooling, int Sparking, int SpeedDelay) {
    static byte heat[NUM_LEDS];
    int cooldown;

    for ( int i = 0; i < NUM_LEDS; i++) {
        cooldown = random(0, ((Cooling * 10) / NUM_LEDS) + 2);

        if (cooldown > heat[i]) {
            heat[i] = 0;
        } else {
            heat[i] = heat[i] - cooldown;
        }
    }

    for ( int k = NUM_LEDS - 1; k >= 2; k--) {
        heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
    }

    if ( random(255) < Sparking ) {
        int y = random(7);
        heat[y] = heat[y] + random(160, 255);
        //heat[y] = random(160,255);
    }

    for ( int j = 0; j < NUM_LEDS; j++) {
        setPixelHeatColor(j, heat[j] );
    }

    showStrip();
    delay(SpeedDelay);
}

void setPixelHeatColor (int Pixel, byte temperature) {
    byte t192 = round((temperature / 255.0) * 191);
    byte heatramp = t192 & 0x3F;
    heatramp <<= 2;
    if ( t192 > 0x80) {
        setPixel(Pixel, 255, 255, heatramp);
    } else if ( t192 > 0x40 ) {
        setPixel(Pixel, 255, heatramp, 0);
    } else {
        setPixel(Pixel, heatramp, 0, 0);
    }
}

void BouncingColoredBalls(int BallCount, byte colors[][3], boolean continuous) {
    float Gravity = -9.81;
    int StartHeight = 1;
    float Height[BallCount];
    float ImpactVelocityStart = sqrt( -2 * Gravity * StartHeight );
    float ImpactVelocity[BallCount];
    float TimeSinceLastBounce[BallCount];
    int   Position[BallCount];
    long  ClockTimeSinceLastBounce[BallCount];
    float Dampening[BallCount];
    boolean ballBouncing[BallCount];
    boolean ballsStillBouncing = true;

    for (int i = 0 ; i < BallCount ; i++) {
        ClockTimeSinceLastBounce[i] = millis();
        Height[i] = StartHeight;
        Position[i] = 0;
        ImpactVelocity[i] = ImpactVelocityStart;
        TimeSinceLastBounce[i] = 0;
        Dampening[i] = 0.90 - float(i) / pow(BallCount, 2);
        ballBouncing[i] = true;
    }

    while (ballsStillBouncing) {
        for (int i = 0 ; i < BallCount ; i++) {
            TimeSinceLastBounce[i] =  millis() - ClockTimeSinceLastBounce[i];
            Height[i] = 0.5 * Gravity * pow( TimeSinceLastBounce[i] / 1000 , 2.0 ) + ImpactVelocity[i] * TimeSinceLastBounce[i] / 1000;

            if ( Height[i] < 0 ) {
                Height[i] = 0;
                ImpactVelocity[i] = Dampening[i] * ImpactVelocity[i];
                ClockTimeSinceLastBounce[i] = millis();

                if ( ImpactVelocity[i] < 0.01 ) {
                    if (continuous) {
                        ImpactVelocity[i] = ImpactVelocityStart;
                    } else {
                        ballBouncing[i] = false;
                    }
                }
            }
            Position[i] = round( Height[i] * (NUM_LEDS - 1) / StartHeight);
        }

        ballsStillBouncing = false; // assume no balls bouncing
        for (int i = 0 ; i < BallCount ; i++) {
            setPixel(Position[i], colors[i][0], colors[i][1], colors[i][2]);
            if ( ballBouncing[i] ) {
                ballsStillBouncing = true;
            }
        }
        showStrip();
        setAll(0, 0, 0);
    }
}

void meteorRain(byte red, byte green, byte blue, byte meteorSize, byte meteorTrailDecay, boolean meteorRandomDecay, int SpeedDelay) {
    setAll(0, 0, 0);

    for (int i = 0; i < NUM_LEDS + NUM_LEDS; i++) {
        for (int j = 0; j < NUM_LEDS; j++) {
            if ( (!meteorRandomDecay) || (random(10) > 5) ) {
                fadeToBlack(j, meteorTrailDecay );
            }
        }
        for (int j = 0; j < meteorSize; j++) {
            if ( ( i - j < NUM_LEDS) && (i - j >= 0) ) {
                setPixel(i - j, red, green, blue);
            }
        }
        showStrip();
        delay(SpeedDelay);
    }
}

void fadeToBlack(int ledNo, byte fadeValue) {
#ifdef ADAFRUIT_NEOPIXEL_H
    // NeoPixel
    uint32_t oldColor;
    uint8_t r, g, b;
    int value;

    oldColor = strip.getPixelColor(ledNo);
    r = (oldColor & 0x00ff0000UL) >> 16;
    g = (oldColor & 0x0000ff00UL) >> 8;
    b = (oldColor & 0x000000ffUL);

    r = (r <= 10) ? 0 : (int) r - (r * fadeValue / 256);
    g = (g <= 10) ? 0 : (int) g - (g * fadeValue / 256);
    b = (b <= 10) ? 0 : (int) b - (b * fadeValue / 256);

    strip.setPixelColor(ledNo, r, g, b);
#endif

#ifndef ADAFRUIT_NEOPIXEL_H
    leds[ledNo].fadeToBlackBy( fadeValue );
#endif
}

void confetti()
{
    // random colored speckles that blink in and fade smoothly
    fadeToBlackBy( leds, NUM_LEDS, 10);
    int pos = random16(NUM_LEDS);
    leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon()
{
    // a colored dot sweeping back and forth, with fading trails
    fadeToBlackBy( leds, NUM_LEDS, 20);
    int pos = beatsin16(13, 0, NUM_LEDS);
    leds[pos] += CHSV( gHue, 255, 192);
}

void bpm()
{
    // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
    uint8_t BeatsPerMinute = 62;
    CRGBPalette16 palette = PartyColors_p;
    uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
    for ( int i = 0; i < NUM_LEDS; i++) { //9948
        leds[i] = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
    }
}

void juggle() {
    // eight colored dots, weaving in and out of sync with each other
    fadeToBlackBy( leds, NUM_LEDS, 20);
    byte dothue = 0;
    for ( int i = 0; i < 8; i++) {
        leds[beatsin16(i + 7, 0, NUM_LEDS)] |= CHSV(dothue, 200, 255);
        dothue += 32;
    }
}

void stripes(CRGB c1, CRGB c2, int width) {
    for (int i = 0; i < NUM_LEDS; i++) {
        if (i % (width * 2) < width) {
            leds[i] = c1;
        }
        else {
            leds[i] = c2;
        }
    }
    FastLED.show();
}

CRGB randomColor() {
    return Wheel(random(256));
}

// ***************************************
// ** FastLed/NeoPixel Common Functions **
// ***************************************

void showStrip() {
#ifdef ADAFRUIT_NEOPIXEL_H
    // NeoPixel
    strip.show();
#endif
#ifndef ADAFRUIT_NEOPIXEL_H
    // FastLED
    FastLED.show();
#endif
}

void setPixel(int Pixel, byte red, byte green, byte blue) {
#ifdef ADAFRUIT_NEOPIXEL_H
    // NeoPixel
    strip.setPixelColor(Pixel, strip.Color(red, green, blue));
#endif
#ifndef ADAFRUIT_NEOPIXEL_H
    // FastLED
    leds[Pixel].r = red;
    leds[Pixel].g = green;
    leds[Pixel].b = blue;
#endif
}

void setAll(byte red, byte green, byte blue) {
    for (int i = 0; i < NUM_LEDS; i++ ) {
        setPixel(i, red, green, blue);
    }
    showStrip();
}
