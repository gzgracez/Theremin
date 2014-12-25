const int analogInPin = A0;  // Analog input pin that the photoresistor is attached to
const int analogOutPin = 9; // Analog output pin that the LED is attached to
const int speakerOutPin=10; 
const int led2=11;
const int slider = 8;

const int numReadings = 10;
int readings[numReadings];      // the readings from the analog input
int index = 0;                  // the index of the current reading
int total = 0;                  // the running total
int average = 0;                // the average

int notes[]={
  31,33,35,37,39,41,44,46,49,52,55,58,62,65,69,73,78,82,87,93,98,104,110,117,123,131,139,147,156,165,175,185,196,208,220,233,247,262,277,294,311,330,349,370,392,415,440,466,494,523,554,587,622,659,698,740,784,831,880,932,988,1047,1109,1175,1245,1319,1397,1480,1568,1661,1760,1865,1976,2093,2217,2349,2489,2637,2794,2960,3136,3322,3520,3729,3951,4186,4435,4699,4978};
int finalNote=0;

unsigned long time=0; // for re-calibration
boolean on=false; // for re-calibration

int sensorValue = 0; // value read from the photoresistor
int sensorMin = 1023; // minimum sensor value
int sensorMax = 0; // maximum sensor value

int outputValue = 0; // value output to the PWM (analog out)
int thisPitch=0; // value output for speaker

void setup() {
  Serial.begin(9600); //initialize serial communications
  pinMode(slider, INPUT);
  pinMode(13, OUTPUT);
  pinMode(analogOutPin, OUTPUT);
  pinMode(speakerOutPin, OUTPUT);
  pinMode(led2, OUTPUT);
  for (int thisReading = 0; thisReading < numReadings; thisReading++) readings[thisReading] = 0;
  digitalWrite(13, LOW);
} //end of setup()

void loop() {
  //400 - 1000 from the photoresistor; output pitch range (120 - 1500Hz)
  int sliderState = digitalRead(slider);
  if (sliderState==1){
    if (on==true){ //first time it's turned on
      time=millis();
      sensorMax=0;
      sensorMin=1023;
      on=false;
    }
    if (time+5000>=millis()){ //calibration
      sensorValue = analogRead(analogInPin);
      if (sensorValue > sensorMax) sensorMax = sensorValue;
      if (sensorValue < sensorMin) sensorMin = sensorValue;
      if (millis()%1000<=500){
        digitalWrite(analogOutPin, HIGH);
        digitalWrite(led2, LOW);
      }
      else {
        digitalWrite(analogOutPin, LOW);
        digitalWrite(led2, HIGH);
      }
    } //end of callibration
    else if (time+12000>millis()){ //3 flashes signaling start of play mode
      if ((millis()-time)%2000<=1000){
        digitalWrite(analogOutPin, HIGH);
        digitalWrite(led2, HIGH);
      }
      else {
        digitalWrite(analogOutPin, LOW);
        digitalWrite(led2, LOW);
      }
    } //end of 3 flashes signaling start of play mode
    else {//after re-calibrating
      digitalWrite(led2, HIGH);
      sensorValue = analogRead(analogInPin);
      sensorValue = constrain(sensorValue, sensorMin, sensorMax);

      total= total - readings[index]; 
      readings[index] = sensorValue; 
      total= total + readings[index]; 
      index = index + 1; 
      if (index >= numReadings) index = 0; 
      average = total / numReadings; 

      //thisPitch = map(average, sensorMin, sensorMax, 120, 1500); 
      //outputValue = map(average, sensorMin, sensorMax, 0, 255); //LED brightness
      outputValue = map(average, sensorMin, sensorMax, 0, 255); //LED brightness
      Serial.println(outputValue);
      analogWrite(analogOutPin, outputValue);
      finalNote=int(map(average, sensorMin, sensorMax, 0,50));
      thisPitch = notes[finalNote];
      tone(speakerOutPin, thisPitch, 10); //play the pitch
      //Serial.println(thisPitch);
    } //output after re-calibration
  } //end of slider on
  else{ //slider off
    digitalWrite(led2, LOW);
    sensorValue=0;
    analogWrite(analogOutPin, 0); 
    on=true;
  } //end of slider off
  delay(10); //delay in between reads for stability
} //end of loop()
