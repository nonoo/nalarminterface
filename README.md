nalarminterface
===============

This is the application package for my alarm system interface. It consist of a firmware, host application, and various (mostly shell script) tools. I made nai (short for nalarminterface) to be able to interact with my alarm box in my house, send an email if there was an alarm, turn on/off the motion detecion in the IP cameras if the alarm is armed or disarmed etc.

![nai overview](https://raw.github.com/nonoo/nalarminterface/master/contrib/systemoverview.png)

As I have a Zyxel NSA325 NAS with [FFP](http://zyxel.nas-central.org/wiki/FFP_as_zypkg) running 24/7, the alarm interface and the TC35 GSM module are connected to it via USB. See [this guide](http://dp.nonoo.hu/installing-ffp-on-zyxel-nsa325/) for installing FFP on this NAS.

About the AVR board
-------------------

The alarm system has two wire outputs which have +13.5V on them if there's an alarm or if the system is armed. I also have an [EP98 sound detection sensor](http://www.ebay.com/sch/i.html?_odkw=ep98+sound&_osacat=0&_from=R40&_trksid=p2045573.m570.l1313.TR0.TRC0.Xep98+sound+detection+sensor&_nkw=ep98+sound+detection+sensor&_sacat=0) which outputs 5V and 0V when a loud enough sound is detected. These 3 wires are connected to an [atxmega128a3u development board](http://100mhz.com/avr-xmega-avr32/development-boards/avr-xmega128a3u-usb-development-mini-board-1089.html) through optocouplers:

--- TODO here comes the schematic of an optocoupler unit ---

There are 4 optocoupler units on the AVR board:

![AVR board](https://raw.github.com/nonoo/nalarminterface/master/contrib/avrboard.jpg)

Here's the input connection schematic of the board:

![AVR board input schematics](https://raw.github.com/nonoo/nalarminterface/master/contrib/avrboardinputs.png)

I think the code is well commented and easily readable, so it can be a nice example for using USB on the xmega with libusb on the host. EEPROM reading/writing, interrupt driven UART console, RTC (for tracking elapsed time and blinking the LED), ADC (for reading the uC VCC) and - of course - GPIO pin interrupt watch are used as well.

avrboard subdir
---------------
This is the firmware for the AVR board. The board can be programmed with an [AVR ISP programmer](http://100mhz.com/avr-xmega-avr32/programmer-debugger-socket-adapter/avr-programmer-usb-avrisp-xpii-avrisp-mkii-mk2-compliant-supports-xmega-pdi-951.html). Maybe the on-board factory USB bootloader can be used for programming as well, but I think the AVR Flip software is a crap on Linux.
It watches 4 input pins and notifies the host application through USB if there was an interrupt.
It has an UART serial console (can be used with a 3.3V UART-USB converter like [these](http://www.ebay.com/sch/i.html?_odkw=ftdi+3.3v&_osacat=0&_from=R40&_trksid=p2045573.m570.l1313.TR10.TRC0.A0.Xusb+serial+3.3v&_nkw=usb+serial+3.3v&_sacat=0)).
Various debug commands can be used on the console (for the full list enter "help", or see console.c).
If an interrupt happened, the status will be saved to the EEPROM, so if there's a power outage, the alarm will be delivered as soon as power comes back and the hostapp connects to the interface again. If there was an error with saving to the EEPROM (like the EEPROM location is faulty), the EEPROM location will be incremented by one, and the hostapp gets notified about this failure.

common subdir
-------------
The sources in this directory are used both by the avrboard and the hostapp.

foscamctrl subdir
-----------------
These scripts can be used to control Foscam 9820W V2 IP cameras.
The documentaion of the used API can be found [here](http://foscam.us/forum/cgi-sdk-for-hd-camera-t6045.html).

hostapp subdir
--------------
This is the host application which connects to the avrboard interface through USB, periodically checks the connection and waits for an interrupt.
It starts a shell script when an interrupt happens or the EEPROM counter increases. These shell scripts can send emails or do other things.

The hostapp application is meant to run on my Zyxel NSA-325 NAS with FFP, it can be easily compiled both to ARM and x86.

logrotate subdir
----------------
All shell scripts log to their current subdirectory. After they are finished, these logrotate scripts can be used to limit their log files size.

mail subdir
-----------
The script here can be used as a wrapper for msmtp to easily send emails.

tc35 subdir
-----------
TODO
