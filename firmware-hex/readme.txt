=== Flashen des USBFunk 1 (ATMega48) ===

Firmware: (via ISP)
avrdude -c usbasp -p atmega48 -U hfuse:w:0xde:m -U lfuse:w:0xdf:m -U flash:w:usbfunk.hex -U eeprom:w:usbfunk.eep


=== Flashen des USBFunk 2 (ATMega88) ===

Bootloader:
avrdude -c usbasp -p atmega88 -U hfuse:w:0xd6:m -U lfuse:w:0xdf:m -U flash:w:bootloader.hex -U eeprom:w:bootloader.eep

Firmware: (AVR aufs Board stecken, beim anschließen Taster halten)
avrdude -c usbasp -p atmega88 -U flash:w:usbfunk.hex -U eeprom:w:usbfunk.eep
