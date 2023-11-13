#include "effects.h"

#include "elapsedMillis.h"

DEFINE_GRADIENT_PALETTE(heatmap_gp){
    0, 255, 0, 0,   // red
    85, 0, 255, 0,  // green
    170, 0, 0, 255, // blue
    255, 255, 0, 0  // red
};

CRGBPalette16 my_pal = heatmap_gp;

static uint8_t pallette_index = 0;

static void rgbFunc(CRGB leds[], uint8_t num_leds)
{
    // All of the pallete gets applies with this
    // fill_palette(leds, num_leds, pallette_index, 255 / num_leds, my_pal, 255, LINEARBLEND);

    for (size_t i = 0; i < num_leds; i++)
    {
        leds[i] = ColorFromPalette(my_pal, pallette_index, 255, LINEARBLEND);
    }

    EVERY_N_MILLISECONDS(2)
    {
        pallette_index++;
        pallette_index %= 255;
    }
}

Effect rgb = {EffectId::Static, &rgbFunc};