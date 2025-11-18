Brief info on various files in this directory:
==============================================
Python Dependencies:
===================
-- AP330P_tools_requirements.txt => Contains all Python package dependencies required for the Apollo330P development tools
        -- Install dependencies with: pip install -r AP330P_tools_requirements.txt
        -- Includes packages for firmware updates, MRAM recovery, configuration generation, and other development utilities
        -- Ensures consistent Python environment setup across different development machines

Scripts/:
================
-- helper scripts: These are used by other scripts
        -- am_defines.py, mram_info0_regs.py, otp_info0_regs.py, key_defines.py, key_table.py
-- create_info0.py => Should be used to generate INFO0
-- create_cust_image_blob.py => Used to generate various OTA or wired images for Ambiq SBL
        -- It requires keys.ini for keys information, and an operation specific ini file to generate the required image
-- uart_wired_update.py => Used to interact with Ambiq SBL as part of Wired update
-- uart_recovery_host.py => Used to run Wired MRAM recovery to recovery either Ambiq SBL or OEM firmware 
-- examples: This folder contains various example .ini files for building various SBL update image types 
        -- The required files should be copied to the parent directory to be used as inputs to create_cust_image_blob.py
        -- keys.ini => Determines location of key assets
        -- wired.ini => Sample ini file to download a blob to a fixed location in the device
        -- wired_ota.ini => Sample ini file to download a preconstructed OTA image to a temp place in device, and trigger OTA
        -- wired_br_ota => Generate a wired bootrom ota binary (for wired SBL update)
        -- firmware.ini => Generate OTA image corresponding to a firmware update (Can be secure or non--secure)
        -- oem_chain.ini => Generate OEM Cert chain update image
-- oem_tools_package: This folder contains various key, asset and certificate generation utilities for provisioning and runtime usage
-- socid.txt => example input file for socid_to_bin.py script
-- socid_to_bin.py => script to convert socid.txt to socid.bin
-- keybank_keys_generate.py => example script to generate keybank keys.
-- arm_utils => arm utility scripts

Jlink scripts:
==============
-- jlink-prog-info0.txt => Sample script to program INFO0 MRAM (info0.bin generated using create_info0.py)
-- jlink-prog-info0_otp.txt => Sample script to program INFO0 OTP (info0_otp.bin generated using create_info0.py)
-- jlink-ota.txt => Sample script to program OTA binary using SBL (ota.bin generated using create_cust_image_blob.py)
-- jlink-ota-bootrom.txt => Sample script to program OTA binary using bootrom (e.g. Ambiq SBL update or Patch)

Firmware Updates:
==============
The ./firmware_updates has OTA update images for the SBL and CM4 firmware packages. Each of the directories has a custom Jlink Script to load the specific OTA image. 
        -- cm4_xtalhs_only_updates - The default Factory CM4 image (Type 1)
        -- cm4_ble_updates - the BLE Radio Image (Type 2)
        -- sbl_updates - Updates for the Secure Bootloader
