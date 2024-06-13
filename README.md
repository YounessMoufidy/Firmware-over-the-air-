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

(By  Brahim Outirza)
### Problems
Verify well the wiring between stm32 and esp8266.

## Project members

### Saad Elmosadik:
https://www.linkedin.com/in/saad-el-mousadik-89057a228?miniProfileUrn=urn%3Ali%3Afs_miniProfile%3AACoAADkJe2wB0qLe1X46R7wvlqeAJBjChgBKF68&lipi=urn%3Ali%3Apage%3Ad_flagship3_search_srp_all%3BJbWeOKJdSQ62SKl3QL%2F%2F%2Fw%3D%3D

### Abdessamad Mobtakir:

https://www.linkedin.com/in/ACoAADbOrFUBDU3EVeOB_nG8EjyJR9WXP7ZZ7zA?miniProfileUrn=urn%3Ali%3Afs_miniProfile%3AACoAADbOrFUBDU3EVeOB_nG8EjyJR9WXP7ZZ7zA&lipi=urn%3Ali%3Apage%3Ad_flagship3_search_srp_all%3BJbWeOKJdSQ62SKl3QL%2F%2F%2Fw%3D%3D

### Brahim Outirza

https://www.linkedin.com/in/ACoAAErkNasBp4yqTBqPq-t6nevQnReD3h3DXlw?lipi=urn%3Ali%3Apage%3Ad_flagship3_search_srp_content%3BtXNSPIzIQOGtcElAg5JUtg%3D%3D

### Mohamed Bahae OUAADDI
https://www.linkedin.com/in/mohamed-bahae-ouaaddi?miniProfileUrn=urn%3Ali%3Afs_miniProfile%3AACoAAEROK7MBGhh5Z-4ZT7ym6MhhWhhLIDBBYTo&lipi=urn%3Ali%3Apage%3Ad_flagship3_search_srp_all%3BmNYQrnqtS%2BmZ4hxMXj0qvg%3D%3D




## Contributing
If you find a bug or have a feature request, please don't hesitate. Your Pull requests are  welcome.
