Name:
=====
 i2s_loopback


Description:
============
 An example to show basic I2S operation.


Purpose:
========
This example enables the I2S interfaces to loop back data from
each other. Either I2S0 or I2S1 can be selected as the master.
NOTE: 1.Use embedded pingpong machine
step 1: prepare 2 blocks of buffer
sTransfer0.ui32RxTargetAddr = addr1;
sTransfer0.ui32RxTargetAddrReverse = addr2;
am_hal_i2s_dma_configure(pI2S0Handle, &g_sI2S0Config, &sTransfer0);
step 2: call am_hal_i2s_interrupt_service to restart DMA operation, the handler help automatically switch to reverse buffer
am_dspi2s0_isr()
{
am_hal_i2s_interrupt_service(pI2S0Handle, ui32Status, &g_sI2S0Config);
}
step 3: fetch the ready data
am_hal_i2s_dma_get_buffer
2.Use new DMA-able buffer
step 1: prepare 1 block of buffer
sTransfer0.ui32RxTargetAddr = addr1;
am_hal_i2s_dma_configure(pI2S0Handle, &g_sI2S0Config, &sTransfer0);
step 2: call am_hal_i2s_dma_transfer_continue to restart DMA operation with new allocated buffer
am_dspi2s0_isr()
{
am_hal_i2s_dma_transfer_continue
}

The required pin connections are as follows.
GPIO16 I2S1CLK  to GPIO11 I2S0CLK
GPIO18 I2S1WS   to GPIO13 I2S0WS
GPIO17 I2S1DOUT to GPIO14 I2S0DIN
GPIO19 I2S1DIN  to GPIO12 I2S0DOUT

Printing takes place over the ITM at 1M Baud.



******************************************************************************


