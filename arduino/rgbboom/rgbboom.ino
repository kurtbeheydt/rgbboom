
// sensors
int pinSensor[4] = {A1, A2, A3, A4};
int pinled[4] = {8, 9, 10, 11};
int sensorValue[4] = {0, 0, 0, 0};
int sensorPrevValue[4] = {0, 0, 0, 0};
int sensorStatus[4] = {0, 0, 0, 0};
int sensorMin[4] = {1023, 1023, 1023, 1023};
int sensorMax[4] = {0, 0, 0, 0}; 

int sensorThreshold = 150;

int i;
int j = 4;

// rgbleds
#define pinRed 3 
#define pinGreen 5 
#define pinBlue 6                   

bool redActive = true;
bool greenActive = true;
bool blueActive = true;

#define PATTERN_FADE 0
#define PATTERN_FIX 1
#define PATTERN_BLINK 2
#define PATTERN_BLINKFAST 3
#define PATTERN_BLINKSUPERFAST 4


int patterns[] = {PATTERN_FADE, PATTERN_FIX, PATTERN_BLINK, PATTERN_BLINKFAST, PATTERN_BLINKSUPERFAST};
int patternStep = 0;
int patternDirection;
int patternPrevTime;
int currentPattern = 0;

void setup() {
  Serial.begin(9600);

  calibrateSensors();

  for (i = 0; i < j; i++) {
    pinMode(pinled[i], OUTPUT);
    digitalWrite(pinled[i], LOW);
    
    Serial.print("sensor ");
    Serial.print(i);
    Serial.print(" min: ");
    Serial.print(sensorMin[i]);
    Serial.print(" max; ");
    Serial.println(sensorMax[i]);
  }

  patternPrevTime = millis();
  
  delay(500);
}

void loop() {

  for (i = 0; i < j; i++) {
    detectSensor(i);
    digitalWrite(pinled[i], sensorStatus[i]);
  }

  nextLightStep();
  
 delay(20);
}

void nextLightStep() {
  switch (patterns[currentPattern]) {
    case PATTERN_FADE:
      ledFade();
      break;
    case PATTERN_FIX:
      ledFix();
      break;
    case PATTERN_BLINK:
      ledBlink(700);
      break;
    case PATTERN_BLINKFAST:
      ledBlink(300);
      break;
    case PATTERN_BLINKSUPERFAST:
      ledBlink(100);
      break;
  }
}

void ledBlink(int blinkDelay) {
  unsigned long currentMillis = millis();
  
  if (currentMillis > (patternPrevTime + blinkDelay)) {
    patternStep = (patternStep > 240) ? 0 : 245;
    patternPrevTime = millis();
  }
  
  analogWrite(pinRed, ((redActive) ? patternStep : 0));
  analogWrite(pinGreen, ((greenActive) ? patternStep : 0));
  analogWrite(pinBlue, ((blueActive) ? patternStep : 0));
}

void ledFix() {
  patternStep = (patternStep > 245) ? 245 : patternStep;
  
  analogWrite(pinRed, ((redActive) ? patternStep : 0));
  analogWrite(pinGreen, ((greenActive) ? patternStep : 0));
  analogWrite(pinBlue, ((blueActive) ? patternStep : 0));
}

void ledFade() {
  if (patternDirection == 0) {
    if (patternStep >= 252) {
      patternDirection = 1;
    } else {
      patternStep++;
    }
  } else {
    if (patternStep <= 4) {
      patternDirection = 0;
    } else {
      patternStep--;
    }
  }
  
  analogWrite(pinRed, ((redActive) ? patternStep : 0));
  analogWrite(pinGreen, ((greenActive) ? patternStep : 0));
  analogWrite(pinBlue, ((blueActive) ? patternStep : 0));
}

void calibrateSensors() {
  unsigned long previousMillis = millis();
  unsigned long currentMillis = millis();

  while (currentMillis < (previousMillis + 1000)) {
    for (i = 0; i < j; i++) {
      sensorValue[i] = analogRead(pinSensor[i]);

      // record the maximum sensor value
      if (sensorValue[i] > sensorMax[i]) {
        sensorMax[i] = sensorValue[i];
      }

      // record the minimum sensor value
      if (sensorValue[i] < sensorMin[i]) {
        sensorMin[i] = sensorValue[i];
      }
    }
    
    currentMillis = millis();
  }
}

boolean detectSensor(int sensor) {
  sensorValue[sensor] = analogRead(pinSensor[sensor]);

  if (sensorStatus[sensor] == 1) {
    if (sensorValue[sensor] >= (sensorPrevValue[sensor] + sensorThreshold)) {
      sensorStatus[sensor] = 0;

      Serial.print("Sensor ");
      Serial.print(sensor);
      Serial.println(" unpressed");
    }
  } else {
    if (sensorValue[sensor] < (sensorMin[sensor] - sensorThreshold)) {
      sensorStatus[sensor] = 1;
      sensorPrevValue[sensor] = sensorValue[sensor];
      
      if (sensor == 0) {
        redActive = 1 - redActive;
      } else if (sensor == 1) {
        greenActive = 1 - greenActive;
      } else if (sensor == 2) {
        blueActive = 1 - blueActive;
      } else if (sensor == 3) {
        if ((currentPattern + 1) < sizeof(patterns)) {
          currentPattern++;
        } else {
          currentPattern = 0;
        }
        Serial.println(currentPattern);
      }

      Serial.print("Sensor ");
      Serial.print(sensor);
      Serial.println(" pressed");
    }
  }
}

