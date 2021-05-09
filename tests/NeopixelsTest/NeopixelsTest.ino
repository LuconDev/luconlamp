/* NeopixelsTest.ino
  Dec 2020
  AdafruitNeopixel Library 1.7.0
  Adafruit NeoPixel Digital RGBW LED Strip https://www.adafruit.com/product/2832?length=1
  Reference: https://github.com/adafruit/Adafruit_NeoPixel/blob/master/examples/RGBWstrandtest/RGBWstrandtest.ino
*/

#include <Adafruit_NeoPixel.h>

// Neopixel constants
#define NUMPIXEL_COUNT 3       // Number of LEDs in strip
#define NEOPIXEL_PIN 2    // D4 NodeMCU
#define MID_BRIGHTNESS 30
#define MAX_BRIGHTNESS 255

//while Adafruit claims this strip to be RGBW, I actually found the strip to behave as GRBW
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXEL_COUNT, NEOPIXEL_PIN, NEO_RGBW + NEO_KHZ800);
uint32_t green = strip.Color(255,   0,   0);
uint32_t red = strip.Color(0,   255,   0);
uint32_t blue = strip.Color(0,   0,   255);
uint32_t trueWhite = strip.Color(0,   0,   0, 255);


void setup() {
  // initialize neopixel strip
  strip.begin(); // Initialize pins for output
  strip.clear();
  strip.show();  // Turn all LEDs off ASAP
  strip.setBrightness(50);
}

void loop() {
  // Fill along the length of the strip in various colors...
  colorWipe(red, 50); // Red
  colorWipe(green, 50); // Green
  colorWipe(blue , 50); // Blue
  colorWipe(trueWhite, 50); // True white (not RGB white)
  delay(2000);
  whiteOverRainbow(75, 5);
  delay(2000);
  pulseWhite(10);
  delay(100);
  pulseGreen(0);
  pulseGreen(0);
  pulseGreen(0);
  delay(2000);
  rainbowFade2White(3, 3, 1);
  delay(2000);
}

void pulseWhite(uint8_t wait) {
  for (int w = 0; w < 256; w++) { // Ramp up from 0 to 255
    // Fill entire strip with white at gamma-corrected brightness level 'j':
    strip.fill(strip.Color(0, 0, 0, strip.gamma8(w)));
    strip.show();
    delay(wait);
  }
  for (int w = 255; w >= 0; w--) { // Ramp down from 255 to 0
    strip.fill(strip.Color(0, 0, 0, strip.gamma8(w)));
    strip.show();
    delay(wait);
  }
}

void pulseGreen(uint8_t wait) {
  for (int g = 0; g < 256; g++) { // Ramp up from 0 to 255
    strip.fill(strip.Color(g, 0, 0, 0));
    strip.show();
    delay(wait);
  }
  for (int g = 255; g >= 0; g--) { // Ramp down from 255 to 0
    strip.fill(strip.Color(g, 0, 0, 0));
    strip.show();
    delay(wait);
  }
}
void setWhiteOn(uint8_t wait) {
  for (int w = 0; w < 256; w++) { // Ramp up from 0 to 255
    // Fill entire strip with white at gamma-corrected brightness level 'w':
    strip.fill(strip.Color(0, 0, 0, strip.gamma8(w)));
    strip.show();
    delay(wait);
  }
}
void setWhiteOff(uint8_t wait) {
  for (int w = 255; w >= 0; w--) { // Ramp down from 255 to 0
    strip.fill(strip.Color(0, 0, 0, strip.gamma8(w)));
    strip.show();
    delay(wait);
  }
}

// Fill strip pixels one after another with a color. Strip is NOT cleared
// first; anything there will be covered pixel by pixel. Pass in color
// (as a single 'packed' 32-bit value, which you can get by calling
// strip.Color(red, green, blue) as shown in the loop() function above),
// and a delay time (in milliseconds) between pixels.
void colorWipe(uint32_t color, int wait) {
  for (int i = 0; i < strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}

void whiteOverRainbow(int whiteSpeed, int whiteLength) {

  if (whiteLength >= strip.numPixels()) whiteLength = strip.numPixels() - 1;

  int      head          = whiteLength - 1;
  int      tail          = 0;
  int      loops         = 3;
  int      loopNum       = 0;
  uint32_t lastTime      = millis();
  uint32_t firstPixelHue = 0;

  for (;;) { // Repeat forever (or until a 'break' or 'return')
    for (int i = 0; i < strip.numPixels(); i++) { // For each pixel in strip...
      if (((i >= tail) && (i <= head)) ||     //  If between head & tail...
          ((tail > head) && ((i >= tail) || (i <= head)))) {
        strip.setPixelColor(i, strip.Color(0, 0, 0, 255)); // Set white
      } else {                                             // else set rainbow
        int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
        strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
      }
    }

    strip.show(); // Update strip with new contents
    // There's no delay here, it just runs full-tilt until the timer and
    // counter combination below runs out.

    firstPixelHue += 40; // Advance just a little along the color wheel

    if ((millis() - lastTime) > whiteSpeed) { // Time to update head/tail?
      if (++head >= strip.numPixels()) {     // Advance head, wrap around
        head = 0;
        if (++loopNum >= loops) return;
      }
      if (++tail >= strip.numPixels()) {     // Advance tail, wrap around
        tail = 0;
      }
      lastTime = millis();                   // Save time of last movement
    }
  }
}

void rainbowFade2White(int wait, int rainbowLoops, int whiteLoops) {
  int fadeVal = 0, fadeMax = 100;

  // Hue of first pixel runs 'rainbowLoops' complete loops through the color
  // wheel. Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to rainbowLoops*65536, using steps of 256 so we
  // advance around the wheel at a decent clip.
  for (uint32_t firstPixelHue = 0; firstPixelHue < rainbowLoops * 65536;
       firstPixelHue += 256) {

    for (int i = 0; i < strip.numPixels(); i++) { // For each pixel in strip...

      // Offset pixel hue by an amount to make one full revolution of the
      // color wheel (range of 65536) along the length of the strip
      // (strip.numPixels() steps):
      uint32_t pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());

      // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
      // optionally add saturation and value (brightness) (each 0 to 255).
      // Here we're using just the three-argument variant, though the
      // second value (saturation) is a constant 255.
      strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue, 255,
                                           255 * fadeVal / fadeMax)));
    }

    strip.show();
    delay(wait);

    if (firstPixelHue < 65536) {                             // First loop,
      if (fadeVal < fadeMax) fadeVal++;                      // fade in
    } else if (firstPixelHue >= ((rainbowLoops - 1) * 65536)) { // Last loop,
      if (fadeVal > 0) fadeVal--;                            // fade out
    } else {
      fadeVal = fadeMax; // Interim loop, make sure fade is at max
    }
  }

  for (int k = 0; k < whiteLoops; k++) {
    for (int j = 0; j < 256; j++) { // Ramp up 0 to 255
      // Fill entire strip with white at gamma-corrected brightness level 'j':
      strip.fill(strip.Color(0, 0, 0, strip.gamma8(j)));
      strip.show();
    }
    delay(1000); // Pause 1 second
    for (int j = 255; j >= 0; j--) { // Ramp down 255 to 0
      strip.fill(strip.Color(0, 0, 0, strip.gamma8(j)));
      strip.show();
    }
  }

  delay(500); // Pause 1/2 second
}
