# node-vl53l3cx
nodejs module with native bindings to the VL53L3CX API provided by STMicro as part of the stm32duino project (https://github.com/stm32duino/VL53L3CX)

The original API code is designed to work within the Arduino environment, and uses Wire.h as the interface to the i2c hardware.  To make this work under standard Linux environments, all i2c related code is replaced by standard ioctl methoods.  Some changes to the original constructor were also required to save the ioctl file descriptor rather than the Wire.h reference.

Some other bug fixes were required, especially in the VL53LX_init_and_start_range, where it was found with a full configuration reset that it overwrote the previously set VL53LX_I2C_SLAVE__DEVICE_ADDRESS with 0, causing the i2c interface on the device to hang.  Also have added a call to VL53LX_software_reset in the initialisation process, rather than relying upon toggling a GPIO connected to the hardware reset/XSDN_I pin.
