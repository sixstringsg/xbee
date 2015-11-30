#include <SoftwareSerial.h>
// Define output pin
int ledPin = 13;
// Define global variable for LED (or Relay) state
int ledState = -1;
// Define input pin (needs to be changed per PCB specs
int switchPin = 8;
// Define global variable for the physical switch state
int switchState = LOW;
SoftwareSerial XBee(2, 3); // RX, TX

void setup() {
  // Initialize the serial
  Serial.begin(9600);
  XBee.begin(9600);
  // Define our pins
  pinMode(ledPin, OUTPUT);
  pinMode(switchPin, INPUT);
  // Turn the LED (Relay) off
  // This is for future code so if I have a power failure,
  // the system will remember its last state
  digitalWrite(ledPin, LOW);
  // and set the LED (Relay) state
  ledState = LOW;
}

void loop() {
  // check to see if there is data waiting
  int avail = XBee.available();
  // initialize our command value (really can be anything)
  // I like -1 as an impossible value (even though it is possible)
  // but whatever it doesn't matter.
  int command = -1;
  // If there is data available from the serial buffer
  if(avail > 0) {
    // check to see what the value is and subtract it by value char('0')
    command = XBee.peek() - '0';
    // if the character received is a '0' and the data in the buffer
    // is size of 1 (I did this to prevent someone trying to fuzz the
    // input making my code do weird things
    if(avail == 1 && command == 0) {
      changeLedState();
    }
    // Flush the input buffer
    for(int i = 0; i < avail; i++) {
      XBee.read();
    }
    // reply back with the state of the LED (relay)
    XBee.println(ledState);
  }
  // define a temp var for the current physical switch state
  int curSwitchState = digitalRead(switchPin);
  // if the temp state is High and the old state was low
  if(curSwitchState == HIGH && switchState == LOW) {
    // Change the old state to High
    switchState = HIGH;
    // or else if the temp state is low and the old state is high
  } else if(curSwitchState == LOW && switchState == HIGH) {
    // this means the button was pushed down and released
    switchState = LOW;
    changeLedState();
  }
  // I use this as a waiting period do make sure all data comes in
  // from the serial lines and also as a debounce for the phys switch
  delay(50);
}

// This menthon changes the switch state from the opposite of what it is
void changeLedState() {
  // XOR the LED (Relay) state with 1
  ledState = ledState ^ 1;
  // and turn off or on the LED (Relay) from the previous outcome
  digitalWrite(ledPin, ledState);
}
