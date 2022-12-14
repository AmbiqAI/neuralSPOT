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
rm -rf third_party/cordio
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

