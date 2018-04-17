/*
 * RC RESET
 * Reset O2 set-top box using IR remote control
 * Hold down power off on your remote control for more than 2 seconds.
 * Relay switches off and on power supply.
 */

#include <IRremote.h>

// constants for O2 set-top box
#define ACTION_TYPE RC5
#define ACTION_CODE 0x10C
#define ACTION_MASK 0x7FF

#define ON HIGH
#define OFF LOW
#define LED_PIN 13

int RECV_PIN = 11;  // IR detector/demodulator
int RELAY_PIN = 7;  // relay control

IRrecv irrecv(RECV_PIN);
decode_results results;

class Relay {
  private:
    byte pin;
 
  void control(byte onOff) {
    digitalWrite(pin, onOff);
  }
  public:
    Relay(byte pin) {
      this->pin = pin;
      pinMode(pin, OUTPUT);
      off();
    }
  
    void on() {
      control(0);
    }
  
    void off()
    {
      control(1);
    }  
};

void waitXSecondsAndResumeIrReceiver(byte seconds) {
  for(byte i = 0; i < (seconds * 4); i++) {
    delay(250);
    irrecv.resume();
  }  
}

void relayAction(class Relay *relay) {
  digitalWrite(LED_PIN, ON);
  relay->on();
  waitXSecondsAndResumeIrReceiver(5);
  digitalWrite(LED_PIN, OFF);  
  relay->off();
  waitXSecondsAndResumeIrReceiver(10);
}

unsigned int last = 0;
unsigned long lastms = millis();
byte toggle = 0;
Relay relay(RELAY_PIN);

void setup() {
  irrecv.enableIRIn(); // Start the receiver
  pinMode(LED_PIN, OUTPUT); // LED built-in
}

void loop() {
  if (irrecv.decode(&results)) {
    if (results.decode_type == ACTION_TYPE && (results.value & ACTION_MASK) == ACTION_CODE) {
      if (results.value != last) {
        last = results.value;      
        lastms = millis();
        toggle = 1;
      } else {
        if (millis() > lastms + 2000 && toggle) {
          relayAction(&relay);
          toggle = 0;
        }      
      }
    }
    irrecv.resume(); // Receive the next value
  }
}
