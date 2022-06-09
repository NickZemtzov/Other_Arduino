// Just run the SetTime thing every time rather than have a time setting function

#include <LiquidCrystal_I2C.h>
#include <Servo.h>


#include <DS1307RTC.h>
#include <TimeLib.h>
#include <SD.h> // need to include the SD library
#define SD_ChipSelectPin 4 //connect pin 4 of arduino to cs pin of sd card
#include <TMRpcm.h> //Arduino library for asynchronous playback of PCM/WAV files
#include <SPI.h> //  need to include the SPI library
LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x3F for a 16 chars and 2 line display

int servoPos = 0;

int AlarmHour = 0;
int AlarmMin = 0;

int AlarmOffset = 1; // minutes between alarm going off and the final pour

const int DisplayOnButton = 2;        // the number of the pushbutton pin
const int TurnOffAlarm = 14;
const int AddOneHour = 5;
const int AddTenMin = 6;
const int AddOneMin = 7;
const int SubOneHour = 8;
const int SubTenMin = 15;
const int SubOneMin = 10;
const int SpeakerControl = 9;
const int ServoControl = 3;

int DisplayOnButtonState = 0;         // variable for reading the pushbutton status
int TurnOffAlarmState = 0;         // variable for reading the pushbutton status
int AddHourState = 0;                 // variable for reading the pushbutton status
int AddTenMinState = 0;               // variable for reading the pushbutton status
int AddOneMinState = 0;                // variable for reading the pushbutton status
int SubHourState = 0;                 // variable for reading the pushbutton status
int SubTenMinState = 0;               // variable for reading the pushbutton status
int SubOneMinState = 0;               // variable for reading the pushbutton status

boolean lastAddHour[5] = {0,0,0,0,0};
boolean lastAddTenMin[5] = {0,0,0,0,0};
boolean lastAddOneMin[5] = {0,0,0,0,0};
boolean lastSubHour[5] = {0,0,0,0,0};
boolean lastSubTenMin[5] = {0,0,0,0,0};
boolean lastSubOneMin[5] = {0,0,0,0,0};

tmElements_t CurrentTime;

void setup() {
  // Set up Serial Monitor
  Serial.begin(9600);
  while (!Serial) ;
  delay(200);
  
  // ButtonStuff
  pinMode(DisplayOnButton, INPUT);
  pinMode(AddOneHour, INPUT);
  pinMode(AddTenMin, INPUT);
  pinMode(AddOneMin, INPUT);
  pinMode(SubOneHour, INPUT);
  pinMode(SubTenMin, INPUT);
  pinMode(SubOneMin, INPUT);
  //pinMode(SpeakerControl, OUTPUT);
  //digitalWrite(SpeakerControl, LOW);
}

void loop() {
  //digitalWrite(SpeakerControl, LOW);
  //digitalWrite(ServoControl, LOW);

  DisplayOnButtonState = digitalRead(DisplayOnButton);
  TurnOffAlarmState = digitalRead(TurnOffAlarm);
  RTC.read(CurrentTime);
  
  if (DisplayOnButtonState == LOW) {
      Serial.println("DisplayOn");
      lcd.init();
      lcd.clear();         
      lcd.backlight();      // Make sure backlight is on

      while (DisplayOnButtonState == LOW) {
        DisplayOnButtonState = digitalRead(DisplayOnButton);
        AddHourState = digitalRead(AddOneHour);
        AddTenMinState = digitalRead(AddTenMin);
        AddOneMinState = digitalRead(AddOneMin);
        SubHourState = digitalRead(SubOneHour);
        SubTenMinState = digitalRead(SubTenMin);
        SubOneMinState = digitalRead(SubOneMin);
        // Display the Current Time
        RTC.read(CurrentTime);
        lcd.setCursor(0,0);
        lcd.print("Time:");
        
        lcd.setCursor(7,0);
        if (CurrentTime.Hour < 10) {
          lcd.print("0");
          lcd.setCursor(8,0);
        }
        lcd.print(CurrentTime.Hour);
        
        lcd.setCursor(9,0);
        lcd.print(":");
        lcd.setCursor(10,0);
        if (CurrentTime.Minute < 10) {
          lcd.print("0");
          lcd.setCursor(11,0);
        }
        lcd.print(CurrentTime.Minute);
  
        lcd.setCursor(12,0);
        lcd.print(":");
        lcd.setCursor(13,0);
        if (CurrentTime.Second < 10) {
          lcd.print("0");
          lcd.setCursor(14,0);
        }
        lcd.print(CurrentTime.Second);

        // Display Alarm Time
        lcd.setCursor(0,1);
        lcd.print("Alarm:");
        
        lcd.setCursor(7,1);
        if (AlarmHour < 10) {
          lcd.print("0");
          lcd.setCursor(8,1);
        }
        lcd.print(AlarmHour);
        
        lcd.setCursor(9,1);
        lcd.print(":");
        lcd.setCursor(10,1);
        if (AlarmMin < 10) {
          lcd.print("0");
          lcd.setCursor(11,1);
        }
        lcd.print(AlarmMin);
  
        lcd.setCursor(12,1);
        lcd.print(":");
        lcd.setCursor(13,1);
        lcd.print("00");

        
        if (AddHourState == LOW && lastAddHour[0] == 1 && lastAddHour[1] == 1 && lastAddHour[2] == 1 && lastAddHour[3] == 1 && lastAddHour[4] == 1) {
          AlarmHour = (AlarmHour + 1)%24;
        } else if (AddTenMinState == LOW && lastAddTenMin[0] == 1 && lastAddTenMin[1] == 1 && lastAddTenMin[2] == 1 && lastAddTenMin[3] == 1 && lastAddTenMin[4] == 1) {
          AlarmMin = (AlarmMin + 10)%60;
        } else if (AddOneMinState == LOW && lastAddOneMin[0] == 1 && lastAddOneMin[1] == 1 && lastAddOneMin[2] == 1 && lastAddOneMin[3] == 1 && lastAddOneMin[4] == 1) {
          AlarmMin = (AlarmMin + 1)%60;
        } else if (SubHourState == LOW && lastSubHour[0] == 1 && lastSubHour[1] == 1 && lastSubHour[2] == 1 && lastSubHour[3] == 1 && lastSubHour[4] == 1) {
          AlarmHour = (AlarmHour + 23)%24;
        } else if (SubTenMinState == LOW && lastSubTenMin[0] == 1 && lastSubTenMin[1] == 1 && lastSubTenMin[2] == 1 && lastSubTenMin[3] == 1 && lastSubTenMin[4] == 1) {
          AlarmMin = (AlarmMin + 50)%60;
        } else if (SubOneMinState == LOW && lastSubOneMin[0] == 1 && lastSubOneMin[1] == 1 && lastSubOneMin[2] == 1 && lastSubOneMin[3] == 1 && lastSubOneMin[4] == 1) {
          AlarmMin = (AlarmMin + 59)%60;
        }
        
        for (int i = 0; i < 4; ++i) {
          lastAddHour[i] = lastAddHour[i+1];
          lastAddTenMin[i] = lastAddTenMin[i+1];
          lastAddOneMin[i] = lastAddOneMin[i+1];
          lastSubHour[i] = lastSubHour[i+1];
          lastSubTenMin[i] = lastSubTenMin[i+1];
          lastSubOneMin[i] = lastSubOneMin[i+1];
        }
        lastAddHour[4] = AddHourState;
        lastAddTenMin[4] = AddTenMinState;
        lastAddOneMin[4] = AddOneMinState;
        lastSubHour[4] = SubHourState;
        lastSubTenMin[4] = SubTenMinState;
        lastSubOneMin[4] = SubOneMinState;
      }
      lcd.noBacklight();
  }


  // Beginning of Alarm procedure
  //Serial.print(CurrentTime.Hour*60 + CurrentTime.Minute);
  //Serial.print("      ");
  //Serial.println(AlarmHour*60 + AlarmMin - AlarmOffset);
  if ((CurrentTime.Hour*60 + CurrentTime.Minute) == (AlarmHour*60 + AlarmMin - AlarmOffset)) {
    //TMRpcm tmrpcm; // create an object for use in this sketch
    Servo myservo;  // create servo object to control a servo
    lcd.clear();         
    lcd.backlight();      // Make sure backlight is on
    myservo.attach(3);
    myservo.write(180);              // tell servo to go to position in variable 'pos'
    //tmrpcm.speakerPin = SpeakerControl;
    //tmrpcm.setVolume(5);
    //tmrpcm.play("WakeUp.wav");
    while (TurnOffAlarmState == HIGH) {
        TurnOffAlarmState = digitalRead(TurnOffAlarm);
        RTC.read(CurrentTime);
        lcd.setCursor(0,0);
        lcd.print("Time Until Alarm");
        
        lcd.setCursor(5,1);
        if (((AlarmHour*60 + AlarmMin) - (CurrentTime.Hour*60 + CurrentTime.Minute+1)) < 10) {
          lcd.setCursor(6,1);
        }
        lcd.print((AlarmHour*60 + AlarmMin) - (CurrentTime.Hour*60 + CurrentTime.Minute+1));
        
        lcd.setCursor(7,1);
        lcd.print(":");
        lcd.setCursor(8,1);
        if (CurrentTime.Second > 50) {
          lcd.print("0");
          lcd.setCursor(9,1);
        }
        lcd.print(60 - CurrentTime.Second);
        if ((CurrentTime.Hour*60 + CurrentTime.Minute) == (AlarmHour*60 + AlarmMin)) {
          lcd.noBacklight();
            for (servoPos = 0; servoPos <= 180; servoPos += 1) { // goes from 0 degrees to 180 degrees
              // in steps of 1 degree
              myservo.write(servoPos);              // tell servo to go to position in variable 'pos'
              delay(15);                       // waits 15 ms for the servo to reach the position
            }
        }
    }
    lcd.clear();
    lcd.setCursor(2,0);
    lcd.print("Alarm is Off");
    myservo.write(180);              // tell servo to go to position in variable 'pos'
    delay(1000);
    //digitalWrite(SpeakerControl, LOW);
    //digitalWrite(ServoControl, LOW);
    while ((CurrentTime.Hour*60 + CurrentTime.Minute) < (AlarmHour*60 + AlarmMin+1)) {
      RTC.read(CurrentTime);
      delay(1000);
      //Serial.print(CurrentTime.Hour*60 + CurrentTime.Minute);
      //Serial.print("      ");
      //Serial.println(AlarmHour*60 + AlarmMin+1);
    }
    lcd.noBacklight();
  }
  






  
}
