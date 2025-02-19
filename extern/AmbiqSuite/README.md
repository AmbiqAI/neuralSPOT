# AmbiqSuite Stub
This directory contains pared-down versions of AmbiqSuite - basically, header files, static libraries, and a handful of source files that are not part of the distributions static libs.

## Process for adding new AmbiqSuite version
In summary, the process is (details below):
1. Create new version subdirectory, copy all AmbiqSuite contents to it
2. Create a top-level version/src and version/lib - use existing imported versions as guide
3. Copy static libs and source code as needed. Most of these files remain the same, so you can copy only those that changed for this version.
4. Remove unneeded files (examples, libs, *.c, images, etc) - basically everything that isnt a header, SVD or license file
5. Remove unneeded third_party components (we only need TinyUSB and FreeRTOS)
6. Copy modified CDC and Malloc headers
7. Patch that one pesky int^uint that causes all the warnings

### BLE/Cordio Notes
In 4.4.1, we added a number of delays to get BLE to work more consistently when in HP mode with caches enabled. These are not in the baseline AmbiqSuite code. To port a new version, this implies:
1. The cordio code needs to be compiled 
2. Look for 'ns_delay_us' (we used this instead of the AmbiqSuite delay function to make it easy to search for) and add those to the new AS cordio code.

```bash
# Get rid of some unneeded components
rm Makefile
rm -rf ambiq_ble
rm -rf bootloader
rm -rf docs
rm -rf debugger_updates
rm -rf makedefs
rm -rf third_party/tools
rm -rf third_party/FatFs
rm -rf third_party/ThinkSi
rm -rf third_party/crypto
rm -rf third_party/prime_mpi
rm -rf third_party/uecc
find boards -type d | grep example | xargs rm -rf
find boards -type f -not -name "*.h" | xargs rm
find . -type f -not -name "*.h" -a -not -name "*.txt" -a -not -name "LICENSE" | egrep -v "\./src|\./lib|module.mk|SVD" | xargs rm 

# Copy over module.mk
cp R4.3.0/module.mk R4.2.0/

# Copy source files into /src
mkdir src
diff ../R4.1.0/src ../R4.3.0/src # figure out what typically changes
cp -R ../R4.3.0/src
cp utils/am_util_delay.c src # overwrite the stuff that changed
cp utils/am_util_id.c src
cp utils/am_util_stdio.c src

# Copy libs to central location
mkdir lib
find . -name "*.a"
mkdir -p lib/apollo4b/blue_evb
mkdir -p lib/apollo4b/evb
mkdir -p lib/apollo4p/evb
mkdir -p lib/apollo4p/blue_evb
cp ./CMSIS/ARM/Lib/ARM/libarm_cortexM4lf_math.a lib
cp /CMSIS/ARM/Lib/ARM/libarm_cortexM4l_math.a lib
cp ./CMSIS/ARM/Lib/ARM/libarm_cortexM4l_math.a lib
cp ./boards/apollo4p_blue_evb/bsp/gcc/bin/libam_bsp.a lib/apollo4p/blue_evb
cp ./boards/apollo4b_blue_evb/bsp/gcc/bin/libam_bsp.a lib/apollo4b/blue_evb
cp ./boards/apollo4b_evb/bsp/gcc/bin/libam_bsp.a lib/apollo4b/evb
cp ./boards/apollo4p_evb/bsp/gcc/bin/libam_bsp.a lib/apollo4p/evb
cp ./mcu/apollo4b/hal/mcu/gcc/bin/libam_hal.a lib/apollo4b
cp ./mcu/apollo4p/hal/mcu/gcc/bin/libam_hal.a lib/apollo4p

# Copy over the files modified for neuralSPOT
cp ./extern/AmbiqSuite/R4.1.0/third_party/tinyusb/src/tusb_config.h ./extern/AmbiqSuite/R4.2.0/third_party/tinyusb/src/tusb_config.h
cp ./extern/AmbiqSuite/R4.3.0/third_party/FreeRTOSv10.1.1/Source/portable/GCC/AMapollo4/FreeRTOSConfig.h ./extern/AmbiqSuite/R4.2.0/third_party/FreeRTOSv10.1.1/Source/portable/GCC/AMapollo4/FreeRTOSConfig.h
cp ./extern/AmbiqSuite/R4.1.0/third_party/FreeRTOSv10.1.1/Source/portable/GCC/AMapollo4/rtos.h ./extern/AmbiqSuite/R4.2.0/third_party/FreeRTOSv10.1.1/Source/portable/GCC/AMapollo4/rtos.h
cp extern/AmbiqSuite/R4.3.0/src/linker_script.ld extern/AmbiqSuite/R4.2.0/src/linker_script.ld
cp extern/AmbiqSuite/R4.1.0/third_party/FreeRTOSv10.1.1/Source/include/portable.h extern/AmbiqSuite/R4.2.0/third_party/FreeRTOSv10.1.1/Source/include/portable.h

# Patch the pesky compile warning (usually in this file)
diff extern/AmbiqSuite/R4.2.0/mcu/apollo4p/hal/mcu/am_hal_card.h extern/AmbiqSuite/R4.3.0/mcu/apollo4p/hal/mcu/am_hal_card.h
```

# NEW INSTRUCTIONS
1. copy the new version into extern/AmbiqSuite
2. Run porthelper - this will attempt to heuristically copy files into src (when they didn't change between versions) and alert you to exceptions.
3. Handle the exceptions
4. Copy libs
5. Copy a bunch of other files that aren't in src or libs (headers)
6. cdc_device.c is a pain right now - until that is fixed, compare versions and if they're not too far apart, use cdc_device_ns.c


```bash
sh porthelper.sh /Users/carlosmorales/dev/ns-mirror/extern/AmbiqSuite/Apollo5B_SDK2_2024_07_02 /Users/carlosmorales/dev/ns-mirror/extern/AmbiqSuite/ambiqsuite-b36aab438d0f

 cp Aug_23_2024_c01ca97f/module.mk Oct_08_2024_e86d97b6
 cp ./Apollo5B_SDK2_2024_07_02/third_party/tinyusb/source/src/tusb_config.h ./Apollo510_SDK3_2024_09_14/third_party/tinyusb/source/src/tusb_config.h
 cp ./Apollo5B_SDK2_2024_07_02/third_party/tinyusb/source/src/tusb_option.h ./Apollo510_SDK3_2024_09_14/third_party/tinyusb/source/src/tusb_option.h
 cp ../Apollo5B_SDK2_2024_07_02/./third_party/FreeRTOSv10.5.1/Source/portable/GCC/AMapollo5/FreeRTOSConfig.h ./third_party/FreeRTOSv10.5.1/Source/portable/GCC/AMapollo5/FreeRTOSConfig.h
 cp ../Apollo5B_SDK2_2024_07_02/./third_party/FreeRTOSv10.5.1/Source/portable/GCC/AMapollo5/rtos.h  ./third_party/FreeRTOSv10.5.1/Source/portable/GCC/AMapollo5/rtos.h
 cp ./mcu/apollo5b/hal/mcu/gcc/bin/libam_hal.a lib/apollo5b
 cp ./mcu/apollo5b/hal/mcu/keil6/bin/libam_hal.lib lib/apollo5b
 cp ./boards/apollo5b_eb_revb/bsp/gcc/bin/libam_bsp.a lib/apollo5b/eb_revb
 cp ./boards/apollo5b_eb_revb/bsp/keil6/bin/libam_bsp.lib lib/apollo5b/eb_revb
 cp ../Apollo5B_SDK2_2024_07_02/src/usb/cdc_device_ns.c src/usb
 cp src/port.c ../Apollo510_SDK3_2024_09_14/src
 ```

Also, there are several 'outliers':
- FreeRTOSConfig.h and rtos.h - needs to be copied: cp ./extern/AmbiqSuite/ambiqsuite_b36aab438d0f/third_party/FreeRTOSv10.5.1/Source/portable/GCC/AMapollo5/FreeRTOSConfig.h extern/AmbiqSuite/R5.2.0/third_party/FreeRTOSv10.5.1/Source/portable/GCC/AMapollo5

- am_mcu_apollo.h : get rid of arm_mve.h include