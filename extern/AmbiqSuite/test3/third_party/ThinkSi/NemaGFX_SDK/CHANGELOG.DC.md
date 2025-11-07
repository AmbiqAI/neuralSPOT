# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).

## [1.3.7] - 2024-10

### Added
- Support for new color format L4 and L4_LE
- Support for new IPI ( Image Pixel Interface ) Interface

## [1.3.6] - 2024-06

### Fixed
- MiP interface multiple partial update with FlipY Bug - SW Workarround: enable by adding compilation flag -DMIP_WA

## [1.3.5] - 2023-10

### Fixed
- MIPICFG_2RGB444_OPT1 on Dual SPI interface
- MIPICFG_2RGB666_OPT1 on Dual SPI interface
- MIPICFG_2RGB888_OPT1 on Dual SPI interface

## [1.3.4] - 2023-07

### Added
- extra_bits argument on nemadc_layer_t struct:
  This field can only configure NEMADC_REG_LAYERn_MODE register bits (30, 29, 28, 26, 5).

### Fixed
- nemadc_MIPI_read(): Fix reading up to 4 bytes of data through DBI-B interface.

## [1.3.3] - 2023-04

### Added
- MiP_display_config_t struct for MiP panel configuration
- nemadc_set_mip_panel_parameters(): set the MiP panel configuration
- nemadc_mip_setup(): Setup MiP interface to send a full frame or multiple partials on a frame with MiP panel.

### Changed
- increase the length of the division in NemaDC

## [1.3.2] - 2023-02

### Added
- support for TSC12/TSC12A color formats
- nemadc_get_ipversion() - return IP version of NemaDC
- NEMADC_REG_LAYER0_COLOR_FORMAT
- NEMADC_REG_LAYER1_COLOR_FORMAT
- NEMADC_REG_LAYER2_COLOR_FORMAT
- NEMADC_REG_LAYER3_COLOR_FORMAT

### Fixed
- nemadc_set_layer_no_scale(): Bug fixes in NemaDC layer for any startX and startY values with 8-bit, 16-bit and 24-bit color formats for the following features:
    - flipX
    - flipXY

## [1.3.1] - 2022-12

### Changed
- nemadc_set_layer(): Bug fixes in NemaDC layer for any startX and startY values with TSC4/TSC6/TSC6A color formats for the following features:
    - no flip
    - flipX
    - flipY
    - flipXY


## [1.3] - 2022-08

### Added
- Support for NemaDC layer FlipX and FlipY feature
- Support for 565_option_1 on DBI-TypeB and (Q)(D)SPI interface color format
    - MIPICFG_1RGB565_OPT1
    - MIPICFG_2RGB565_OPT1
    - MIPICFG_4RGB565_OPT1
    - MIPICFG_8RGB565_OPT1
    - MIPICFG_16RGB565_OPT1

### Changed
- nemadc_set_layer(): Support for NemaDC layer for any startxy values for TSC4/TSC6/TSC6A color formats (previously limited to multiple of 4)


## [1.2] - 2022-05

### Added
  - NEMADC_TSC4
  - NEMADC_TSC6
  - NEMADC_TSC6A
  - NEMADC_REG_LAYER0_COLORDEC_STARTXY
  - NEMADC_REG_LAYER1_COLORDEC_STARTXY
  - NEMADC_REG_LAYER2_COLORDEC_STARTXY
  - NEMADC_REG_LAYER3_COLORDEC_STARTXY
