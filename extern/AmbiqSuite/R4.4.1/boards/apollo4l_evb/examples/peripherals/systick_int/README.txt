Name:
=====
 systick_int


Description:
============
 A simple example of using the SysTick interrupt.


This example is a simple demonstration of the use of the SysTick interrupt.

If the test board has LEDs (as defined in the BSP), the example will
blink the board's LED0 every 1/2 second.
If the test board does not have LEDs, a GPIO is toggled every 1/2 second.

Since the clock to the core is gated during sleep, whether deep sleep or
normal sleep, the SysTick interrupt cannot be used to wake the device.


******************************************************************************


