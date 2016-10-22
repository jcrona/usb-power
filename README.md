# TP-Link TL-WR703N USB power switch (GPIO) driver for OpenWrt


## Synopsis

The TP-Link TL-WR703N router is great for hacking, but its USB port is not 100% reliable.
From time to time, the router loses the USB connection, and cannot enumerate the connected device(s) anymore.
In such a state, the only way to get the USB back is to physically disconnect the USB device(s), which could be an issue if you have no easy access to the router. 

This driver adds a SYSFS entry allowing to control the USB power switch GPIO. This gives the ability to either reset the USB port remotely, or to let the router do it by itself if it detects that something goes wrong.


## Build instruction

In order to build this driver as an OpenWrt package and/or add it to an OpenWrt firmware build:
- checkout an OpenWrt build tree (only tested on Barrier Breaker 14.07 so far)
- move this folder to __openwrt/package/kernel/usb-power__

Now, if you run `$ make menuconfig` from the __openwrt__ root folder, you should see an __usb-power__ entry in the __Kernel modules/Other modules__ sub-menu.
Just select it as built-in or package, and launch a build.


## Usage

Once registered, this driver adds the following sysfs entry:
- __/sys/devices/platform/usb-power.0/value__

Write 0 in it to disable the USB power switch, and write 1 to re-enable it.


## License

This driver is distributed under the GPLv2 license. See the LICENSE file for more information.


## Miscellaneous

This driver has been successfully tested on a TP-Link TL-WR703N router running OpenWrt.

Fell free to visit my [blog](http://blog.rona.fr), and/or send me a mail !

__  
Copyright (C) 2016 Jean-Christophe Rona

