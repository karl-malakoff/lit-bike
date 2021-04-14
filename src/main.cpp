#include <Arduino.h>
#include "FastLED.h"

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


typedef void (*callback)(void);
typedef void (*updateFunk)(void);
// TODO - maybe - make it so update funks are called automatically

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
  pinMode(pin, INPUT);
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

class LedFlasher {
private:
  int ledPin;
  bool flashing = false;
  unsigned long flashDelay = 500;
  int currentState = LOW;
  unsigned long lastTick = 0;
public:
  LedFlasher(int pin, unsigned long flashDelay = 500);
  void turnOn();
  void turnOff();
  void change(bool on);
  int getState();
  void update();
};

LedFlasher::LedFlasher(int pin, unsigned long flashDelay = 500){
  this->ledPin = pin;
  this->flashDelay = flashDelay;
  pinMode(pin, OUTPUT);
}

void LedFlasher::turnOn() {
  this->lastTick = millis();
  this->flashing = true;
  this->currentState = HIGH;
  digitalWrite(this->ledPin, this->currentState);
}

void LedFlasher::turnOff() {
  this->lastTick = millis();
  this->flashing = false;
  this->currentState = LOW;
  digitalWrite(this->ledPin, this->currentState);
}

void LedFlasher::change(bool on) {
  if (on && !this->flashing) {
    this->turnOn();
  } else if (!on && this->flashing) {
    this->turnOff();
  }
}

int LedFlasher::getState() {
  return this->currentState;
}

void LedFlasher::update() {
  if (this->flashing){
    long int now = millis();
    if ((now - this->lastTick) > this->flashDelay) {
      this->currentState = this->currentState == HIGH ? LOW : HIGH;
      digitalWrite(this->ledPin, this->currentState);
      this->lastTick = now;
    }
  }
}

const int RIGHT_LED_PIN = 16;
const int LEFT_LED_PIN = 14;
const int PARTY_LED_INDICATOR_PIN = 6;

LedFlasher rightFlasher(RIGHT_LED_PIN);
LedFlasher leftFlasher(LEFT_LED_PIN);
LedFlasher partyFlasher(PARTY_LED_INDICATOR_PIN);

struct BikeState
{
  bool leftIndicating = false;
  bool rightIndicating = false;
  bool partying = false;

  // Changing indicates that the state machine should transition
  bool changing = false;
};

BikeState state;

// temp
bool ledOn;
bool change;

int ticks = 0;
CHSV backColor = CHSV(0, 244, 244);

void leftIndicatorClick() {
  state.changing = true;
  state.leftIndicating = !state.leftIndicating;
}

void rightIndicatorClick() {
  state.changing = true;
  state.rightIndicating = !state.rightIndicating;
}

void partyIndicatorClick() {
  state.changing = true;
  // TODO - this should change the party mode
  state.partying = !state.partying;
}

void setLeds(CRGB* leds, int num, CRGB color){
  for (int i = 0; i < num; i++){
    leds[i] = color;
  }
}


void setup() {
  // Setup all our pins:
  // pinMode(LEFT_INDICATOR_PIN, INPUT);
  // pinMode(RIGHT_INDICATOR_PIN, INPUT);
  // pinMode(PARTY_PIN, INPUT);

  // pinMode(LEFT_LED_PIN, OUTPUT); 
  // pinMode(RIGHT_LED_PIN, OUTPUT); 
  // pinMode(PARTY_LED_INDICATOR_PIN, OUTPUT); 

  // digitalWrite(LEFT_LED_PIN, HIGH);
  // digitalWrite(RIGHT_LED_PIN, HIGH);
  // digitalWrite(PARTY_LED_INDICATOR_PIN, HIGH);

  FastLED.addLeds<NEOPIXEL, BACK_LED_PIN>(backLeds, TOT_NUM_LED_BACK);
  FastLED.addLeds<NEOPIXEL, PARTY_LED_PIN>(partyLeds, NUM_LED_PARTY);

  // set the back leds to red
  setLeds(back, NUM_LED_BACK, backColor);
  FastLED.show();

  // attach callbacks
  leftButton.AttachClick(leftIndicatorClick);
  rightButton.AttachClick(rightIndicatorClick);
  partyButton.AttachClick(partyIndicatorClick);
}

void loop() {
  leftButton.Update();
  rightButton.Update();
  partyButton.Update();

  leftFlasher.update();
  rightFlasher.update();
  partyFlasher.update();

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

  if (state.changing) {
    // we just do all the changes, slightly inefficient but whatevs

    Serial.println("changing!");
    // TODO these should actually flash
    // digitalWrite(LEFT_LED_PIN, state.leftIndicating);
    // digitalWrite(RIGHT_LED_PIN, state.rightIndicating);
    // digitalWrite(PARTY_LED_INDICATOR_PIN, state.partying);

    leftFlasher.change(state.leftIndicating);
    rightFlasher.change(state.rightIndicating);
    partyFlasher.change(state.partying);

    state.changing = false;
  }

  // Always flash the back leds

}