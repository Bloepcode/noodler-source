#include <Adafruit_NeoPixel.h>
#include <Arduino.h>

// Buzzer pin is used for signalinf that the noodles are done.
#define BUZZER_PIN 10
// Output pin is used for controlling the relay that powers the heating element.
#define OUTPUT_PIN 8
// LED pin is used for controlling the RGB ring.
#define LED_PIN 6
#define LED_COUNT 24
// Total on seconds for water to pour
#define TOTAL_ON_MILLISECONDS 180000
// #define TOTAL_ON_MILLISECONDS (uint32_t)12000
// Total off seconds for noodles to cook
#define TOTAL_OFF_MILLISECONDS 220000
// #define TOTAL_OFF_MILLISECONDS (uint32_t)12000

enum State
{
  HEATING,
  COOKING,
  FINISHED
};

// Create an instance of the Adafruit_NeoPixel class
Adafruit_NeoPixel ring(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
uint32_t seconds = 0;
State state = HEATING;

// put function declarations here:
void displayStatus();
void finished();

void setup()
{
  // Start the serial communication
  Serial.begin(9600);

  // Initialize the rgb ring
  ring.begin();
  ring.show();

  // Set the buzzer pin as an output
  pinMode(BUZZER_PIN, OUTPUT);

  // Set the output pin as an output
  pinMode(OUTPUT_PIN, OUTPUT);

  // Turn on the heating element
  digitalWrite(OUTPUT_PIN, LOW);
}

void loop()
{

  if (state == FINISHED)
  {
    finished();
    return;
  }

  displayStatus();

  if (state == HEATING)
  {
    if (seconds >= TOTAL_ON_MILLISECONDS)
    {
      state = COOKING;
      seconds = 0;
      // Turn off the heating element
      digitalWrite(OUTPUT_PIN, HIGH);
    }
  }
  else if (state == COOKING)
  {
    if (seconds >= TOTAL_OFF_MILLISECONDS)
    {
      state = FINISHED;
    }
  }

  // delay(1000);
  for (int i = 0; i < 10; i++)
  {
    delay(1000 / 24);
    seconds += 1000 / 24;
    displayStatus();
  }
}

void displayStatus()
{
  uint8_t rHeating = 245;
  uint8_t gHeating = 85;
  uint8_t bHeating = 32;
  uint32_t heatingStageColor = ring.Color(rHeating, gHeating, bHeating);
  uint8_t rCooking = 32;
  uint8_t gCooking = 125;
  uint8_t bCooking = 247;
  uint32_t cookingStageColor = ring.Color(rCooking, gCooking, bCooking);

  uint32_t totalSeconds = seconds;

  if (state == COOKING)
  {
    totalSeconds += TOTAL_ON_MILLISECONDS;
  }

  // Get how many leds should be on
  int onLeds = floor(((float)totalSeconds / ((float)TOTAL_ON_MILLISECONDS + (float)TOTAL_OFF_MILLISECONDS)) * LED_COUNT);
  // Get where the stage indicator should be
  int stageIndicator = floor(((float)TOTAL_ON_MILLISECONDS / ((float)TOTAL_ON_MILLISECONDS + (float)TOTAL_OFF_MILLISECONDS)) * LED_COUNT);
  // Get the brightness of the last pixel
  float secondsPerLed = ((float)TOTAL_ON_MILLISECONDS + (float)TOTAL_OFF_MILLISECONDS) / LED_COUNT;
  float nextLedBrightness = (totalSeconds - (onLeds * secondsPerLed)) / secondsPerLed;

  for (int i = 0; i < onLeds; i++)
  {
    if (i >= stageIndicator)
    {
      ring.setPixelColor(i, cookingStageColor);
    }
    else
    {
      ring.setPixelColor(i, heatingStageColor);
    }
  }

  if (onLeds >= stageIndicator)
  {
    ring.setPixelColor(onLeds, ring.Color(rCooking * nextLedBrightness, gCooking * nextLedBrightness, bCooking * nextLedBrightness));
  }
  else
  {
    ring.setPixelColor(onLeds, ring.Color(rHeating * nextLedBrightness, gHeating * nextLedBrightness, bHeating * nextLedBrightness));
  }

  ring.show();
}

// put function definitions here:
void finished()
{
  for (int i = 0; i < LED_COUNT; i++)
  {
    ring.setPixelColor(i, ring.Color(9, 227, 53));
    ring.show();
  }

  // Enable the buzzer
  tone(BUZZER_PIN, 1500);

  delay(250);

  for (int i = 0; i < LED_COUNT; i++)
  {
    ring.setPixelColor(i, ring.Color(0, 0, 0));
    ring.show();
  }

  // Disable the buzzer
  noTone(BUZZER_PIN);

  delay(250);
}
