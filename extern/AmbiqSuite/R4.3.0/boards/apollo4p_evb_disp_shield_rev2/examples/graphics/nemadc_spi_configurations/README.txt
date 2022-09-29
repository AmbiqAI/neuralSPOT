Name:
=====
 nemadc_spi_configurations


Description:
============
 NemaDC SPI4,DSPI and QSPI clock polarity/phase configurations example.


This example demonstrates how to drive a SPI4,DSPI,QSPI panel.

4-wire SPI includes 4 signals,
* Chip select (CSX)
* SPI clock (CLK)
* SPI bidirectional data interface (DATA)
* Data and command switch (DCX).

1P1T 2-wire Dual-SPI interface includes 4 signals,
* Chip select (CSX)
* SPI clock (CLK)
* Data interface 0 (DATA0)
* Data interface 1 (DATA1).

Quad-SPI interface includes 6 signals,
* Chip select (CSX)
* SPI clock (CLK)
* Data interface 0 (DATA0)
* Data interface 1 (DATA1).
* Data interface 2 (DATA2).
* Data interface 3 (DATA3).

When define TESTMODE_EN to 1, this example runs at test pattern mode.
When define TESTMODE_EN to 0, this example runs at image display mode.


******************************************************************************


