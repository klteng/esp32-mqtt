# esp32-mqtt

Dependency Library: https://github.com/espressif/esp-iot-solution
ESP-IDF: https://github.com/espressif/esp-idf

If you haven't clone the dependency library and ESP-IDF into your local driver, please do the following:

git clone --recursive https://github.com/espressif/esp-idf
git clone --recursive https://github.com/espressif/esp-iot-solution


## Code Compilation

Clone esp32-mqtt into your local drive, please do the following:

git clone --recursive https://github.com/klteng/esp32-mqtt

Edit mqtt_demo/app_main.c to change the access point SSID and PASSWORD:
 
export IDF_PATH="your esp-idf path"
export IOT_SOLUTION_PATH="your esp-iot-solution path"

Run command 'make menuconfig' to configure Serial Flasher COM port.  
Run command 'make erase_flash' if you have previous software running on the ESP32.  If no, skip this steps.
Run command 'make flash' to compile and flash into the ESP32.

Use Terminal software to view debug output UART0 from ESP32.  


## Additional Information

This example code is connecting to Mosquitto test server at 'test.mosquitto.org'.  


