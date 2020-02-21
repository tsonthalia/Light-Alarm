// Include the AccelStepper Library
#include <AccelStepper.h>
#include <DS3231.h>

// Define step constants
#define FULLSTEP 4
#define HALFSTEP 8

// Creates two instances
// Pins entered in sequence IN1-IN3-IN2-IN4 for proper step sequence
AccelStepper dimmerStepper(FULLSTEP, 8, 10, 9, 11);
AccelStepper switchStepper(FULLSTEP, 4, 3, 5, 6);

DS3231 rtc(SDA, SCL);

String days[5] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday"};
String wakeUp[5] = {"06:00", "06:00", "06:00", "06:45", "06:00"};
String sundown = "17:00";

void setup() {
  // set the maximum speed, acceleration factor,
  // initial speed and the target position for motor 1
  dimmerStepper.setMaxSpeed(1000.0);
  dimmerStepper.setAcceleration(100.0);
  dimmerStepper.setSpeed(200);

  // set the same for motor 2
  switchStepper.setMaxSpeed(1000.0);
  switchStepper.setAcceleration(100.0);
  switchStepper.setSpeed(200);

  rtc.begin();

  Serial.begin(9600);
}

void loop() {
  Serial.println(rtc.getDOWStr());
  Serial.println(rtc.getTimeStr());
  
  for (int i = 0; i<5; i++) {
    if (String(rtc.getDOWStr()).equals(days[i]) == 1 and String(rtc.getTimeStr()).indexOf(wakeUp[i]) != -1) {
      Serial.println("Time to Wake Up"); 

      int intervals = 10;
      int steps = 450;
      
      turnDimmerStepperOn();
      dimmerStepper.moveTo(-(steps)); // Put Dimmer to Min
      while(dimmerStepper.distanceToGo() != 0) {
        dimmerStepper.run();
      }
      turnDimmerStepperOff();
      

      turnSwitchStepperOn();
      switchStepper.moveTo(-330); // Flip Switch On
      while(switchStepper.distanceToGo() != 0) {
        switchStepper.run();
      }

      switchStepper.moveTo(0); // Move to Neutral
      while(switchStepper.distanceToGo() != 0) {
        switchStepper.run();
      }
      turnSwitchStepperOff();

      for (int i = 0; i<intervals; i++) {
        // Wait 3 minutes (180000 milliseconds)
        delay(1800000/intervals);
        
        turnDimmerStepperOn();

        // Move 45 steps
        dimmerStepper.moveTo(dimmerStepper.currentPosition() + steps/intervals);
        while(dimmerStepper.distanceToGo() != 0) {
          dimmerStepper.run();
        }
      
        turnDimmerStepperOff();
      }

      // Wait 30 Minutes (1800000 milliseconds)
      delay(1800000); 

      turnSwitchStepperOn();
      switchStepper.moveTo(330); // Flip Switch Off
      while(switchStepper.distanceToGo() != 0) {
        switchStepper.run();
      }

      switchStepper.moveTo(0); // Move to Neutral
      while(switchStepper.distanceToGo() != 0) {
        switchStepper.run();
      }
      turnSwitchStepperOff();
    }
  }

  if (String(rtc.getTimeStr()).indexOf(sundown) != -1) { // If Sundown, Turn Light On
    turnSwitchStepperOn();
    switchStepper.moveTo(-330); // Flip Switch On
    while(switchStepper.distanceToGo() != 0) {
      switchStepper.run();
    }

    switchStepper.moveTo(0); // Move to Neutral
    while(switchStepper.distanceToGo() != 0) {
      switchStepper.run();
    }
    turnSwitchStepperOff();
  }

  turnSwitchStepperOff();
  turnDimmerStepperOff(); 

  delay(60000); //60000ms = 1min
}

void turnDimmerStepperOn() {
  digitalWrite(8,HIGH);
  digitalWrite(9,HIGH);
  digitalWrite(10,HIGH);
  digitalWrite(11,HIGH);
}

void turnDimmerStepperOff() {
  digitalWrite(8,LOW);
  digitalWrite(9,LOW);
  digitalWrite(10,LOW);
  digitalWrite(11,LOW);
}

void turnSwitchStepperOn() {
  digitalWrite(4,HIGH);
  digitalWrite(3,HIGH);
  digitalWrite(5,HIGH);
  digitalWrite(6,HIGH);
}

void turnSwitchStepperOff() {
  digitalWrite(4,LOW);
  digitalWrite(3,LOW);
  digitalWrite(5,LOW);
  digitalWrite(6,LOW); 
}
