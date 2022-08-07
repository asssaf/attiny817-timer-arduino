#include <avr/sleep.h>

#define DEBUG true
#define Serial if(DEBUG)Serial

#define PIN_COUNT 20
#define LED_BUILTIN 5
#define EN_OUT_PIN 19
#define ON_TIME_MILLIS 5000
#define RX_PIN 8
#define TX_PIN 9

void wdt_enable() {
  _PROTECTED_WRITE(WDT.CTRLA,WDT_PERIOD_8KCLK_gc); // no window, 8 seconds
}

void wdt_disable() {
  _PROTECTED_WRITE(WDT.CTRLA,0);
}

void wdt_reset() {
  __asm__ __volatile__ ("wdr"::);
}

void rtc_init(void) {
    // wait for rtc
    while (RTC.STATUS > 0);

    RTC.CLKSEL = RTC_CLKSEL_INT1K_gc;       // 1kHz internal oscillator

    RTC.PITINTCTRL = RTC_PI_bm;             // periodic interrupt enabled

    RTC.PITCTRLA = RTC_PERIOD_CYC32768_gc   // RTC clock cycles 32768 (32 seconds at 1Khz)
    | RTC_PITEN_bm;                         // enable
}

// periodic rtc interrupt service routine
ISR(RTC_PIT_vect) {
  RTC.PITINTFLAGS = RTC_PI_bm;              // clear the pit
}

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(115200);

  // set gpio pins to input to minimize power use
  for (int pin = 0; pin < PIN_COUNT; ++pin) {
    if (DEBUG && (pin == RX_PIN) || (pin == TX_PIN)) {
      // leave the rx/tx pins alone
      continue;
    }
    pinMode(pin, INPUT_PULLUP); 
  }

  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(EN_OUT_PIN, OUTPUT);
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();

  rtc_init();
  sei();
}

// the loop function runs over and over again forever
void loop() {
  unsigned long start_time = millis();
  Serial.println("started");
  
  digitalWrite(LED_BUILTIN, LOW);         // turn the LED on
  digitalWrite(EN_OUT_PIN, HIGH);         // set enable pin high
  delay(ON_TIME_MILLIS);                  // wait

  digitalWrite(EN_OUT_PIN, LOW);          // set enable pin low
  digitalWrite(LED_BUILTIN, HIGH);        // turn the LED off

  Serial.println("going to sleep");
  Serial.flush();

  sleep_cpu();
}
