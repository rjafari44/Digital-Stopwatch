#include <Timer.h>
#include <TM1637Display.h>

// global variables
const int BUTTON_START_PAUSE{8}; // pin for Start/Pause/Resume button
const int BUTTON_STOP{10};       // pin for Stop button
const int BUZZER_PIN{21};        // pin for buzzer output
const int TM_CLK{4};             // TM1637 clock pin
const int TM_DIO{6};             // TM1637 data pin

// stopwatch timer object
Timer timer;

// display object (4 digit)
TM1637Display display(TM_CLK, TM_DIO);

// variables for interrupts, sets a flag
volatile bool startPausePressed = false; 
volatile bool stopPressed = false;       

// main setup, returns nothing
void setup() {

  // set buttons with an internal pull up resistor for reading HIGH when not pressed
  pinMode(BUTTON_START_PAUSE, INPUT_PULLUP);
  pinMode(BUTTON_STOP, INPUT_PULLUP);

  // set buzzer as output
  pinMode(BUZZER_PIN, OUTPUT);

  // attach hardware interrupts to buttons (trigger on press)
  attachInterrupt(digitalPinToInterrupt(BUTTON_START_PAUSE), startPauseISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(BUTTON_STOP), stopISR, FALLING);

  // initialize display
  display.clear();          // clear any previous segments
  display.setBrightness(7); // set brightness (0–7)
}

// main loop, returns nothing
void loop() {

  // variables
  unsigned long elapsedMillis{}; // long for elapsed time in milliseconds
  int seconds{};                 // int for seconds portion
  int minutes{};                 // int for total minutes elapsed
  int displayNumber{};           // int for combined mmss value for display
  static bool wasStopped = true; // bool for Tracks if stopwatch is stopped
  static int lastMinute = -1;    // int to detect minute changes

  // if statement for start,Pause, and resume button
  if (startPausePressed) {
    startPausePressed = false;  // clear interrupt flag
    tone(BUZZER_PIN, 523, 200); // confirmation beep

    // if statement for detecting when timer was stopped
    if (wasStopped) {
      timer.stop();       // ensure clean reset
      timer.start();      // start from 00:00
      wasStopped = false; // set flag for timer to still be running
      lastMinute = -1;    // reset minute tracking
    }
    else if (timer.state() == RUNNING) { // if timer is running
      timer.pause(); // pause stopwatch
    }
    else if (timer.state() == PAUSED) { // if timer is paused
      timer.resume(); // resume stopwatch
    }
  }

  // if statement for when stop button is pressed
  if (stopPressed) {
    stopPressed = false;        // clear interrupt flag
    tone(BUZZER_PIN, 523, 200); // confirmation beep
    timer.stop();               // stop stopwatch
    wasStopped = true;          // set timer flag to stopped
  }

  // read elapsed time from timer
  elapsedMillis = timer.read();                 
  seconds = (elapsedMillis / 1000) % 60; // calculate minutes from elapsed time   
  minutes = (elapsedMillis / 1000) / 60; // calculate seconds from elapsed time        

  // display time in mm:ss format (colon enabled)
  displayNumber = minutes * 100 + seconds;      
  display.showNumberDecEx(displayNumber, 0x40, true);

  // if statement to beep once every new minute while running
  if (!wasStopped && timer.state() == RUNNING) {
    if (minutes != lastMinute && seconds == 0) {
      lastMinute = minutes;
      tone(BUZZER_PIN, 659, 300);  // Minute beep
    }
  }
}

// function for setting start/pause interrupt service routine, returns nothing
void startPauseISR() {
  startPausePressed = true; // set flag when Start button pressed
}

// function for stopping interrupt service routine, returns nothing
void stopISR() {
  stopPressed = true; // Set flag when Stop button pressed
}
