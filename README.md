# Firmware over the air

## Project overview
![alt text](https://github.com/YounessMoufidy/Firmware-over-the-air-/blob/main/Project%20general%20overview.png)



## Hardware Required
- STM32 bluepill
- ESP82666 or ESP32(if Using esp32 you have to change some parameters in the arduino ide firmware)
- Oled I2C
  

## Test
### Proteuse schematic
- For testing firmware over the air ,we have used CAN communication between two stm32,the stm that  its firmware has been updated,sends the temperature value via CAN to the other stm32 that displays it in an I2C OLED
  
 ![alt text](https://github.com/YounessMoufidy/Firmware-over-the-air-/blob/main/Proteuse%20schematic.jpeg)


### Problems
Verify well the wiring between stm32 and esp8266.

## Contributing
If you find a bug or have a feature request, please don't hesitate. Your Pull requests are  welcome.
