#include <Adafruit_NeoPixel.h>

#define PIN 6
#define NUM_PIXELS 14

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_PIXELS, PIN, NEO_GRBW + NEO_KHZ800);

uint8_t  mode   = 5, // Starting animation effect
         offset = 0;
uint32_t color  = 0X00A4B3; // Starting color for rainbow
uint32_t prevTime;

//predefined colors
const uint32_t RED = pixels.Color(255, 0, 0);
const uint32_t GREEN = pixels.Color(0, 255, 0);
const uint32_t BLUE = pixels.Color(0, 0, 255);
const uint32_t PINK = pixels.Color(236, 79, 100);
const uint32_t YELLOW = pixels.Color(255, 255, 0);
const uint32_t WHITE = pixels.Color(255, 255, 255);
const uint32_t ORANGE = pixels.Color(255, 165, 0);
const uint32_t PURPLE = pixels.Color(91, 44, 86);
const uint32_t INDIGO = pixels.Color(75, 0, 130);
const uint32_t VIOLET = pixels.Color(238, 130, 238);
const uint32_t OFF = 0;

//color array for random colors
uint32_t ALL_COLORS[] = {RED, GREEN, BLUE, PINK, YELLOW, WHITE, ORANGE, PURPLE, INDIGO, VIOLET};

uint32_t ALL_OFF[7] = {OFF, OFF, OFF, OFF, OFF, OFF, OFF};

//runs once on boot
void setup() {
  //setup the pixels
  pixels.begin();
  pixels.setBrightness(255); // 1 brightness
  prevTime = millis();
}


//runs over and over
void loop() {
  uint8_t  i;
  uint32_t t;

  switch (mode) {

    case 0: //rainbow hold
      rainbowCycle(20, 1, 255);
      delay(500);
      break;

    case 1: //wave
      wave(ALL_COLORS[random(10)], 200);
      break;

    case 2: //rainbow cycle fast
      rainbowCycle(5, 6, 100);
      delay(50);
      break;

    case 3: //explosion
      explosion(ALL_COLORS[random(10)]);
      break;

    case 4: //rainbow cycle slow
      rainbowCycle(20, 3, 100);
      delay(50);
      break;

    case 5: //heartbeat
      heartbeat(RED, 500);
      break;
  }

  t = millis();
  if ((t - prevTime) > 3000) {     // Every 3 seconds...
    mode++;                        // Next mode
    if (mode > 5) {                // End of modes?
      mode = 0;                    // Start modes over
      //color >>= 8;                 // Next color R->G->B
      if (!color) color = 0xB300A4; // Reset color
    }
    for (i = 0; i < 32; i++) pixels.setPixelColor(i, 0);
    prevTime = t;

  }
}

//MARK: animation modes

void rainbowCycle(uint8_t wait, int e, int range) {
  uint16_t r, j;

  for (j = 0; j < 256 * e; j++) {
    for (r = 0; r < pixels.numPixels(); r++) {
      pixels.setPixelColor(r, Wheel(((r * range / pixels.numPixels()) + j) & 255));
    }
    pixels.show();
    delay(wait);
  }
}

//WIP dont use?
static void chase(uint32_t c) {
  for (uint16_t i = 1; i < pixels.numPixels() - 7; i++) {
    if (i < 7) {
      pixels.setPixelColor(i  , c); // Draw new pixel
      pixels.setPixelColor(i + 7  , c); // Draw new pixel
    }
    if ( (i - 2) >= 0 ) {
      pixels.setPixelColor(i - 3, OFF); // Erase pixel a few steps back
      pixels.setPixelColor(i + 7 - 3, OFF); // Erase pixel a few steps back
    } else {
      pixels.setPixelColor(i - 3 + 7, OFF);
      //pixels.setPixelColor(i+7-3+7, OFF);
      pixels.setPixelColor(0, BLUE);
    }
    pixels.show();
    delay(50);
  }
}

// Sends a color from one side of the jewel, through the middle, to the other side.
void wave(uint32_t color, long delayTimeInMs) {
  // set pixels 1, 2 (one side of jewel)
  uint32_t firstStripeColors[7] = {OFF, color, color, OFF, OFF, OFF, OFF};
  setPixelColors(firstStripeColors, delayTimeInMs);

  // set pixels 0, 3, 6 (middle of jewel)
  uint32_t secondStripeColors[7] = {color, OFF, OFF, color, OFF, OFF, color};
  setPixelColors(secondStripeColors, delayTimeInMs);

  // set pixels 4, 5 (other side of jewel)
  uint32_t thirdStripeColors[7] = {OFF, OFF, OFF, OFF, color, color, OFF};
  setPixelColors(thirdStripeColors, delayTimeInMs);
}


// Use centerBlink() and alternateBlink() to create an explosion animation
void explosion(uint32_t color) {
  for (uint8_t iteration = 0; iteration < 4; iteration++) {
    centerBlink(color, 60);
  }

  for (uint8_t iteration = 0; iteration < 10; iteration++) {
    alternateBlink(color, 100);
  }
}

void heartBlink(uint32_t color, long delayTimeInMs) {
  // note: the pixel to turn off varies depending on the orientation of your jewel,
  //       so determine which index the top pixel is and turn that pixel off (it
  //       may take a bit of trial and error).
  uint32_t colorsForHeart[7] = {color, color, color, color, color, color, OFF};
  setPixelColors(colorsForHeart, delayTimeInMs);
  setPixelColors(ALL_OFF, delayTimeInMs);
}

// Use the heartBlink() function to simulate a heartbeat
//    - two beats each lasting a third of the given delay time
//    - pause for entire given delay time
void heartbeat(uint32_t color, long delayTimeInMs) {
  heartBlink(color, delayTimeInMs / 3);
  heartBlink(color, delayTimeInMs / 3);
  delay(delayTimeInMs);
}

//MARK: kinda unessasary functions

// Make the pixels blink alternately to animate the end of the explosion
//    - blink 3 alternating pixels on the outer ring, once
//    - blink the remaining 3 pixels on the outer ring and the middle pixel, once
void alternateBlink(uint32_t color, long delayTimeInMs) {
  uint32_t everyOther[7] = {OFF, color, OFF, color, OFF, color, OFF};
  setPixelColors(everyOther, delayTimeInMs);

  uint32_t theOtherEveryOther[7] = {color, OFF, color, OFF, color, OFF, color};
  setPixelColors(theOtherEveryOther, delayTimeInMs);
}

// Blink the center pixel to animate the initial part of an explosion
void centerBlink(uint32_t color, long delayTimeInMs) {
  uint32_t centerOnly[7] = {color, OFF, OFF, OFF, OFF, OFF, OFF};
  setPixelColors(centerOnly, delayTimeInMs);
  setPixelColors(ALL_OFF, delayTimeInMs);
}

//MARK: helper functons

// Takes a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if (WheelPos < 85) {
    return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if (WheelPos < 170) {
    WheelPos -= 85;
    return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
    WheelPos -= 170;
    return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

//Takes an array and sets all pixels based on the array
void setPixelColors(uint32_t colors[], long delayTimeInMs) {
  for (uint8_t pixelIndex = 0; pixelIndex < NUM_PIXELS / 2; pixelIndex++) {
    pixels.setPixelColor(pixelIndex, colors[pixelIndex]);
    pixels.setPixelColor(pixelIndex + 7, colors[pixelIndex]);
  }
  pixels.show();
  delay(delayTimeInMs);
}
