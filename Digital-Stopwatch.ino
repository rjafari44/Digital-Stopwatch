#include <Timer.h>
#include <TM1637Display.h>

// Pin Definitions
#define LED_PIN 4
#define BUTTON_START_PAUSE 16
#define BUTTON_STOP 17
#define BUZZER_PIN 21
#define TM1637_CLK 22
#define TM1637_DIO 23

// Objects
Timer timer;
TM1637Display display(TM1637_CLK, TM1637_DIO);

// Variables
volatile bool startPausePressed = false;
volatile bool stopPressed = false;
bool ledState = false;
int lastMinute = -1;

void setup() {

  // LED setup
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // Button setup
  pinMode(BUTTON_START_PAUSE, INPUT_PULLUP);
  pinMode(BUTTON_STOP, INPUT_PULLUP);

  // Interrupts for buttons
  attachInterrupt(digitalPinToInterrupt(BUTTON_START_PAUSE), startPauseISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(BUTTON_STOP), stopISR, FALLING);

  // TM1637 display setup
  display.clear();
  display.setBrightness(7);
}

void loop() {
  // Handle Start/Pause/Resume button
  if (startPausePressed) {
    startPausePressed = false;
    tone(BUZZER_PIN, 523, 200); // Play C5 note
    if (timer.state() == STOPPED) {
      timer.start();
      ledState = true;
      digitalWrite(LED_PIN, ledState);
    } else if (timer.state() == RUNNING) {
      timer.pause();
      ledState = false;
      digitalWrite(LED_PIN, ledState);
    } else if (timer.state() == PAUSED) {
      timer.resume();
      ledState = true;
      digitalWrite(LED_PIN, ledState);
    }
  }

  // Handle Stop button
  if (stopPressed) {
    stopPressed = false;
    tone(BUZZER_PIN, 523, 200); // Play C5 note
    timer.stop();
    ledState = false;
    digitalWrite(LED_PIN, ledState);
  }

  // Update stopwatch display
  if (timer.state() != STOPPED) {
    unsigned long elapsedMillis = timer.read();
    int seconds = (elapsedMillis / 1000) % 60;
    int minutes = (elapsedMillis / 1000) / 60;

    // Update display: MM:SS
    int displayNumber = minutes * 100 + seconds;
    display.showNumberDecEx(displayNumber, 0x40, true);

    // Sound buzzer every minute
    if (minutes != lastMinute && seconds == 0) {
      lastMinute = minutes;
      tone(BUZZER_PIN, 659, 300); // E5 note
    }
  }
}

// Interrupt Service Routines
void startPauseISR() {
  startPausePressed = true;
}

void stopISR() {
  stopPressed = true;
}
