#include <Servo.h>

#define MSB_IN 2
#define LSB_IN 3
#define BTN_IN 4
#define SERVO_PIN 5
#define LED_PIN 6

Servo changerServo;

bool dataMode;
byte markerData, marker;
int servoAngle;

unsigned long buttonHoldStart, lastPress;

bool checkButton() {
  static bool lastState = 0;

  if (!digitalRead(BTN_IN)) {
    if (!lastState && (lastPress - millis() > 100)) {
      lastState = true;
      lastPress = millis();
      buttonHoldStart = millis();
      return true;
    } else
      return false;
  } else {
    lastState = false;
    return false;
  }
}

unsigned long buttonHold() {
  return !digitalRead(BTN_IN) ? millis() - buttonHoldStart : 0;
}


unsigned long blinkTill;
byte blinks;

void updateBlink() {
  bool lit = !dataMode;

  if (blinks) {
    if (blinkTill < millis()) {
      blinks --;
      blinkTill = millis() + 150;
    }
    lit = !(blinks % 2);
  } else
    blinkTill = 0;

  digitalWrite(LED_PIN, lit);
}

void setBlinks(byte numBlinks) {
  blinks = numBlinks ? 2 * numBlinks + 1 : 0;
}


void setup() {
  pinMode(MSB_IN, INPUT_PULLUP);
  pinMode(LSB_IN, INPUT_PULLUP);
  pinMode(BTN_IN, INPUT_PULLUP);

  pinMode(LED_PIN, OUTPUT);

  changerServo.attach(SERVO_PIN, 500, 2500);

  dataMode = true;
  markerData = 0;
  marker = 0;

  buttonHoldStart = 0;
  lastPress = 0;

  blinkTill = 0;
  blinks = 0;

  digitalWrite(LED_PIN, 0);
}

void loop() {
  byte _markerData = 2 * !digitalRead(MSB_IN);
  _markerData += !digitalRead(LSB_IN);

  if (markerData != _markerData) {
    markerData = _markerData;
    if (dataMode) {
      marker = markerData;
      setBlinks(marker + 1);
    }
  }

  markerData = _markerData;

  if (checkButton()) {
    dataMode = false;
    if (++marker > 3)
      marker = 0;
    setBlinks(marker + 1);
  }

  if (buttonHold() > 500) {
    dataMode = true;
    marker = markerData;
  }

  updateBlink();
  changerServo.write(map(marker, 0, 3, 0, 180));
}
