# PlantPal Complete Wiring Guide

This guide contains all pin assignments for rewiring your PlantPal system.

## Power Supply
- **External Power Supply**: 7-12V DC (recommended for stable operation with LCD and motors)
- **Arduino Mega 2560**: Connect external power to VIN and GND
- **Breadboard Power**: Connect 5V and GND rails to breadboard

---

## LEDs (Status Indicators)

| LED Color | Arduino Pin | State |
|-----------|-------------|-------|
| **Yellow** | Pin 25 | DISABLED state |
| **Green** | Pin 24 | IDLE state |
| **Red** | Pin 26 | ERROR state |
| **Blue** | Pin 12 | RUNNING state |

**Wiring:**
- Each LED: Anode (long leg) → Pin via 220Ω resistor
- Each LED: Cathode (short leg) → GND

---

## Buttons

| Button | Arduino Pin | Type | Notes |
|--------|-------------|------|-------|
| **Start/Stop** | Pin 18 | Interrupt-capable | Uses hardware interrupt (ISR) |
| **Reset** | Pin 14 | Polling | Uses software polling (no interrupt) |

**Wiring:**
- One side of button → Pin
- Other side of button → GND
- Internal pull-up resistors are enabled in code (no external resistors needed)

---

## LCD Display (16x2 Character LCD)

| LCD Pin | Arduino Pin | Function |
|---------|-------------|----------|
| VSS | GND | Ground |
| VDD | 5V | Power |
| V0 | Pin 9 (PWM) | Contrast (PWM controlled, no potentiometer needed) |
| RS | Pin 23 | Register Select |
| EN | Pin 22 | Enable |
| D4 | Pin 2 | Data bit 4 |
| D5 | Pin 53 | Data bit 5 |
| D6 | Pin 5 | Data bit 6 |
| D7 | Pin 49 | Data bit 7 |
| A (Backlight +) | 5V via 220Ω resistor | Backlight anode |
| K (Backlight -) | GND | Backlight cathode |

**Notes:**
- Contrast is controlled via PWM on pin 9 (no potentiometer needed)
- Backlight should be on (connected to 5V and GND)

---

## Sensors (Analog)

| Sensor | Arduino Pin | ADC Channel | Notes |
|--------|-------------|-------------|-------|
| **Water Level** | A0 | Channel 0 | Analog sensor |
| **Soil Moisture** | A2 | Channel 2 | Analog sensor |
| **Humidity** | A4 | Channel 4 | Analog sensor (currently reading from A4) |
| **DHT11 Temperature** | Pin 8 (Digital) | N/A | Digital sensor (currently not connected/using fake values) |

**Wiring:**
- **Water Sensor (A0)**: 
  - S pin → A0
  - + → 5V (or breadboard power)
  - - → GND
  
- **Soil Sensor (A2)**:
  - Signal → A2
  - VCC → 5V (or breadboard power)
  - GND → GND
  
- **Humidity Sensor (A4)**:
  - Signal → A4
  - VCC → 5V (or breadboard power)
  - GND → GND

- **DHT11 (Pin 8)** - Optional:
  - VCC → 5V
  - GND → GND
  - DATA → Pin 8
  - (Note: Currently using fake temperature values, sensor not required)

---

## Stepper Motor (Vent Control)

| Motor Wire | Arduino Pin | Port | Notes |
|------------|-------------|------|-------|
| Wire 1 | Pin 40 | Port G, bit 1 | Stepper motor control |
| Wire 2 | Pin 41 | Port G, bit 0 | Stepper motor control |
| Wire 3 | Pin 42 | Port L, bit 7 | Stepper motor control |
| Wire 4 | Pin 43 | Port L, bit 6 | Stepper motor control |

**Wiring:**
- Connect stepper motor driver to pins 40, 41, 42, 43
- Motor power supply: Connect to external power (not Arduino 5V)
- Motor GND: Connect to Arduino GND

**Note:** Stepper motor runs in IDLE, RUNNING, and ERROR states. Stops in DISABLED state.

---

## Fan Motor (Optional - Currently Not Implemented)

The code logs fan motor activity, but the physical fan motor is not currently wired. If you add it:
- Connect fan motor to appropriate driver/relay
- Fan activates in RUNNING state
- Fan stops in ERROR state and when leaving RUNNING state

---

## Complete Pin Summary

### Digital Pins:
- **Pin 2**: LCD D4
- **Pin 5**: LCD D6
- **Pin 8**: DHT11 DATA (optional, not currently used)
- **Pin 9**: LCD V0 (PWM for contrast)
- **Pin 12**: Blue LED
- **Pin 14**: Reset Button
- **Pin 18**: Start/Stop Button
- **Pin 22**: LCD EN
- **Pin 23**: LCD RS
- **Pin 24**: Green LED
- **Pin 25**: Yellow LED
- **Pin 26**: Red LED
- **Pin 40**: Stepper Motor Wire 1
- **Pin 41**: Stepper Motor Wire 2
- **Pin 42**: Stepper Motor Wire 3
- **Pin 43**: Stepper Motor Wire 4
- **Pin 49**: LCD D7
- **Pin 53**: LCD D5

### Analog Pins:
- **A0**: Water Level Sensor
- **A2**: Soil Moisture Sensor
- **A4**: Humidity Sensor

### Power:
- **5V**: Power for sensors, LCD, LEDs
- **GND**: Common ground for all components
- **VIN**: External power input (7-12V recommended)

---

## Wiring Checklist

- [ ] All LEDs connected (Yellow 25, Green 24, Red 26, Blue 12)
- [ ] Start/Stop button on pin 18
- [ ] Reset button on pin 14
- [ ] LCD fully wired (RS=23, EN=22, D4=2, D5=53, D6=5, D7=49, V0=9)
- [ ] Water sensor on A0
- [ ] Soil sensor on A2
- [ ] Humidity sensor on A4
- [ ] Stepper motor on pins 40, 41, 42, 43
- [ ] External power supply connected (7-12V to VIN)
- [ ] All GND connections made
- [ ] All 5V connections made

---

## Testing Order

1. **Power**: Verify external power supply is connected
2. **LEDs**: Test each LED by entering different states
3. **Buttons**: Test Start/Stop and Reset buttons
4. **LCD**: Should display "PlantPal v1.0" on startup
5. **Sensors**: Check Serial Monitor for sensor readings
6. **Stepper Motor**: Should run in IDLE/RUNNING/ERROR states
7. **State Transitions**: Test all state changes

---

## Troubleshooting

- **LCD not displaying**: Check contrast (pin 9), verify all data pins
- **LEDs not working**: Check pin assignments and resistor values
- **Buttons not working**: Verify pull-up resistors (internal, no external needed)
- **Sensors reading 0**: Check power and GND connections
- **Stepper motor not moving**: Verify power supply and pin connections

---

## Notes

- All timestamps use uptime (not real-time clock)
- Temperature currently uses fake values (22°C)
- Fan motor logging is present but physical motor not implemented
- Serial Monitor shows all state changes and motor activity with timestamps

