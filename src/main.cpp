#include <Arduino.h>
#include "FastLED.h"
#include "OneButton.h"

// PWM led pins
const int PARTY_LED_PIN = 5;
const int BACK_LED_PIN = 10;

const int NUM_LED_LEFT = 5;
const int NUM_LED_BACK = 4;
const int NUM_LED_RIGHT = 5;
const int TOT_NUM_LED_BACK = NUM_LED_BACK + NUM_LED_LEFT + NUM_LED_RIGHT;

CRGB backLeds[TOT_NUM_LED_BACK];
// TODO - this could change depending on the order
CRGB *left = backLeds;
CRGB *back = left + NUM_LED_LEFT;
CRGB *right = back + NUM_LED_BACK;

const int NUM_LED_PARTY = 30;

CRGB partyLeds[NUM_LED_PARTY];

const int LEFT_INDICATOR_PIN = 2;
const int RIGHT_INDICATOR_PIN = 8;
const int PARTY_PIN = 7;

// TODO - replace this with something better...
// OneButton rightIndicator(8, true);
// OneButton partyButton(7, true);

typedef void (*callback)(void);

class Button
{
private:
  unsigned long lastDebounceTime = 0;
  unsigned long debounceDelay;
  int lastButtonState = LOW;
  int buttonState = LOW;
  int pin;
  callback onClick = NULL;
public:
  Button(int pin, unsigned long debounceDelay = 50);
  void Update();
  void AttachClick(callback onClick);
};

Button::Button(int pin, unsigned long debounceDelay = 50)
{
  this->pin = pin;
  this->debounceDelay = debounceDelay;
}

void Button::AttachClick(callback onClick) {
  this->onClick = onClick;
}

void Button::Update()
{
  int reading = digitalRead(this->pin);

  if (reading != this->lastButtonState) {
    this->lastDebounceTime = millis();
  }

  if ((millis() - this->lastDebounceTime) > this->debounceDelay) {
    if(reading != this->buttonState) {
      this->buttonState = reading;

    // TODO - this is where callbacks would go
      if(this->buttonState == HIGH){
        if (this->onClick != NULL) {
          this->onClick();
        }
      }
    }

    
  }

  this->lastButtonState = reading;
}

Button leftButton(LEFT_INDICATOR_PIN);
Button rightButton(RIGHT_INDICATOR_PIN);
Button partyButton(PARTY_PIN);

int leftButtonState = 0;
int lastLeftButtonState = LOW;

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers


const int RIGHT_LED_PIN = 15;
const int LEFT_LED_PIN = 14;
const int PARTY_LED_INDICATOR_PIN = 6;

// temp
bool ledOn;
bool change;

int ticks = 0;
CHSV backColor = CHSV(0, 244, 244);

void leftIndicatorClick() {
  static int click = 0;
  Serial.print("clicked ");
  Serial.println(click++);
  ledOn = !ledOn;
  change = true;
}

void setLeds(CRGB* leds, int num, CRGB color){
  for (int i = 0; i < num; i++){
    leds[i] = color;
  }
}


void setup() {
  // Setup all our pins:
  pinMode(LEFT_INDICATOR_PIN, INPUT);
  pinMode(RIGHT_INDICATOR_PIN, INPUT);
  pinMode(PARTY_PIN, INPUT);

  pinMode(LEFT_LED_PIN, OUTPUT); 
  pinMode(RIGHT_LED_PIN, OUTPUT); 
  pinMode(PARTY_LED_INDICATOR_PIN, OUTPUT); 

  FastLED.addLeds<NEOPIXEL, BACK_LED_PIN>(backLeds, TOT_NUM_LED_BACK);
  FastLED.addLeds<NEOPIXEL, PARTY_LED_PIN>(partyLeds, NUM_LED_PARTY);

  // set the back leds to red
  setLeds(back, NUM_LED_BACK, backColor);
  FastLED.show();

  // attach callbacks
  leftButton.AttachClick(leftIndicatorClick);
}

void loop() {
  leftButton.Update();
  rightButton.Update();
  partyButton.Update();

  // testing code for left button (pin 4)
  // int leftReading = digitalRead(2);

  // if (leftReading != lastLeftButtonState) {
  //   // reset the debouncing timer
  //   lastDebounceTime = millis();
  // }

  // if ((millis() - lastDebounceTime) > debounceDelay) {
  //   if (leftReading != leftButtonState) {
  //     leftButtonState = leftReading;

  //     if (leftButtonState == HIGH) {
  //       change = true;
  //     }
  //   }
  // }
  // lastLeftButtonState = leftReading;

  // TODO this will be party mode
  if (change) {
    CRGB color;
    color.setHSV(random8(), random8(), random8());
    setLeds(backLeds, TOT_NUM_LED_BACK, color);
    FastLED.show();
    change = false;
  }

  // Always flash the back leds

}