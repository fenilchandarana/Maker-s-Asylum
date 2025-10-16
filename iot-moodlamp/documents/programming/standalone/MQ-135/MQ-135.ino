/*
  MQUnifiedsensor Library - MQ135 on XIAO ESP32-C3

  Fixed for ESP32-C3:
  - Uses 3.3V voltage resolution
  - Uses 12-bit ADC (0–4095)
  - Correct A0 mapping (GPIO2 on XIAO ESP32-C3)
  - Warning messages handled properly
*/

// Include the library
#include <MQUnifiedsensor.h>

// Definitions
#define placa "XIAO_ESP32C3"
#define Voltage_Resolution 3.3       // ESP32-C3 works at 3.3V
#define pin A2                       // A0 on XIAO ESP32C3 = GPIO2 (ADC capable)
#define type "MQ-135"                // MQ135
#define ADC_Bit_Resolution 12        // ESP32C3 ADC is 12-bit (0–4095)
#define RatioMQ135CleanAir 3.6       // RS / R0 = 3.6 in clean air

// Declare Sensor
MQUnifiedsensor MQ135(placa, Voltage_Resolution, ADC_Bit_Resolution, pin, type);

void setup() {
  // Init serial port
  Serial.begin(115200);

  // Set math model to calculate PPM concentration
  MQ135.setRegressionMethod(1); //_PPM =  a*ratio^b
  MQ135.setA(102.2); 
  MQ135.setB(-2.473); // Example equation for NH4

  /*
    Other gasses:
    GAS      | a      | b
    CO       | 605.18 | -3.937  
    Alcohol  | 77.255 | -3.18 
    CO2      | 110.47 | -2.862
    Toluen   | 44.947 | -3.445
    NH4      | 102.2  | -2.473
    Aceton   | 34.668 | -3.369
  */

  // Initialize sensor
  MQ135.init(); 

  // Calibration
  Serial.print("Calibrating please wait");
  float calcR0 = 0;
  for(int i = 1; i <= 10; i++) {
    MQ135.update(); // Read voltage
    calcR0 += MQ135.calibrate(RatioMQ135CleanAir);
    Serial.print(".");
    delay(200);
  }
  MQ135.setR0(calcR0 / 10);
  Serial.println("  done!");

  // Connection check
  if(isinf(calcR0)) {
    Serial.println("Warning: Connection issue, R0 is infinite (Open circuit detected). Check wiring and supply.");
    while(1);
  }
  if(calcR0 == 0) {
    Serial.println("Warning: Connection issue, R0 is zero (short to ground). Check wiring and supply.");
    while(1);
  }

  // Enable debug
  MQ135.serialDebug(true);
}

void loop() {
  MQ135.update();       // Read the analog pin
  MQ135.readSensor();   // Calculate PPM concentration
  MQ135.serialDebug();  // Print data to Serial Monitor
  delay(500);           // Sampling interval
}
