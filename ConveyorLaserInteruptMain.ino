#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>
#include <EEPROM.h>


LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x3F for a 16 chars and 2 line display
float taktTime = 4.5; //Note this is in seconds
float temp = 0.1;

boolean last[5] = {0,0,0,0,0};
boolean lastSubtract[5] = {0,0,0,0,0};
boolean lastIncrease[5] = {0,0,0,0,0};
boolean lastDecrease[5] = {0,0,0,0,0};
boolean lastTakt[5] = {0,0,0,0,0};
boolean lastReset[5] = {0,0,0,0,0};

int offset = int(now());
int count = -1;

const int increaseButton = 2;
const int decreaseButton = 3;
const int resetButton = 4;
const int taktButton = 5;     // the number of the pushbutton pin
const int Detector = 6;
const int subtractButton = 7;
const int pauseButton = 8;
const int redLED = 9;
const int yellowLED = 10;
const int greenLED = 11;

int subtractButtonState = 0;         // variable for reading the pushbutton status
int increaseButtonState = 0;         // variable for reading the pushbutton status
int decreaseButtonState = 0;         // variable for reading the pushbutton status
int taktButtonState = 0;         // variable for reading the pushbutton status
int resetButtonState = 0;         // variable for reading the pushbutton status
int pauseButtonState = 0;         // variable for reading the pushbutton status

const int ledPin =  13;      // The LED light on the Nano. This is just for testing and troubleshooting

void setup() {
  EEPROM.get(0,temp);
  if (temp < 241 && temp >= 0) {
    EEPROM.get(0,taktTime);
  }
  
  Serial.begin(9600);
  
  // LCD stuff
  lcd.init();
  lcd.clear();         
  lcd.backlight();      // Make sure backlight is on
  
  //Laser stuff
  pinMode(Detector, INPUT);

  //Buttons
  pinMode(ledPin, OUTPUT);
  pinMode(subtractButton, INPUT);
  pinMode(increaseButton, INPUT);
  pinMode(decreaseButton, INPUT);
  pinMode(taktButton, INPUT);
  pinMode(resetButton, INPUT);
  pinMode(pauseButton, INPUT);
  pinMode(redLED, OUTPUT);
  pinMode(yellowLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
}

void loop() {
  Serial.println(EEPROM.read(0));
  if (last[0] == 0 && last[1] == 0 && last[2] == 0 && last[3] == 0 && last[4] == 0 && digitalRead(Detector) == 1) {
    count++;
  }
  for (int i = 0; i < 4; ++i) {
    last[i] = last[i+1];
  }
  last[4] = digitalRead(Detector);

  //buttons
  subtractButtonState = digitalRead(subtractButton);
  increaseButtonState = digitalRead(increaseButton);
  decreaseButtonState = digitalRead(decreaseButton);
  taktButtonState = digitalRead(taktButton);
  resetButtonState = digitalRead(resetButton);
  pauseButtonState = digitalRead(pauseButton);

  if (pauseButtonState == HIGH) {
    digitalWrite(ledPin, HIGH);
    Serial.println("Pause");
    lcd.clear();
    lcd.setCursor(5,0);
    lcd.print("Paused");
    while (pauseButtonState == HIGH) {
      delay(1000);
      pauseButtonState = digitalRead(pauseButton);
      offset = offset + 1;
    }
    lcd.clear();
  } else if (resetButtonState == HIGH && lastReset[0] == 0 && lastReset[1] == 0 && lastReset[2] == 0 && lastReset[3] == 0 && lastReset[4] == 0) {
    // turn LED on:
    digitalWrite(ledPin, HIGH);
    Serial.println("reset");
    offset = int(now());
    lcd.clear();
  } else if (taktButtonState == HIGH) {
    if (lastTakt[0] == 0 && lastTakt[1] == 0 && lastTakt[2] == 0 && lastTakt[3] == 0 && lastTakt[4] == 0) {
      lcd.clear();
    }
    digitalWrite(ledPin, HIGH);
    Serial.println("takt"); 
      if (increaseButtonState == HIGH && lastIncrease[0] == 0 && lastIncrease[1] == 0 && lastIncrease[2] == 0 && lastIncrease[3] == 0 && lastIncrease[4] == 0) {
        digitalWrite(ledPin, HIGH);
        Serial.println("increase");
        taktTime = taktTime + 0.1;
      } else if (decreaseButtonState == HIGH && lastDecrease[0] == 0 && lastDecrease[1] == 0 && lastDecrease[2] == 0 && lastDecrease[3] == 0 && lastDecrease[4] == 0) {
        digitalWrite(ledPin, HIGH);
        Serial.println("decrease");
        taktTime = taktTime - 0.1;
      }
    lcd.setCursor(0,0);   //Move cursor to character 2 on line 1
    lcd.print("Desired Takt (s)");
    lcd.setCursor(0,1);   //Move cursor to character 2 on line 1
    lcd.print(taktTime);
    EEPROM.put(0,taktTime); // saves the takt time so it isn't reset every time
  } else if (subtractButtonState == HIGH && lastSubtract[0] == 0 && lastSubtract[1] == 0 && lastSubtract[2] == 0 && lastSubtract[3] == 0 && lastSubtract[4] == 0) {
    digitalWrite(ledPin, HIGH);
    Serial.println("subtract");
    count = count - 1;
  } else {
    // turn LED off:
    digitalWrite(ledPin, LOW);
    Serial.println(0);
    if (lastTakt[0] == 1 && lastTakt[1] == 1 && lastTakt[2] == 1 && lastTakt[3] == 1 && lastTakt[4] == 1) {
      lcd.clear();
    }
    lcd.setCursor(0,0);   //Set cursor to character 2 on line 0
    lcd.print("Current:");
    lcd.setCursor(0,1);   //Move cursor to character 2 on line 1
    lcd.print("Goal:");
    lcd.setCursor(10,1);   //Move cursor to character 2 on line 1
    lcd.print(int((now()-offset)/taktTime));
    lcd.setCursor(10,0);   //Move cursor to character 2 on line 1
    lcd.print(count);
  }

  for (int i = 0; i < 4; ++i) {
    lastSubtract[i] = lastSubtract[i+1];
    lastIncrease[i] = lastIncrease[i+1];
    lastDecrease[i] = lastDecrease[i+1];
    lastTakt[i] = lastTakt[i+1];
    lastReset[i] = lastReset[i+1];
  }
  lastSubtract[4] = subtractButtonState;
  lastIncrease[4] = increaseButtonState;
  lastDecrease[4] = decreaseButtonState;
  lastTakt[4] = taktButtonState;
  lastReset[4] = resetButtonState;

  //LED calculations
  if (count >= int((now()-offset)/taktTime)) {
    digitalWrite(redLED, LOW);
    digitalWrite(yellowLED, LOW);
    digitalWrite(greenLED, HIGH);
  } else if (count < int((now()-offset)/taktTime*0.9)) {
    digitalWrite(redLED, HIGH);
    digitalWrite(yellowLED, LOW);
    digitalWrite(greenLED, LOW);
  } else {
    digitalWrite(redLED, LOW);
    digitalWrite(yellowLED, HIGH);
    digitalWrite(greenLED, LOW);
  }
    

  
}
