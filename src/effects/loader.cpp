#include "effects.h"

#include "elapsedMillis.h"

static elapsedMillis stamp;

static uint8_t seqIndex = 0;
static const uint8_t seqCount = 8;
static uint8_t seqOrder[seqCount] = {0, 1, 2, 3, 8, 7, 6, 5};
static const uint8_t ledCount = 9;

static CHSV black = rgb2hsv_approximate(CRGB::Black);
static CHSV targetValues[ledCount] = {black, black, black, black, black, black, black, black, black};
static bool isFadeIn[ledCount] = {false, false, false, false, false, false, false, false, false};
static int16_t brightness[ledCount] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
static uint8_t step = 2;

static void fancyLoader(CRGB leds[], uint8_t num_leds)
{
    leds[4] = CRGB::Black;

    // Set a targter color we wish to reach
    if (stamp > 100)
    {
        stamp = 0;
        targetValues[seqOrder[seqIndex]] = rgb2hsv_approximate(CRGB::RosyBrown);
        isFadeIn[seqOrder[seqIndex]] = true;
        seqIndex++;
        seqIndex %= seqCount;
    }

    // Fade in target values
    for (size_t i = 0; i < ledCount; i++)
    {
        if (isFadeIn[i])
        {
            CHSV c = targetValues[i];

            if (brightness[i] + step > 255)
            {
                brightness[i] = 255;
            }
            else
            {
                brightness[i] += step;
            }

            leds[i] = CHSV(c.h, c.v, brightness[i]);

            if (brightness[i] == 255)
            {
                isFadeIn[i] = false;
            }
        }
    }

    // Fade out others
    for (size_t i = 0; i < ledCount; i++)
    {
        if (!isFadeIn[i] && brightness[i] != 0)
        {
            CHSV c = targetValues[i];

            if (brightness[i] - step < 0)
            {
                brightness[i] = 0;
            }
            else
            {
                brightness[i] -= step;
            }

            leds[i] = CHSV(c.h, c.v, brightness[i]);
            brightness[i] -= step;

            if (brightness[i] == 0)
            {
                isFadeIn[i] = false;
            }
        }
    }
};

Effect loaderEffect = {EffectId::Loader, &fancyLoader};