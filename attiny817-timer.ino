#include <avr/sleep.h>

#define DEBUG true
#define Serial if(DEBUG)Serial

const int LED_BUILTIN_817 = 5;
const int EN_OUT_PIN = 19;
const unsigned long ON_TIME_MILLIS = 5000;

void wdt_enable() {
  _PROTECTED_WRITE(WDT.CTRLA,WDT_PERIOD_8KCLK_gc); // no window, 8 seconds
}

void wdt_disable() {
  _PROTECTED_WRITE(WDT.CTRLA,0);
}

void wdt_reset() {
  __asm__ __volatile__ ("wdr"::);
}

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(115200);

  // set gpio pins to input to minimize power use
  for (int pin = 0; pin < 20; ++pin) {
    if (DEBUG && (pin == 8) || (pin == 9)) {
      // leave the rx/tx pins alone
      continue;
    }
    pinMode(pin, INPUT_PULLUP); 
  }

  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN_817, OUTPUT);
  pinMode(EN_OUT_PIN, OUTPUT);
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
}

// the loop function runs over and over again forever
void loop() {
  wdt_enable();
  unsigned long start_time = millis();
  Serial.println("started");
  
  digitalWrite(LED_BUILTIN_817, LOW);     // turn the LED on 
  digitalWrite(EN_OUT_PIN, HIGH);         // set enable pin high
  delay(ON_TIME_MILLIS);                  // wait

  digitalWrite(EN_OUT_PIN, LOW);          // set enable pin low
  digitalWrite(LED_BUILTIN_817, HIGH);    // turn the LED off

  Serial.println("going to sleep");
  Serial.flush();
  wdt_reset();
  sleep_cpu();
}
