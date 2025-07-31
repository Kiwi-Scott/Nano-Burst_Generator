#include <Encoder.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET -1
Adafruit_SSD1306 display(128, 64, &Wire, OLED_RESET);

// === Encoders ===
Encoder encFreq(2, 3);
Encoder encPulses(4, 5);
Encoder encGate(6, 7);

#define BTN_FREQ   8
#define BTN_PULSE  9
#define BTN_GATE  10

// === Output Pin (OC1A = Pin 9) ===
#define OUT_PORT PORTB
#define OUT_BIT  _BV(1)

volatile uint16_t pulseCount = 0;
volatile uint16_t pulseLimit = 10;
volatile bool gateActive = true;
unsigned long gateDelay = 5000; // µs
unsigned long gateTimer = 0;

volatile uint32_t freq = 20000;
volatile uint16_t ocrVal = 399;

uint16_t freqStep = 1;
uint8_t pulseStep = 1;
uint16_t gateStep = 10;

void setup() {
  pinMode(9, OUTPUT); // OC1A output
  pinMode(BTN_FREQ, INPUT_PULLUP);
  pinMode(BTN_PULSE, INPUT_PULLUP);
  pinMode(BTN_GATE, INPUT_PULLUP);

  // === Timer1 for square wave
  TCCR1A = _BV(COM1A0);              // Toggle OC1A on compare
  TCCR1B = _BV(WGM12);               // CTC mode
  TIMSK1 = _BV(OCIE1A);              // Enable compare interrupt
  OCR1A = ocrVal;                    // Initial compare value

  // === OLED Setup
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.display();

  Serial.begin(9600);
}

ISR(TIMER1_COMPA_vect) {
  if (!gateActive) return;
  if (++pulseCount >= pulseLimit * 2) {
    TCCR1B &= 0b11111000; // stop timer
    gateActive = false;
    pulseCount = 0;
    OUT_PORT &= ~OUT_BIT;
    gateTimer = micros();
  }
}

void loop() {
  // === Encoder Reads
  static long lastFreq = 0, lastPulse = 0, lastGate = 0;
  long f = encFreq.read() / 4;
  long p = encPulses.read() / 4;
  long g = encGate.read() / 4;

  if (f != lastFreq) {
    freq = constrain(freq + (f - lastFreq) * freqStep, 100, 500000);
    lastFreq = f;
    ocrVal = (F_CPU / (2UL * freq)) - 1;
    OCR1A = ocrVal;
  }

  if (p != lastPulse) {
    pulseLimit = constrain(pulseLimit + (p - lastPulse) * pulseStep, 1, 1000);
    lastPulse = p;
  }

  if (g != lastGate) {
    gateDelay = constrain(gateDelay + (g - lastGate) * gateStep, 100, 1000000);
    lastGate = g;
  }

  // === Gate Restart
  if (!gateActive && (micros() - gateTimer >= gateDelay)) {
    TCNT1 = 0;
    TCCR1B = _BV(WGM12) | _BV(CS10); // prescaler = 1
    gateActive = true;
  }

  // === Buttons for Step Toggling
  static bool lastBF = 1, lastBP = 1, lastBG = 1;
  bool bf = digitalRead(BTN_FREQ);
  bool bp = digitalRead(BTN_PULSE);
  bool bg = digitalRead(BTN_GATE);

  if (bf == LOW && lastBF == HIGH)
    freqStep = (freqStep == 1) ? 10 : (freqStep == 10) ? 100 : (freqStep == 100) ? 1000 : 1;

  if (bp == LOW && lastBP == HIGH)
    pulseStep = (pulseStep == 1) ? 5 : (pulseStep == 5) ? 10 : 1;

  if (bg == LOW && lastBG == HIGH)
    gateStep = (gateStep == 10) ? 100 : (gateStep == 100) ? 1000 : 10;

  lastBF = bf;
  lastBP = bp;
  lastBG = bg;

  // === OLED Display Update (1 Hz)
  static unsigned long lastDisp = 0;
  if (millis() - lastDisp > 1000) {
    lastDisp = millis();
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Freq: "); display.print(freq); display.println(" Hz");
    display.print("Pulses: "); display.println(pulseLimit);
    display.print("Delay: "); display.print(gateDelay); display.println(" us");
    display.print("Fstep: "); display.println(freqStep);
    display.print("Pstep: "); display.println(pulseStep);
    display.print("Gstep: "); display.println(gateStep);
    display.display();
  }
}
