# sensor_data_pipeline
Getting output from multiple sensors and presenting it in the web server.

This projects aims to get readings from multiple peripherals and saves those outputs to a database to present them in a web page. MySQL is used as a database.

Devices used in this project are:
-Ultrasonic sensor
-Magnetic door sensor
-Heat sensor
-Sim 800 module
-ESP 32 WROOM-1 module(Wi-Fi and Bluetooth)
-Raspberry Pi 4

Sensors and SIM module are connected to the ESP32 on self-made PCB. Ultrasonic sensor is to determine if a person is in close proximity of the device, if so, report the distance. Heat sensor is to report ambient temprature in celsius. Magnetic door sensor is to report the duration of each door sensor while they stay apart. Idea behind this is to attach each door sensor to the places where you need to record the duration where door-like objects has been open. Sim module is to send those readings to anyone's phone. It is also designed to give location of the device via SIM. ESP 32 development board is to make all the necessary electrical interfaces with the peripherals as well as GND connections.(UART,SPI I2C) It is also used for connecting to internet and making connections with the databases and web pages.

In order to maintain smooth pipeline connections with the database and web page, Raspberry Pi 4 is used. (Linux based mini computer) All the pipeline structure are written and stored inside Raspberry Pi 4. In the link down below, the pipeline structure can be seen alongside with the project that inspired me.

 https://RandomNerdTutorials.com/esp32-esp8266-mysql-database-php/


PS: The main.cpp is not fully updated and it is not ready to use since it is constantly changes according to circumstances. Also SIM 800 module codes are not provided.
