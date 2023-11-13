#ifndef effects_h
#define effects_h

#include <FastLED.h>

enum EffectId
{
    Static = 0,
    TicTacToe = 1,
    RGBId = 2,
    Loader = 3
};

struct Effect
{
    uint8_t id;
    void (*process)(CRGB leds[], uint8_t num_leds);
};

#endif