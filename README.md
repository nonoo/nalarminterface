nalarminterface
===============

This is the application package for my 4 port alarm system USB interface. It consist of a firmware, host application, and various (mostly shell script) tools.

I made nai (short for nalarminterface) to be able to interact with my alarm box in my house, send an email if there was an alarm, turn on/off the motion detecion in the IP cameras if the alarm is armed or disarmed etc.

I think the code is well commented and easily readable, so it can be a nice example for using USB on the xmega with libusb on the host. EEPROM reading/writing, interrupt driven UART console, RTC (for tracking elapsed time and blinking the LED), ADC (for reading the uC VCC) and - of course - GPIO pin interrupt watch are used as well.

avrboard subdir
---------------
This is the firmware for the atxmega128a3u board which can be bought at [100mhz.com](http://100mhz.com/avr-xmega-avr32/development-boards/avr-xmega128a3u-usb-development-mini-board-1089.html). The board can be programmed with an [AVR ISP programmer](http://100mhz.com/avr-xmega-avr32/programmer-debugger-socket-adapter/avr-programmer-usb-avrisp-xpii-avrisp-mkii-mk2-compliant-supports-xmega-pdi-951.html).
It watches 4 input pins and notifies the host application through USB if there was an interrupt.
It has an UART serial console (can be used with a 3.3V UART-USB converter like [these](http://www.ebay.com/sch/i.html?_odkw=ftdi+3.3v&_osacat=0&_from=R40&_trksid=p2045573.m570.l1313.TR10.TRC0.A0.Xusb+serial+3.3v&_nkw=usb+serial+3.3v&_sacat=0).
Various debug commands can be used on the console (for the full list enter "help", or see console.c).
If an interrupt happened, the status will be saved to the EEPROM, so if there's a power outage, the alarm will be delivered as soon as power comes back and the hostapp connects to the interface again. If there was an error with saving to the EEPROM (like the EEPROM location is faulty), the EEPROM location will be incremented by one, and the hostapp gets notified about this failure.

common subdir
-------------
The sources in this directory are used both by the avrboard and the hostapp.

foscamctrl subdir
-----------------
These scripts can be used to control Foscam 9820W V2 IP cameras.
The documentaion of the used API can be found here:
http://foscam.us/forum/cgi-sdk-for-hd-camera-t6045.html

hostapp subdir
--------------
This is the host application which connects to the avrboard interface through USB, periodically checks the connection and waits for an interrupt.
It starts a shell script when an interrupt happens or the EEPROM counter increases. These shell scripts can send emails or do other things.

logrotate subdir
----------------
All shell scripts log to their current subdirectory. After they are finished, these logrotate scripts can be used to limit their log files size.

mail subdir
-----------
The script here can be used as a wrapper for msmtp to easily send emails.
