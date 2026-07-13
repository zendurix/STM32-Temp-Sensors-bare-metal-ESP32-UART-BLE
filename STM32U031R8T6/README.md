 SSD1306 128x32  bare metal (no HAL) Temperature sensor usage with displaying readings on display:  
 Board used - Nucleo STM32U031R8T6.    
   
 Connections:   
 OLED VCC -> Board 3V3  
 OLED GND -> Board GND  
 OLED SDA -> Board D14 (PB9 on processor)  
 OLED SCL -> Board D15 (PB8 on processor)  
  
 TermoResistor -> Board A5 (PC0 on processor)   
 DS18B20 DQ -> Board A0 (PA0 on processor)  

 For full wiring schematic check `schematic.png`  
   
 This programs shows readings from 3 temperature sensors on OLED Display :    
 - Termoresistor -> Analog (A) - ADC
 - Digital termometer DS18B20 -> Digital (D) - 1 wire protocol
 - internal temp (CPU) - ADC

   Drivers for SSD1306 I2C from https://github.com/zendurix/STM32-SSD1306-Display-Driver-bare-metal
 
