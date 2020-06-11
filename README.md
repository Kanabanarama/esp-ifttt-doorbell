es-ifttt-doorbell
===========================================

# An ESP8266 IoT doorbell

This program sends a request to IFTTT to call your phone on GPIO input events.

To make a power supply directly from the doorbell wiring you have to rectify and
then step down the voltage as it is often 6 to 15 volts AC. You also have to
limit the bell signal to a 3.3V level, e.g. with a zener-diode.
