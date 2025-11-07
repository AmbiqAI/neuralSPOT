Name:
=====
 i2s_loopback


Description:
============
 An example to show basic I2S operation.




Purpose:
========
This example enables the I2S interfaces to loop back data from each other
 and compares data from the I2S TX and data from the another I2S RX. Either
 I2S0 or I2S1 can be selected as the master.
 For Apollo510L, there is only one I2S instance, so the loopback is between I2S0_TX
 and I2S0_RX.
 This example loops forever.


 I2S configurations:
  - 2 channels
  - 48 kHz sample rate
  - Standard I2S format
  - 32 bits word width
  - 24 bits bit-depth

 NOTE: Usage of software-implemented ping pong machine
 step 1: Prepare 2 blocks of buffer.
  - sTransfer0.ui32RxTargetAddr = addr1;
  - sTransfer0.ui32RxTargetAddrReverse = addr2;
  - am_hal_i2s_dma_configure(pI2S0Handle, &g_sI2S0Config, &sTransfer0);
 step 2: Call am_hal_i2s_interrupt_service() in the ISR to restart DMA operation,
 the ISR helps automatically switch to the reverse buffer.
 step 3: Fetch the valid data by calling am_hal_i2s_dma_get_buffer().

 The required pin connections are as follows:

 Pin connections for Apollo510_evb

 IOX is selected mode 1.
 No peripheral card is connected.

 set Mikrobus level shifter to 1.8v (mbus_if_pwr)

 GPIO_5 I2S0CLK is connected to GPIO_16 I2S1CLK(mikcrobus RST).
 GPIO_7(mikrobus MISO) I2S0WS is connected to GPIO_18 I2S1WS.
 GPIO_6(mikrobus MOSI) I2S0DOUT is connected to GPIO_19 I2S1DIN.
 GPIO_4 I2S0DIN is connected to GPIO_17 I2S1DOUT(mikrobus AN).

 Apollo330mP evb (Single I2S Self-Loopback)

     I2S0 Self-Loopback
     --------------------
     GPIO[109]  CLK                       (No external connection)
     GPIO[ 29]  DIN  <----------------->  GPIO[30]  DOUT
     GPIO[ 31]  WS                        (No external connection)
     GND

     Note: For Apollo510L and Apollo330P devices, only I2S0 is available.
     The loopback is achieved by connecting I2S0_DIN to I2S0_DOUT directly.

 Printing takes place over the SWO at 1M Baud.



******************************************************************************


