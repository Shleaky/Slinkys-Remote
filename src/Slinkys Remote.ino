
//        IR Remote transmitter        //
//Designed and Developed by: Cody Payne
//Libraries used bellow were developed independently, 
//see documentation through GitHub for respective authors and further usage
//Version 0.5 - Stable
//Released Date: 11/05/2024
//Version 1.0 - Under development
//V1.0 adds functionality to recieve and record IR signals from other remotes of any type and protocol
//Release Date: Expected May 2024

//        Hardware        //
//Development Board: RP2040-Zero 
//OLED Screen: SSD1306 - 3.3v - 0x3C address for I2C at pins 4(SDA) and 5(SCL)
//IR Emitter + Reciever: YL-73 combined pcb of both emitter and reciever, can be swapped for independent components
//Push Buttons: 3x miniature push button DPST tactile switches
//LED: Onboard Neopixel LED 
//Battery: Single cell 3.7v lipo 300mah
//Single Pole Switch inline with Battery power supply

//        Usage         //
// This remote functions like a typical remote with some key differences
// The 'next' and 'previous' buttons are used to cycle througha  list of commands displayed on the OLED screen
// The user then sends the command by pressing the third 'send' button
// This allows the remote to mimic the buttons of any remote without needing extra push buttons
// Edit the tvCodes and codeNames arrays to include more commands or change the code values

#define DECODE_SAMSUNG // Only concerned with Samsung devices and there transmission type, comment out if using other brands

#include "PinDefinitionsAndMore.h" // This library will assist in the setup of IR transmission settings with majority of development boards
#include "IRremote.hpp" //Infra Red send and recieve library for all IR transmission types with brand specific tools
#include <Wire.h> // Used to establish and regulate the I2C pins for communication with OLED
#include <Adafruit_GFX.h>// Used to 'draw' on the OLED screen
#include <Adafruit_SSD1306.h> // Used to define and initialise the OLED display and I2C communication
#include <Adafruit_NeoPixel.h> // Used to bit-bang the neopixel LED with precision without affecting board timers

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); // setup SSD1306 selecting Wire lib for I2C, RESET is configured for rp2040-zero

//#undef IR_SEND_PIN  // To use a different send pin it must be first undefined 
//#define IR_SEND_PIN GPIO // Re define correct pin

#define LED_PIN 16 // Onboard LED GPIO pin, cannot be changed. Used to verify transmission for user.
#define NUMPIXELS   1   // Number of neo pixel LEDs onboard
Adafruit_NeoPixel strip(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800); // Note the onboard neopixel LED recieves GRB and not RGB

IRsend irsend; // Init irsend to designated transmission pin

const int buttonNextPin = 12; // Assinged to GPIO 12 and connected push btn 1
const int buttonPrevPin = 11; // assigned to GPIO 11 and connected push btn 2
const int buttonSendPin = 10; // Assigned to GPIO 10 and connected push btn 3

// Define IR codes for the TV
unsigned long tvCodes[] = {0xE0E040BF, 0xE0E0E01F, 0xE0E0E01F, 0xE0E058A7, 0xFF02FD, 0xE0E040BF, 0xE0E0E01F, 0xE0E0E01F}; //HEX Values for each command
const char* codeNames[] = {"Power", "Volume +", "Volume -", "Channel +", "Channel -", "Ching", "Chong", "Chang"}; //Corresponding command name as a string
const int numCodes = sizeof(tvCodes) / sizeof(tvCodes[0]); // The number of respective codes obtained by dividing the total variable size in bytes by the amount of bites in each index
int currentIndex = 0; // Maintain memory of current index which is selected

void setup() {
  
  Serial.begin(9600); // open the serial port at 9600 bps:
  while(!Serial); // Wait for the serial connection to init before continuing setup
  Serial.println("Setup Start"); // Used for Debugging
  Wire.begin();  // Initiate the Wire library

  IrSender.begin();
  disableLEDFeedback();
  // Assign pinmode to input for each button and configurte on board resistor for PULLUP 
  // Allowing low signal trigger and easier debouncing
  pinMode(buttonNextPin, INPUT_PULLUP);
  pinMode(buttonPrevPin, INPUT_PULLUP);
  pinMode(buttonSendPin, INPUT_PULLUP);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  // Check to ensure display has connected
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x64
    Serial.println(F("SSD1306 allocation failed"));// Used for Debugging
    for(;;); // Don't proceed, loop forever
  } 
  
  display.clearDisplay(); //Clear screen of anything previously written text
  display.setTextSize(2); //Size 2 makes use of the entire screen size, Size 1 is too small for most users
  display.setTextColor(WHITE); //For B/W OLEDS White is selected for best transparency
  display.setCursor(0, 10); //Set cursor to top left of screen to write new text
  updateDisplay();//Display the first option (power) so the correct option is diplayrf to user on Boot

  strip.begin(); // This initializes the NeoPixel library.
  strip.show(); // Initialize all pixels to 'off'

  Serial.println("Setup Complete"); // Used for Debugging
}

void loop() {
  //Serial.println("Loop Begin"); // Used for Debugging

  // Show Green light when device is working and not transmitting
  strip.setPixelColor(0, strip.Color(0, 255, 0)); //Set green light
  strip.show(); // Show green light

  //Main loop which checks for button press on the remote and updates screen accordingly
  if (digitalRead(buttonNextPin) == LOW) {
      currentIndex = (currentIndex + 1) % numCodes; // Loop the index number if it has gone beyond length of array
    updateDisplay(); // Update display with next option
    delay(200); // debounce delay
    Serial.println("next pin pressed");// Used for Debugging
  }
  if (digitalRead(buttonPrevPin) == LOW) {
    currentIndex = (currentIndex - 1 + numCodes) % numCodes; // Loop the index number if it has gone beyond length of array
    updateDisplay();// Update display with previous option
    delay(200); // debounce delay
    Serial.println("prev pin pressed");// Used for Debugging
  }
  if (digitalRead(buttonSendPin) == LOW) {
    strip.setPixelColor(0, strip.Color(255, 0, 0)); // Set red light for transmission
    strip.show(); //Show red light for transmission
    sendIRCode();// Transmit IR code of the selected option
    delay(200); // debounce delay
    Serial.println("send pin pressed");// Used for Debugging
  }
}

//Function used to update display with current index which is selected
void updateDisplay() {
  display.clearDisplay(); // Clear the display of any previous information
  display.setCursor(0, 10); // Set the cursor to left hand side and close to middle of screen
  display.println("Command:"); // Draw Command string
  display.println(codeNames[currentIndex]); // Draw the currently selected command
  display.display(); // Send the drawn information to the display
}

//Function used to transmit the code of the currently selected index and update the display to show the transmission has been sent
//once the transmission has finished updated the display to show the currently selected index as it did before the transmission began
void sendIRCode() {
  irsend.sendNEC(tvCodes[currentIndex], 32); //Send the IR signal for the respective command for Samsung using NEC protocol, 32 bits
  display.clearDisplay(); // Clear the display of any previous information
  display.setCursor(0, 10); // Set the cursor to left hand side and close to middle of screen
  display.println("Sending:"); // Draw sending string
  display.println(codeNames[currentIndex]); // Draw the currently selected command
  display.display(); // Send the drawn information to the display
  delay(1000); // Delay the process for 1 second to wait for the transmission to complete
  updateDisplay(); // Return the display to it's original state before transmitting
}
