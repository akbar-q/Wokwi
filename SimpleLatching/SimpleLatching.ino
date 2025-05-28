/*
 * ESP32 Power Management System
 * 
 * Features:
 * - PV panel voltage monitoring via ADC with voltage divider
 * - PIR motion sensor for occupancy detection
 * - 4 relay outputs for appliances (AC, lighting, TV, fridge)
 * - Power management based on available solar power
 * 
 * Pin Configuration:
 * - PV Voltage: GPIO34 (ADC1_CH6)
 * - PIR Motion: GPIO13
 * - Relay 1 (AC): GPIO23
 * - Relay 2 (Lighting): GPIO22
 * - Relay 3 (TV): GPIO21
 * - Relay 4 (Fridge): GPIO19
 */

// Include necessary libraries
#include <Arduino.h>

// Define pin assignments
#define PV_VOLTAGE_PIN 34    // ADC pin for PV voltage measurement
#define PIR_MOTION_PIN 13     // Digital input for PIR motion sensor
#define RELAY_AC_PIN 23       // Relay for Air Conditioner
#define RELAY_LIGHT_PIN 22    // Relay for Lighting
#define RELAY_TV_PIN 21       // Relay for TV
#define RELAY_FRIDGE_PIN 19   // Relay for Fridge

// Voltage divider parameters
#define R1 100000.0  // R1 resistance in ohms (100k)
#define R2 10000.0   // R2 resistance in ohms (10k)
#define ADC_REF_VOLTAGE 3.3  // ESP32 ADC reference voltage
#define ADC_RESOLUTION 4095.0 // 12-bit ADC resolution

// Power management thresholds (adjust based on your system)
#define LOW_POWER_THRESHOLD 12.0  // Voltage below which we consider low power (V)
#define MEDIUM_POWER_THRESHOLD 18.0 // Voltage for medium power availability (V)
#define HIGH_POWER_THRESHOLD 24.0  // Voltage for high power availability (V)

// Timing parameters
#define MOTION_TIMEOUT 300000  // 5 minutes (in ms) - lights stay on for this duration after last motion
#define PV_READ_INTERVAL 5000  // Read PV voltage every 5 seconds
#define POWER_MANAGE_INTERVAL 10000 // Run power management every 10 seconds

// Global variables
float pvVoltage = 0.0;         // Measured PV voltage
bool motionDetected = false;    // Current motion state
unsigned long lastMotionTime = 0; // Time when last motion was detected
unsigned long lastPvReadTime = 0; // Last time PV voltage was read
unsigned long lastPowerManageTime = 0; // Last time power management was run

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  
  // Configure pins
  pinMode(PV_VOLTAGE_PIN, INPUT);
  pinMode(PIR_MOTION_PIN, INPUT);
  pinMode(RELAY_AC_PIN, OUTPUT);
  pinMode(RELAY_LIGHT_PIN, OUTPUT);
  pinMode(RELAY_TV_PIN, OUTPUT);
  pinMode(RELAY_FRIDGE_PIN, OUTPUT);
  
  // Initialize relays to OFF state (assuming LOW turns relay off)
  digitalWrite(RELAY_AC_PIN, LOW);
  digitalWrite(RELAY_LIGHT_PIN, LOW);
  digitalWrite(RELAY_TV_PIN, LOW);
  digitalWrite(RELAY_FRIDGE_PIN, LOW);
  
  Serial.println("ESP32 Power Management System Initialized");
}

void loop() {
  unsigned long currentTime = millis();
  
  // Read PIR motion sensor
  checkMotion(currentTime);
  
  // Read PV voltage periodically
  if (currentTime - lastPvReadTime >= PV_READ_INTERVAL) {
    readPvVoltage();
    lastPvReadTime = currentTime;
  }
  
  // Run power management algorithm periodically
  if (currentTime - lastPowerManageTime >= POWER_MANAGE_INTERVAL) {
    managePower(currentTime);
    lastPowerManageTime = currentTime;
  }
  
  // Small delay to prevent watchdog timer issues
  delay(10);
}

// Function to read and calculate PV voltage
void readPvVoltage() {
  // Read ADC value
  int adcValue = analogRead(PV_VOLTAGE_PIN);
  
  // Calculate voltage at ADC input
  float adcVoltage = (adcValue / ADC_RESOLUTION) * ADC_REF_VOLTAGE;
  
  // Calculate actual PV voltage using voltage divider formula
  pvVoltage = adcVoltage * (R1 + R2) / R2;
  
  Serial.print("PV Voltage: ");
  Serial.print(pvVoltage);
  Serial.println(" V");
}

// Function to check motion sensor and update state
void checkMotion(unsigned long currentTime) {
  bool currentMotionState = digitalRead(PIR_MOTION_PIN);
  
  if (currentMotionState) {
    motionDetected = true;
    lastMotionTime = currentTime;
    Serial.println("Motion detected!");
  }
  
  // Check for motion timeout (turn off lights if no motion for set duration)
  if (motionDetected && (currentTime - lastMotionTime > MOTION_TIMEOUT)) {
    motionDetected = false;
    Serial.println("Motion timeout - no activity detected");
  }
}

// Power management function
void managePower(unsigned long currentTime) {
  Serial.println("Running power management...");
  
  // Always keep fridge running (highest priority)
  digitalWrite(RELAY_FRIDGE_PIN, HIGH);
  
  // Manage other appliances based on available power
  if (pvVoltage < LOW_POWER_THRESHOLD) {
    // Critical low power - turn off non-essential loads
    digitalWrite(RELAY_AC_PIN, LOW);
    digitalWrite(RELAY_TV_PIN, LOW);
    
    // Lights only on if motion detected
    digitalWrite(RELAY_LIGHT_PIN, motionDetected ? HIGH : LOW);
    
    Serial.println("Power state: CRITICAL - Minimal loads only");
  } 
  else if (pvVoltage >= LOW_POWER_THRESHOLD && pvVoltage < MEDIUM_POWER_THRESHOLD) {
    // Low power - limited operation
    digitalWrite(RELAY_AC_PIN, LOW);  // AC off
    digitalWrite(RELAY_TV_PIN, HIGH); // TV allowed
    
    // Lights based on motion
    digitalWrite(RELAY_LIGHT_PIN, motionDetected ? HIGH : LOW);
    
    Serial.println("Power state: LOW - Essential loads only");
  } 
  else if (pvVoltage >= MEDIUM_POWER_THRESHOLD && pvVoltage < HIGH_POWER_THRESHOLD) {
    // Medium power - most loads allowed
    digitalWrite(RELAY_AC_PIN, HIGH); // AC allowed
    digitalWrite(RELAY_TV_PIN, HIGH); // TV allowed
    
    // Lights based on motion
    digitalWrite(RELAY_LIGHT_PIN, motionDetected ? HIGH : LOW);
    
    Serial.println("Power state: MEDIUM - Most loads allowed");
  } 
  else {
    // High power - all loads allowed
    digitalWrite(RELAY_AC_PIN, HIGH); // AC on
    digitalWrite(RELAY_TV_PIN, HIGH); // TV on
    
    // Lights based on motion
    digitalWrite(RELAY_LIGHT_PIN, motionDetected ? HIGH : LOW);
    
    Serial.println("Power state: HIGH - All loads allowed");
  }
  
  // Print relay states
  Serial.print("Relay States - AC:");
  Serial.print(digitalRead(RELAY_AC_PIN));
  Serial.print(" Light:");
  Serial.print(digitalRead(RELAY_LIGHT_PIN));
  Serial.print(" TV:");
  Serial.print(digitalRead(RELAY_TV_PIN));
  Serial.print(" Fridge:");
  Serial.println(digitalRead(RELAY_FRIDGE_PIN));
}