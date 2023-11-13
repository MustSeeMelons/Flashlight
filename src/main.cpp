#include <FastLED.h>
#include <Arduino.h>
#include "effects.h"
#include <Bounce2.h>

// FastLED stuffs
#define DI_PIN_ONE 3
#define BRIGHTNESS 255
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
#define UPDATED_PER_SECOND 100

// Brown out shutdown
#define BOD_PIN 4

#define BTN_PIN 5

// Basic led arr seup
const int led_count = 9;
CRGB leds[led_count];

// All the effects!
extern Effect staticWhite;
extern Effect ticTacToe;
extern Effect rgb;
extern Effect loaderEffect;

const uint8_t effectCount = 4;
uint8_t effectIndex = 0;
Effect *effectArr[effectCount] = {&staticWhite, &rgb, &ticTacToe, &loaderEffect};

// Button things
Bounce bounce = Bounce();

Effect *currEffect = effectArr[effectIndex];

void setup()
{
    bounce.attach(BTN_PIN, INPUT_PULLUP);
    bounce.interval(50);

    randomSeed(analogRead(0));

    pinMode(BOD_PIN, OUTPUT);

    // Tweaked due to the intensity not being the same for each: https://www.inolux-corp.com/datasheet/SMDLED/Addressable%20LED/IN-PI554FCH.pdf
    FastLED.addLeds<LED_TYPE, DI_PIN_ONE, COLOR_ORDER>(leds, led_count).setCorrection({70, 70, 255}); // BAT
    // FastLED.addLeds<LED_TYPE, DI_PIN_ONE, COLOR_ORDER>(leds, led_count).setCorrection({128, 128, 240}); // 5V

    digitalWrite(BOD_PIN, HIGH);

    FastLED.setBrightness(BRIGHTNESS);
}

void loop()
{
    bounce.update();

    if (bounce.changed())
    {
        int deboucedInput = bounce.read();

        if (deboucedInput == HIGH)
        {
            effectIndex++;
            effectIndex %= effectCount;
            currEffect = effectArr[effectIndex];
        }
    }

    currEffect->process(leds, led_count);

    FastLED.show();
}