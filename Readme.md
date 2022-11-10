# What is uscan (Universal Network Scanner for Linux)
Uscan is a proof of concept, from [Universal Network Scanner](https://github.com/julienblitte/UniversalScanner), for a Linux port written in C.
Universal Network Scanner tool with flexible framework to ease implementation of any vanilla discovery IP protocol such as SSDP/UPnP, mDNS, proprietary discovery protocols, etc. 

# Licence
This application and all the source code is released under licence LGPL 3.0.
You can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
You can refer to the license content at this link: https://www.gnu.org/licenses/lgpl-3.0.html

# Legal notice
All the implementation is done during my free time and are my own work and released under the above LGPL license.
All protocols are analysed by reverse engineering in territory of French Republic (France), by observation of network packet traffic and without any decompilation of code, in order to aim of system interoperability.
All trademarks are the property of their respective owners.

# Warranty
This tool is delivered as it, without any warranty. If you want specific support, or specific version, contact me.

# Protocols already supported
## Generic protocols
* SSDP (UPnP)
* WS-Discovery (ONVIF)

## Proprietary protocols
* Dahua camera discovery protocol
* Hikvision discovery protocol

## Vendors supported by design
* Uniview: WS-Discovery
* Flir: SSDP 
* Siqura: SSDP and WS-Discovery
* Mobotix: WS-Discovery

## Protocols to be ported
* DNS-SD (Zeroconf)
* GigE Vision

* Axis discovery protocol
* Bosch discovery protocol
* Google cast discovery protocol
* Hanwha (Samsung) discovery protocol
* Vivotek discovery protocol
* Sony discovery protocol
* 360Vision discovery protocol
* NiceVision discovery protocol
* Panasonic discovery protocol
* Arecont discovery protocol
* Ubiquiti discovery protocol
* VStarCam discovery protocol
* Eaton discovery protocol
* Microchip discovery protocol
* Advantech discovery protocol
* Lantronix discovery protocol
* Eden Optima Box discovery protocol
* CyberPower discovery protocol

* Uniview: WS-Discovery
* Flir: GigE Vision
* GCE Electronics: Microchip (enhanced)
* ELA Access Control: Microchip
* Vauban Access Control: Lantronix
* Eden Access Control (sub-controller): Lantronix

## Other protocols being implementated on Universal Network Scanner
* Foscam discovery protocol
* Dlink discovery protocol
* Hid discovery protocol
