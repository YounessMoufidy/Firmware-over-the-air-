# Firmware over the air

## Project overview
![alt text](https://github.com/YounessMoufidy/Firmware-over-the-air-/blob/main/Project%20general%20overview.png)



## Hardware Required
- STM32 bluepill
- ESP82666 or ESP32(if Using esp32 you have to change some parameters in the arduino ide firmware)
- Oled I2C
  

## Test
### Proteuse schematic
- For testing firmware over the air ,we have used CAN communication between two stm32,the stm that  its firmware has been updated,sends the temperature value via CAN to the other stm32 that displays it in an I2C OLED(You can clone the repo "STM32_with_dht11")
  
 ![alt text](https://github.com/YounessMoufidy/Firmware-over-the-air-/blob/main/Proteuse%20schematic.jpeg)

(By  Brahim Outirza)

## WEB page
![alt text](https://github.com/YounessMoufidy/Firmware-over-the-air-/blob/main/Webpage.png)

Our webpage is based on node red,and MQTT Broker used in this project is mosquitto.

## Firebase
![alt text](https://github.com/YounessMoufidy/Firmware-over-the-air-/blob/main/firebase.png)


### Problems

Verify well the wiring between stm32 and esp8266.

## Project members

### EL MOUSADIK Saad
https://www.linkedin.com/in/saad-el-mousadik-89057a228

###  Mobtakir Abdessamad
https://www.linkedin.com/in/abdessamad-moubtakir-331b3a217/

### Outirza Brahim 

https://www.linkedin.com/in/brahim-outirza-9779572b0/

### OUAADDI Mohamed Bahae 
https://www.linkedin.com/in/mohamed-bahae-ouaaddi

### MOUFIDY Youness

https://www.linkedin.com/in/younessmoufidy/

## Contributing
If you find a bug or have a feature request, please don't hesitate. Your Pull requests are  welcome.
