# Arduino Latching Start/Stop Circuit (Wokwi Simulation)

This project demonstrates a basic **Start/Stop latch** circuit using an ESP32 and simple digital logic, written in Arduino C++ and simulated in Wokwi. Pressing the **Start** button turns on an output and keeps it on (latched) until the **Stop** button is pressed.

## Files

- `latching_start_stop.ino` – Arduino sketch for the latch logic
- `project.wokwi.json` – Wokwi configuration file for simulation setup

## Logic Description

- **START (pin 26):** When pressed, the output latches on.
- **STOP (pin 14):** When pressed, the output unlatches and turns off.
- **OUTPUT (pin 25):** Remains HIGH after Start until Stop is pressed.

The circuit uses internal pull-up resistors and active-low buttons (pressed = LOW).

## Circuit Behavior Truth Table

| START (X0) | STOP (X1) | Previous State | Output (Y0) |
|------------|-----------|----------------|-------------|
|    1       |     1     | Off            | Off         |
|    0       |     1     | Off            | **On**      |
|    1       |     1     | On             | On          |
|    1       |     0     | On             | **Off**     |
|    0       |     0     | Any            | Off         |

**Note:** Logic assumes active-low inputs. `0` = button pressed.

## Code Summary

```cpp
#define START_PIN 26
#define STOP_PIN 14
#define OUTPUT_PIN 25

bool latched = false;

void setup() {
  pinMode(START_PIN, INPUT_PULLUP);
  pinMode(STOP_PIN, INPUT_PULLUP);
  pinMode(OUTPUT_PIN, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  bool start = digitalRead(START_PIN);
  bool stop = digitalRead(STOP_PIN);

  if (!stop) {
    latched = false;
  } else if (!start) {
    latched = true;
  }

  digitalWrite(OUTPUT_PIN, latched);
  delay(10);
}



Available at https://wokwi.com/projects/428371883782917121

Simulate this project on https://wokwi.com