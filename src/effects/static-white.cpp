#include "effects.h"

#include "elapsedMillis.h"

static elapsedMillis stamp;
static bool isFirst = true;

static void white(CRGB leds[], uint8_t num_leds)
{
    if (stamp > 1000 || isFirst)
    {
        fill_solid(leds, num_leds, CRGB::White);
        isFirst = false;
        stamp = 0;
    }
}

Effect staticWhite = {EffectId::Static, &white};