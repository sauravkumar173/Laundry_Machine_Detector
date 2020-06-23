# Laundry Machine Detector IOT Project
This repository contains all the files for the laundry machine detector project. The specifics of which can
be viewed here: https://medium.com/@sauravkumar173/creating-a-laundry-machine-iot-detector-with-esp8266-and-ruby-on-rails-bd986a7ae009


## Installation

This project uses Ruby on Rails and an Wemos D1 Mini board with ESP8266. This means that a Linux machine of WSL environment will be needed to host the server, and an Arduino IDE is necessary to flash the board.

1. Flash INO file in "laundry\_machine\_detector/Arduino Code/ESP8266\_Washing\_Machine\_Detection/ESP8266\_Washing\_Machine\_Detection.ino"
2. Open the Rails project from "LED-Logger/Ruby Rails/laundry\_machine\_logger/"
3. Run the server with "rails -s -b 0.0.0.0"
4. If the ESP8266 cannot connect to the Rails server, ensure the IP address, SSID and Password is correct to your setup
5. Ensure the MMA845X Accelerometer is correctly connected to the Wemos D1 Mini board

## Usage
Attach the Wemos board to the side of the laundry machine. Once connected to the rails server, the device will automatically detect if the machine is running or not. The state of the machine and the acceleration data will be logged in the SQL data base and will automatically appear on the webpage every 30 seconds.

## Authors
Saurav Kumar
