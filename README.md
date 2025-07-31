# Nano Burst Generator 
A high-performance Arduino Nano–based **square wave burst generator** with:
- Rotary encoder control
- OLED screen feedback
- 50% duty hardware output (Timer1)
- Adjustable frequency, pulse count, and gate delay

---

## Features

- Output: Clean 50% square wave on **Pin 9**
- Frequency: 100 Hz – 500 kHz
- Pulses per burst: 1 – 1000
- Gate delay: 100 µs – 1 sec
- Controlled via 3 rotary encoders (with button step toggles)
- Real-time display via SSD1306 I²C OLED

---

## BOM (Bill of Materials)

See [BOM.md](BOM.md) for full list of parts.

---

## Wiring

| Function        | Arduino Pin | Notes          |
|----------------|-------------|----------------|
| Encoder 1 A/B  | 2, 3        | Frequency       |
| Encoder 1 Btn  | 8           | Toggle freq step |
| Encoder 2 A/B  | 4, 5        | Pulse count     |
| Encoder 2 Btn  | 9           | Toggle count step |
| Encoder 3 A/B  | 6, 7        | Gate delay      |
| Encoder 3 Btn  | 10          | Toggle delay step |
| OLED SDA/SCL   | A4 / A5     | I²C interface   |
| Output Signal  | **Pin 9 (OC1A)** | Use with scope / gate driver |

You can optionally add 10k pull-up resistors to encoder buttons for stability.

---

## Uploading

1. Open `src/Nano_BurstGen.ino` in the Arduino IDE
2. Install these libraries:
   - `Encoder` by Paul Stoffregen
   - `Adafruit SSD1306`
   - `Adafruit GFX`
3. Select **Arduino Nano** and correct **ATmega328P** variant
4. Upload and watch OLED light up!

---

## Output Example

![OLED example](images/OLED-example.jpg)

- Adjust each encoder to tune output
- Buttons toggle coarse/fine control (x1 / x10 / x100...)

---

## Recommended Use

Use output with:
- High-voltage gate drivers
- Ignition/MOSFET experiments
- Pulse charging
- DIY RF testing
- Inductive kickback exploration

---

## License

MIT License — open-source hardware tool, share freely!