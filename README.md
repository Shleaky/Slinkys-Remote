Slinkys IR Remote Controller
This repository contains the Arduino sketch for a customizable IR remote control designed for Samsung TVs, using the RP2040-Zero microcontroller. 
This remote is capable of sending pre-defined IR codes displayed on an OLED screen, allowing for a simple and effective way to control various functions of your TV without multiple physical buttons. 
The project also includes plans for future expansion to record IR codes from other remotes.

Features
Flexible Control: Cycle through commands like power on/off, volume control, and channel switching using just three buttons.
Display Interface: Commands are displayed on a small OLED screen, providing clear feedback on the selected function.
Extensible Codebase: Easy to modify and extend the list of commands and supported devices.
Development Insights: Detailed debugging outputs through serial communication for development and troubleshooting.
Hardware Components
Development Board: RP2040-Zero
Display: SSD1306 OLED Screen (3.3V, I2C at pins 4 (SDA) and 5 (SCL))
IR Components: YL-73 combined PCB (IR emitter and receiver)
User Input: 3 miniature DPST tactile push buttons
Feedback LED: Onboard Neopixel LED
Power Supply: Single cell 3.7V LiPo 300mAh battery with a single pole switch

Dependencies
This project requires the following Arduino libraries:

IRremote.hpp: For sending and receiving IR signals.
Adafruit_GFX.h and Adafruit_SSD1306.h: For OLED display operations.
Adafruit_NeoPixel.h: For controlling the onboard NeoPixel LED.
Wire.h: For I2C communications.
Install these libraries through the Arduino Library Manager or download them from their respective GitHub repositories.

Configuration
Define the appropriate pins and settings in the sketch according to your hardware setup. The default configuration is set for the RP2040-Zero and the specified components.

Usage
After setting up the hardware and uploading the code:

Power on the device.
Use the 'Next' and 'Previous' buttons to cycle through the command options displayed on the OLED.
Press the 'Send' button to transmit the IR code corresponding to the displayed command.
The device uses visual feedback (NeoPixel LED) to indicate operation status:

Green light: ready state
Red light: transmitting IR code
Contributing
Contributions to this project are welcome. Please feel free to fork the repository, make improvements, and submit pull requests. For major changes, please open an issue first to discuss what you would like to change.

License
This project is licensed under the MIT License - see the LICENSE file for details.

Acknowledgments
This project uses the IRremote library, developed independently. See its GitHub page for documentation and more details.
