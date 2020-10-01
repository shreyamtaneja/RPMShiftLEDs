# RPMShiftLEDs
LEDs that tell you when to change gears according to RPM for racing games. The project uses a NodeMCU ESP8266 to receive and process the data sent out by the game and control an individually addressable RGB LED Strip.

Supported Games:
1. Forza Horizon 4
2. Forza Motorsport 7 (coming soon)
3. Project Cars 2 (in the works)

Required Hardware:
1. NodeMCU ESP8266
2. Individually Addressable RBG LED Strip (WS2812B recommended)

What does it actually do?
So my project not only shows the RPM on the LEDs, but it also shows how far the acceleration and the brake pedals are pressed (not that it helps, but it does look pretty cool, video coming soon). So basically I use 1 LED strip, but divide it into three regions. If you know how to code, you can modify/remove the Accelerator and brake input LEDs if you don't want them. But if you don't know how to code, I would advise you not to change the code.

Instructions:
1. Install [Arduino IDE](https://www.arduino.cc/en/Main/Software).
2. Set up Arduino IDE for ESP8266. Google how to do it or follow the instructions [here](https://randomnerdtutorials.com/how-to-install-esp8266-board-arduino-ide/#:~:text=The%20ESP8266%20community%20created%20an%20add-on%20for%20the,you%E2%80%99re%20using%20Windows%2C%20Mac%20OS%20X%20or%20Linux.)
3. Install the latest version of FastLED from the Arduino IDE Library Manager. Open Arduino IDE, then go to Tools>Manage Libraries..., then search for FastLED and install the one by Daniel Garcia.
4. Install the ESP32 library for Arduino IDE by following the instructions [here](https://github.com/espressif/arduino-esp32/blob/master/docs/arduino-ide/boards_manager.md).
5. Solder wires to the LED strip accordingly (I'm not gonna teach you that just google it).
6. Connect the Din wire from the LED strip to D5 pin (or any other Digital Pin if you know what you are doing) on the ESP8266 and the Ground wire to the ground pin.
7. Power the LED strip with an adapter.
8. Choose the right sketch for the game and open it in Arduino IDE.
9. Change the LED_PIN, NUM_LEDS, RPM_LEDS, ACCEL_LEDS, BRAKE_LEDS, LED_TYPE according to your own LED strip and how you wish to set up your LEDs.
10. Change the staticIP, gateway, subnet according to your own network. Instructions [here](https://circuits4you.com/2018/03/09/esp8266-static-ip-address-arduino-example/#:~:text=%20ESP8266%20Static%20IP%20Address%20arduino%20Example%20,to%20WiFi%20Router%20with%20Above%20Configuration%20More%20).
11. Chnage the ssid and password to that of your WiFi router.
12. Upload the sketch on your ESP8266.
13. Race!

TODO:
1. Upload video of the LED lights working.
2. Change behavior of lights to turn on only in the high-RPM ranges, just like an actual racecar or an F1 car.
3. Modify code to support Forza Motorsport 7.
4. Optimize Arduino code.
5. Start working on support for Project Cars 2.
6. Boost gauge?
