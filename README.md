# Temple Shir Tikva Menorah Controller

Load this code on an arduino (which has eeprom support) to control a sainsmart relay board which can be used to automatically turn menorah lights on and off in succession.

The controller is designed to advance to the next Hanukkah day upon power up assuming it was powered down because the outside light timer killed power to the menorah.

It will also advance the Hanukkah day once per 6 hours.

The button can be used to manually advance the hanukkah day if it is set incorrectly.

The arduino eeprom has a 100,000 write life cycle which means it will eventually need to be replaced.

If you are monitoring the serial output of the arduino, you will see it display what it thinks the menorah is supposed to be showing and see that it is saying the prayers before lighting the candles just in case no one else is there to say them.
