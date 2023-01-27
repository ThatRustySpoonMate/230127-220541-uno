#include <Arduino.h>

//#define ManualBaseline 523 // Uncomment to set a manual baseline


uint16_t adc_reading;
uint16_t calibratedBaseline = 0; // ADC reading with no load connected
float voltageRef = 5000; // milliVolts
float voltageBaseline = 0;

float ADCMillivoltsPerAmp = 25; // ADC Sensitivity

float voltageReading;
float voltageDifference;
float circuitCurrent;

void calibrateADC();
uint16_t takeAverageReading(uint8_t numSamples, uint8_t channel);

void calibrateADC() {
  delay(10); // Give time for ADC to stabilize

  calibratedBaseline  = takeAverageReading(64, A0); // Average 64 samples

}

// Samples param1 number of conversions from ADC and returns the average of the sum of them all
uint16_t takeAverageReading(uint8_t numSamples, uint8_t channel) {

  uint32_t result = 0;
  
  for(uint16_t iter = 0; iter < numSamples; iter++) {
    result += analogRead(channel);
  }

  result = result / numSamples;

  return (uint16_t)result;
  
}

void setup() {
  pinMode(A0, INPUT);
  pinMode(LED_BUILTIN, OUTPUT); // Can be used for showing errors or something
  
  Serial.begin(115200);
  delay(100);
  
   
  // Handle calibrating ADC readings
  #ifndef ManualBaseline
  Serial.print("Calibrating ADC Baseline, make sure load is disconnected...");
  delay(2000);
  calibrateADC();
  Serial.println("Complete");
  #endif

  #ifdef ManualBaseline
  calibratedBaseline = ManualBaseline;
  Serial.println("ADC baseline has been manually set, no calibration necessary.");
  delay(500);
  #endif
  

  Serial.print("Baseline set to: ");
  Serial.println(calibratedBaseline);

  voltageBaseline = (float)calibratedBaseline / 1023 * voltageRef; // Convert ADC reading into a voltage 

  Serial.print("Voltage Reference: ");
  Serial.print(voltageBaseline);
  Serial.println("mV");

}

void loop() {
  
  
  // put your main code here, to run repeatedly:

  adc_reading = takeAverageReading(16, A0); // Average 16 samples into one reading
  voltageReading = (float)adc_reading / 1023 * voltageRef; // Convert ADC reading into a voltage 
  Serial.print(adc_reading);
  Serial.print(":  ");
  

  voltageDifference = voltageReading - voltageBaseline;
  
  circuitCurrent = voltageDifference / ADCMillivoltsPerAmp;
  
  Serial.print(circuitCurrent);
  Serial.println("A");

  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));

  delay(1000); // Update every 1s
}
