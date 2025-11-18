import optparse
import configparser
import sys
import os
import logging
sys.path.append(os.path.join(sys.path[0], "..", ".."))

from common_utils import loggerinitializer

KEY_BANK_DATA_SIZE = 64
OEM_ASSET_SIZE = 2048

CURRENT_PATH = sys.path[0]
CURRENT_PATH_SCRIPTS = os.sep + "common"
# this is the scripts local path, from where the program was called
sys.path.append(CURRENT_PATH + CURRENT_PATH_SCRIPTS)

OUTPUT_DIR_NAME = CURRENT_PATH + "/inputData/"

# Util's log file
LOG_FILENAME = "oemAssetGen.log"

class ArgumentParser:
    def __init__(self):
        self.cfg_filename = None
        self.log_filename = LOG_FILENAME
        self.parser = optparse.OptionParser(usage="usage: %prog cfg_file [log_filename]")

    def parse_arguments(self):
        (options, args) = self.parser.parse_args()
        if len(args) > 2 or len(args) < 1:
            self.parser.error("incorrect number of positional arguments")
        elif len(args) == 2:
            self.log_filename = args[1]
        self.cfg_filename = args[0]

##################################################
# memcpy - Copy data fron one buffer to other.
# The Source and destination should be of bytes object
# Parameters are -  Destination,
#                   Source,
#                   destination offset in bytes
#                   size
###################################################
def copyBytes(dest, src, offset, size):
    for i in range(size):
        dest[i + offset] = src[i]

# The GetDataFromBinFile gets the data from a binary file
def GetDataFromBinFile(fileName):
    binStr = str()
    try:
        # Open a binary file and write the data to it
        FileObj = open(fileName, "rb")
        binStr = FileObj.read()
        binSize = len(binStr)
        FileObj.close()

    except IOError as Error7:
        (errno, strerror) = Error7.args
        print("Error in openning file - %s" %fileName)
        sys.exit(1)
    return binSize, binStr

class AssetGenConfig:
    CFG_SECTION_NAME = "OEM_ASSET_GEN_CFG"

    def __init__(self):
        pass

    @property
    def section_name(self):
        return self.CFG_SECTION_NAME

class AssetGenConfigParser:
    def __init__(self, config_filename):
        self.config_filename = config_filename
        self.config = configparser.ConfigParser()
        self.logger = logging.getLogger()
        self._config_holder = AssetGenConfig()

    def get_config(self):
        return self._config_holder

    def parse_config(self):
        local_dict = {}
        self.logger.info("Parsing config file: " + self.config_filename)
        self.config.read(self.config_filename)

        if not self.config.has_section(self._config_holder.section_name):
            self.logger.info("section [" + self._config_holder.section_name + "] wasn't found in cfg file\n")
            return None

        # Check for OTP_SBL_WPROT0-7: Flash write-protection bits
        if self.config.has_option(self._config_holder.section_name, 'OTP_SBL_WPROT0'):
            otp_sbl_wprot0_str = self.config.get(self._config_holder.section_name, 'OTP_SBL_WPROT0')
            local_dict['OTP_SBL_WPROT0'] = int(otp_sbl_wprot0_str, 16)
        else:
            self.logger.info("OTP_SBL_WPROT0: Flash Write-Protection Word 0 Not Provided \n")
            return None, None

        if self.config.has_option(self._config_holder.section_name, 'OTP_SBL_WPROT1'):
            otp_sbl_wprot1_str = self.config.get(self._config_holder.section_name, 'OTP_SBL_WPROT1')
            local_dict['OTP_SBL_WPROT1'] = int(otp_sbl_wprot1_str, 16)
        else:
            self.logger.info("OTP_SBL_WPROT1: Flash Write-Protection Word 1 Not Provided \n")
            return None, None

        if self.config.has_option(self._config_holder.section_name, 'OTP_SBL_WPROT2'):
            otp_sbl_wprot2_str = self.config.get(self._config_holder.section_name, 'OTP_SBL_WPROT2')
            local_dict['OTP_SBL_WPROT2'] = int(otp_sbl_wprot2_str, 16)
        else:
            self.logger.info("OTP_SBL_WPROT2: Flash Write-Protection Word 2 Not Provided \n")
            return None, None

        if self.config.has_option(self._config_holder.section_name, 'OTP_SBL_WPROT3'):
            otp_sbl_wprot3_str = self.config.get(self._config_holder.section_name, 'OTP_SBL_WPROT3')
            local_dict['OTP_SBL_WPROT3'] = int(otp_sbl_wprot3_str, 16)
        else:
            self.logger.info("OTP_SBL_WPROT3: Flash Write-Protection Word 3 Not Provided \n")
            return None, None

        if self.config.has_option(self._config_holder.section_name, 'OTP_SBL_WPROT4'):
            otp_sbl_wprot4_str = self.config.get(self._config_holder.section_name, 'OTP_SBL_WPROT4')
            local_dict['OTP_SBL_WPROT4'] = int(otp_sbl_wprot4_str, 16)
        else:
            self.logger.info("OTP_SBL_WPROT4: Flash Write-Protection Word 4 Not Provided \n")
            return None, None

        if self.config.has_option(self._config_holder.section_name, 'OTP_SBL_WPROT5'):
            otp_sbl_wprot5_str = self.config.get(self._config_holder.section_name, 'OTP_SBL_WPROT5')
            local_dict['OTP_SBL_WPROT5'] = int(otp_sbl_wprot5_str, 16)
        else:
            self.logger.info("OTP_SBL_WPROT5: Flash Write-Protection Word 5 Not Provided \n")
            return None, None

        if self.config.has_option(self._config_holder.section_name, 'OTP_SBL_WPROT6'):
            otp_sbl_wprot6_str = self.config.get(self._config_holder.section_name, 'OTP_SBL_WPROT6')
            local_dict['OTP_SBL_WPROT6'] = int(otp_sbl_wprot6_str, 16)
        else:
            self.logger.info("OTP_SBL_WPROT6: Flash Write-Protection Word 6 Not Provided \n")
            return None, None

        if self.config.has_option(self._config_holder.section_name, 'OTP_SBL_WPROT7'):
            otp_sbl_wprot7_str = self.config.get(self._config_holder.section_name, 'OTP_SBL_WPROT7')
            local_dict['OTP_SBL_WPROT7'] = int(otp_sbl_wprot7_str, 16)
        else:
            self.logger.info("OTP_SBL_WPROT7: Flash Write-Protection Word 7 Not Provided \n")
            return None, None

        # OTP_SBL_RPROT0-7: Flash copy/read-protection bits
        if self.config.has_option(self._config_holder.section_name, 'OTP_SBL_RPROT0'):
            otp_sbl_rprot0_str = self.config.get(self._config_holder.section_name, 'OTP_SBL_RPROT0')
            local_dict['OTP_SBL_RPROT0'] = int(otp_sbl_rprot0_str, 16)
        else:
            self.logger.info("OTP_SBL_RPROT0: Flash Copy/Read-Protection Word 0 Not Provided \n")
            return None, None

        if self.config.has_option(self._config_holder.section_name, 'OTP_SBL_RPROT1'):
            otp_sbl_rprot1_str = self.config.get(self._config_holder.section_name, 'OTP_SBL_RPROT1')
            local_dict['OTP_SBL_RPROT1'] = int(otp_sbl_rprot1_str, 16)
        else:
            self.logger.info("OTP_SBL_RPROT1: Flash Copy/Read-Protection Word 1 Not Provided \n")
            return None, None

        if self.config.has_option(self._config_holder.section_name, 'OTP_SBL_RPROT2'):
            otp_sbl_rprot2_str = self.config.get(self._config_holder.section_name, 'OTP_SBL_RPROT2')
            local_dict['OTP_SBL_RPROT2'] = int(otp_sbl_rprot2_str, 16)
        else:
            self.logger.info("OTP_SBL_RPROT2: Flash Copy/Read-Protection Word 2 Not Provided \n")
            return None, None

        if self.config.has_option(self._config_holder.section_name, 'OTP_SBL_RPROT3'):
            otp_sbl_rprot3_str = self.config.get(self._config_holder.section_name, 'OTP_SBL_RPROT3')
            local_dict['OTP_SBL_RPROT3'] = int(otp_sbl_rprot3_str, 16)
        else:
            self.logger.info("OTP_SBL_RPROT3: Flash Copy/Read-protection Word 3 Not Provided \n")
            return None, None

        if self.config.has_option(self._config_holder.section_name, 'OTP_SBL_RPROT4'):
            otp_sbl_rprot4_str = self.config.get(self._config_holder.section_name, 'OTP_SBL_RPROT4')
            local_dict['OTP_SBL_RPROT4'] = int(otp_sbl_rprot4_str, 16)
        else:
            self.logger.info("OTP_SBL_RPROT4: Flash Copy/Read-protection Word 4 Not Provided \n")
            return None, None

        if self.config.has_option(self._config_holder.section_name, 'OTP_SBL_RPROT5'):
            otp_sbl_rprot5_str = self.config.get(self._config_holder.section_name, 'OTP_SBL_RPROT5')
            local_dict['OTP_SBL_RPROT5'] = int(otp_sbl_rprot5_str, 16)
        else:
            self.logger.info("OTP_SBL_RPROT5: Flash Copy/Read-protection Word 5 Not Provided \n")
            return None, None

        if self.config.has_option(self._config_holder.section_name, 'OTP_SBL_RPROT6'):
            otp_sbl_rprot6_str = self.config.get(self._config_holder.section_name, 'OTP_SBL_RPROT6')
            local_dict['OTP_SBL_RPROT6'] = int(otp_sbl_rprot6_str, 16)
        else:
            self.logger.info("OTP_SBL_RPROT6: Flash Copy/Read-protection Word 6 Not Provided \n")
            return None, None

        if self.config.has_option(self._config_holder.section_name, 'OTP_SBL_RPROT7'):
            otp_sbl_rprot7_str = self.config.get(self._config_holder.section_name, 'OTP_SBL_RPROT7')
            local_dict['OTP_SBL_RPROT7'] = int(otp_sbl_rprot7_str, 16)
        else:
            self.logger.info("OTP_SBL_RPROT7: Flash Copy/Read-protection Word 7 Not Provided \n")
            return None, None

        # OTP_CUST_WPROT0-7: Flash write-protection bits
        if self.config.has_option(self._config_holder.section_name, 'OTP_CUST_WPROT0'):
            otp_cust_wprot0_str = self.config.get(self._config_holder.section_name, 'OTP_CUST_WPROT0')
            local_dict['OTP_CUST_WPROT0'] = int(otp_cust_wprot0_str, 16)
        else:
            self.logger.info("OTP_CUST_WPROT0: Flash Write-Protection Word 0 Not Provided \n")
            return None, None

        if self.config.has_option(self._config_holder.section_name, 'OTP_CUST_WPROT1'):
            otp_cust_wprot1_str = self.config.get(self._config_holder.section_name, 'OTP_CUST_WPROT1')
            local_dict['OTP_CUST_WPROT1'] = int(otp_cust_wprot1_str, 16)
        else:
            self.logger.info("OTP_CUST_WPROT1: Flash Write-Protection Word 1 Not Provided \n")
            return None, None

        if self.config.has_option(self._config_holder.section_name, 'OTP_CUST_WPROT2'):
            otp_cust_wprot2_str = self.config.get(self._config_holder.section_name, 'OTP_CUST_WPROT2')
            local_dict['OTP_CUST_WPROT2'] = int(otp_cust_wprot2_str, 16)
        else:
            self.logger.info("OTP_CUST_WPROT2: Flash Write-Protection Word 2 Not Provided \n")
            return None, None

        if self.config.has_option(self._config_holder.section_name, 'OTP_CUST_WPROT3'):
            otp_cust_wprot3_str = self.config.get(self._config_holder.section_name, 'OTP_CUST_WPROT3')
            local_dict['OTP_CUST_WPROT3'] = int(otp_cust_wprot3_str, 16)
        else:
            self.logger.info("OTP_CUST_WPROT3: Flash Write-Protection Word 3 Not Provided \n")
            return None, None

        if self.config.has_option(self._config_holder.section_name, 'OTP_CUST_WPROT4'):
            otp_cust_wprot4_str = self.config.get(self._config_holder.section_name, 'OTP_CUST_WPROT4')
            local_dict['OTP_CUST_WPROT4'] = int(otp_cust_wprot4_str, 16)
        else:
            self.logger.info("OTP_CUST_WPROT4: Flash Write-Protection Word 4 Not Provided \n")
            return None, None

        if self.config.has_option(self._config_holder.section_name, 'OTP_CUST_WPROT5'):
            otp_cust_wprot5_str = self.config.get(self._config_holder.section_name, 'OTP_CUST_WPROT5')
            local_dict['OTP_CUST_WPROT5'] = int(otp_cust_wprot5_str, 16)
        else:
            self.logger.info("OTP_CUST_WPROT5: Flash Write-Protection Word 5 Not Provided \n")
            return None, None

        if self.config.has_option(self._config_holder.section_name, 'OTP_CUST_WPROT6'):
            otp_cust_wprot6_str = self.config.get(self._config_holder.section_name, 'OTP_CUST_WPROT6')
            local_dict['OTP_CUST_WPROT6'] = int(otp_cust_wprot6_str, 16)
        else:
            self.logger.info("OTP_CUST_WPROT6: Flash Write-Protection Word 6 Not Provided \n")
            return None, None

        if self.config.has_option(self._config_holder.section_name, 'OTP_CUST_WPROT7'):
            otp_cust_wprot7_str = self.config.get(self._config_holder.section_name, 'OTP_CUST_WPROT7')
            local_dict['OTP_CUST_WPROT7'] = int(otp_cust_wprot7_str, 16)
        else:
            self.logger.info("OTP_CUST_WPROT7: Flash Write-Protection Word 7 Not Provided \n")
            return None, None

        # OTP_CUST_RPROT0-7: Flash copy/read-protection bits
        if self.config.has_option(self._config_holder.section_name, 'OTP_CUST_RPROT0'):
            otp_cust_rprot0_str = self.config.get(self._config_holder.section_name, 'OTP_CUST_RPROT0')
            local_dict['OTP_CUST_RPROT0'] = int(otp_cust_rprot0_str, 16)
        else:
            self.logger.info("OTP_CUST_RPROT0: Flash Copy/Read-Protection Word 0 Not Provided \n")
            return None, None

        if self.config.has_option(self._config_holder.section_name, 'OTP_CUST_RPROT1'):
            otp_cust_rprot1_str = self.config.get(self._config_holder.section_name, 'OTP_CUST_RPROT1')
            local_dict['OTP_CUST_RPROT1'] = int(otp_cust_rprot1_str, 16)
        else:
            self.logger.info("OTP_CUST_RPROT1: Flash Copy/Read-Protection Word 1 Not Provided \n")
            return None, None

        if self.config.has_option(self._config_holder.section_name, 'OTP_CUST_RPROT2'):
            otp_cust_rprot2_str = self.config.get(self._config_holder.section_name, 'OTP_CUST_RPROT2')
            local_dict['OTP_CUST_RPROT2'] = int(otp_cust_rprot2_str, 16)
        else:
            self.logger.info("OTP_CUST_RPROT2: Flash Copy/Read-Protection Word 2 Not Provided \n")
            return None, None

        if self.config.has_option(self._config_holder.section_name, 'OTP_CUST_RPROT3'):
            otp_cust_rprot3_str = self.config.get(self._config_holder.section_name, 'OTP_CUST_RPROT3')
            local_dict['OTP_CUST_RPROT3'] = int(otp_cust_rprot3_str, 16)
        else:
            self.logger.info("OTP_CUST_RPROT3: Flash Copy/Read-Protection Word 3 Not Provided \n")
            return None, None

        if self.config.has_option(self._config_holder.section_name, 'OTP_CUST_RPROT4'):
            otp_cust_rprot4_str = self.config.get(self._config_holder.section_name, 'OTP_CUST_RPROT4')
            local_dict['OTP_CUST_RPROT4'] = int(otp_cust_rprot4_str, 16)
        else:
            self.logger.info("OTP_CUST_RPROT4: Flash Copy/Read-Protection Word 4 Not Provided \n")
            return None, None

        if self.config.has_option(self._config_holder.section_name, 'OTP_CUST_RPROT5'):
            otp_cust_rprot5_str = self.config.get(self._config_holder.section_name, 'OTP_CUST_RPROT5')
            local_dict['OTP_CUST_RPROT5'] = int(otp_cust_rprot5_str, 16)
        else:
            self.logger.info("OTP_CUST_RPROT5: Flash Copy/Read-Protection Word 5 Not Provided \n")
            return None, None

        if self.config.has_option(self._config_holder.section_name, 'OTP_CUST_RPROT6'):
            otp_cust_rprot6_str = self.config.get(self._config_holder.section_name, 'OTP_CUST_RPROT6')
            local_dict['OTP_CUST_RPROT6'] = int(otp_cust_rprot6_str, 16)
        else:
            self.logger.info("OTP_CUST_RPROT6: Flash Copy/Read-Protection Word 6 Not Provided \n")
            return None, None

        if self.config.has_option(self._config_holder.section_name, 'OTP_CUST_RPROT7'):
            otp_cust_rprot7_str = self.config.get(self._config_holder.section_name, 'OTP_CUST_RPROT7')
            local_dict['OTP_CUST_RPROT7'] = int(otp_cust_rprot7_str, 16)
        else:
            self.logger.info("OTP_CUST_RPROT7: Flash Copy/Read-Protection Word 7 Not Provided \n")
            return None, None

        # OTP_DCU_DISABLEOVERRIDE
        if self.config.has_option(self._config_holder.section_name, 'OTP_DCU_DISABLEOVERRIDE'):
            am_otp_dcu_disableoverride = self.config.get(self._config_holder.section_name, 'OTP_DCU_DISABLEOVERRIDE')
            local_dict['OTP_DCU_DISABLEOVERRIDE'] = int(am_otp_dcu_disableoverride, 16)
        else:
            self.logger.info("OTP_DCU_DISABLEOVERRIDE: OTP DCU override setting not provided \n")
            return None, None

        # OTP_SEC_POL
        # OTP_SEC_POL.AUTH_ENF_ECC
        if self.config.has_option(self._config_holder.section_name, 'OTP_SEC_POL.AUTH_ENF_ECC'):
            opt_sec_pol_auth_enf_ecc = int(self.config.get(self._config_holder.section_name, 'OTP_SEC_POL.AUTH_ENF_ECC'),16)
            opt_sec_pol_auth_enf_ecc = opt_sec_pol_auth_enf_ecc << 29
        else:
            self.logger.info("OTP_SEC_POL.AUTH_ENF_ECC: Security Policy Not Provided \n")
            return None, None

        # OTP__SEC_POL.ENC_ENFORCE
        if self.config.has_option(self._config_holder.section_name, 'OTP_SEC_POL.ENC_ENFORCE'):
            opt_sec_pol_enc_force = int(self.config.get(self._config_holder.section_name, 'OTP_SEC_POL.ENC_ENFORCE'),16)
            opt_sec_pol_enc_force = opt_sec_pol_enc_force << 26
        else:
            self.logger.info("OTP_SEC_POL.ENC_ENFORCE: Security Policy Not Provided \n")
            return None, None

        # OTP_SEC_POL.AUTH_ENFORCE
        if self.config.has_option(self._config_holder.section_name, 'OTP_SEC_POL.AUTH_ENFORCE'):
            opt_sec_pol_auth_enforce = int(self.config.get(self._config_holder.section_name, 'OTP_SEC_POL.AUTH_ENFORCE'),16)
            opt_sec_pol_auth_enforce = opt_sec_pol_auth_enforce << 23
        else:
            self.logger.info("OTP_SEC_POL.AUTH_ENFORCE: Security Policy Not Provided \n")
            return None, None

        opt_sec_pol = opt_sec_pol_auth_enf_ecc | opt_sec_pol_enc_force | \
                    opt_sec_pol_auth_enforce
        local_dict['OTP_SEC_POL'] = opt_sec_pol

        # OTP_BOOT_OVERRIDE
        # OTP_BOOT_OVERRIDE.ENABLE
        opt_boot_override = 0
        if self.config.has_option(self._config_holder.section_name, 'OTP_BOOT_OVERRIDE.ENABLE'):
            opt_boot_override_enable = int(self.config.get(self._config_holder.section_name, 'OTP_BOOT_OVERRIDE.ENABLE'), 16)
            opt_boot_override_enable = opt_boot_override_enable << 9
        else:
            self.logger.info("OTP_BOOT_OVERRIDE.ENABLE: Security Policy Not Provided \n")
            return None, None

        # OTP_BOOT_OVERRIDE.POL
        if self.config.has_option(self._config_holder.section_name, 'OTP_BOOT_OVERRIDE.POL'):
            opt_boot_override_pol = int(self.config.get(self._config_holder.section_name, 'OTP_BOOT_OVERRIDE.POL'), 16)
            opt_boot_override_pol = opt_boot_override_pol << 8
        else:
            self.logger.info("OTP_BOOT_OVERRIDE.POL: Security Policy Not Provided \n")
            return None, None

        # OTP_BOOT_OVERRIDE.GPIO
        if self.config.has_option(self._config_holder.section_name, 'OTP_BOOT_OVERRIDE.GPIO'):
            opt_boot_override_gpio = int(self.config.get(self._config_holder.section_name, 'OTP_BOOT_OVERRIDE.GPIO'), 16)
            opt_boot_override_gpio  = opt_boot_override_gpio
        else:
            self.logger.info("OTP_BOOT_OVERRIDE.GPIO: Security Policy Not Provided \n")
            return None, None

        opt_boot_override = opt_boot_override_enable | opt_boot_override_pol | opt_boot_override_gpio
        local_dict['OTP_BOOT_OVERRIDE'] = opt_boot_override

        # OTP_WIRED_CONFIG
        # OTP_WIRED_CONFIG.UART
        if self.config.has_option(self._config_holder.section_name, 'OTP_WIRED_CONFIG.UART'):
            opt_wired_config_uart = int(self.config.get(self._config_holder.section_name, 'OTP_WIRED_CONFIG.UART'), 16)
            opt_wired_config_uart = opt_wired_config_uart
        else:
            self.logger.info("OTP_WIRED_CONFIG.UART: Security Policy Not Provided \n")
            return None, None

        # OTP_WIRED_CONFIG.SPI
        if self.config.has_option(self._config_holder.section_name, 'OTP_WIRED_CONFIG.SPI'):
            opt_wired_config_spi = int(self.config.get(self._config_holder.section_name, 'OTP_WIRED_CONFIG.SPI'), 16)
            opt_wired_config_spi = opt_wired_config_spi << 1
        else:
            self.logger.info("OTP_WIRED_CONFIG.SPI: Security Policy Not Provided \n")
            return None, None

        # OTP_WIRED_CONFIG.I2C
        if self.config.has_option(self._config_holder.section_name, 'OTP_WIRED_CONFIG.I2C'):
            opt_wired_config_i2c = int(self.config.get(self._config_holder.section_name, 'OTP_WIRED_CONFIG.I2C'), 16)
            opt_wired_config_i2c = opt_wired_config_i2c << 2
        else:
            self.logger.info("OTP_WIRED_CONFIG.I2C: Security Policy Not Provided \n")
            return None, None

        # OTP_WIRED_CONFIG.SLAVEINTPIN
        if self.config.has_option(self._config_holder.section_name, 'OTP_WIRED_CONFIG.SLAVEINTPIN'):
            opt_wired_config_slaveintpin = int(self.config.get(self._config_holder.section_name, 'OTP_WIRED_CONFIG.SLAVEINTPIN'), 16)
            opt_wired_config_slaveintpin = opt_wired_config_slaveintpin << 3
        else:
            self.logger.info("OTP_WIRED_CONFIG.SLAVEINTPIN: Security Policy Not Provided \n")
            return None, None

        # OTP_WIRED_CONFIG.I2CADDR
        if self.config.has_option(self._config_holder.section_name, 'OTP_WIRED_CONFIG.I2CADDR'):
            opt_wired_config_i2caddr = int(self.config.get(self._config_holder.section_name, 'OTP_WIRED_CONFIG.I2CADDR'), 16)
            opt_wired_config_i2caddr = opt_wired_config_i2caddr << 18
        else:
            self.logger.info("OTP_WIRED_CONFIG.SLAVEINTPIN: Security Policy Not Provided \n")
            return None, None

        # OTP_WIRED_CONFIG.UARTMODULE
        if self.config.has_option(self._config_holder.section_name, 'OTP_WIRED_CONFIG.UARTMODULE'):
            opt_wired_config_uartmodule = int(self.config.get(self._config_holder.section_name, 'OTP_WIRED_CONFIG.UARTMODULE'), 16)
            opt_wired_config_uartmodule  = opt_wired_config_uartmodule << 16
        else:
            self.logger.info("OTP_WIRED_CONFIG.UARTMODULE: Security Policy Not Provided \n")
            return None, None

        opt_wired_config = opt_wired_config_uart | opt_wired_config_spi | opt_wired_config_i2c | \
                        opt_wired_config_slaveintpin | opt_wired_config_i2caddr | opt_wired_config_uartmodule
        local_dict['OTP_WIRED_CONFIG'] = opt_wired_config

        # OTP_CUSTOTP_READ_KEY0-3: 128-bit customer keybank read key Word0-3
        if self.config.has_option(self._config_holder.section_name, 'OTP_CUSTOTP_READ_KEY0'):
            opt_custotp_read_key0_str = self.config.get(self._config_holder.section_name, 'OTP_CUSTOTP_READ_KEY0')
            local_dict['OTP_CUSTOTP_READ_KEY0'] = int(opt_custotp_read_key0_str, 16)
        else:
            self.logger.info("OTP_CUSTOTP_READ_KEY0: 128-bit customer keybank read key Word0 Not Provided \n")
            return None, None

        if self.config.has_option(self._config_holder.section_name, 'OTP_CUSTOTP_READ_KEY1'):
            opt_custotp_read_key1_str = self.config.get(self._config_holder.section_name, 'OTP_CUSTOTP_READ_KEY1')
            local_dict['OTP_CUSTOTP_READ_KEY1'] = int(opt_custotp_read_key1_str, 16)
        else:
            self.logger.info("OTP_CUSTOTP_READ_KEY1: 128-bit customer keybank read key Word1 Not Provided \n")
            return None, None

        if self.config.has_option(self._config_holder.section_name, 'OTP_CUSTOTP_READ_KEY2'):
            opt_custotp_read_key2_str = self.config.get(self._config_holder.section_name, 'OTP_CUSTOTP_READ_KEY2')
            local_dict['OTP_CUSTOTP_READ_KEY2'] = int(opt_custotp_read_key2_str, 16)
        else:
            self.logger.info("OTP_CUSTOTP_READ_KEY2: 128-bit customer keybank read key Word2 Not Provided \n")
            return None, None

        if self.config.has_option(self._config_holder.section_name, 'OTP_CUSTOTP_READ_KEY3'):
            opt_custotp_read_key3_str = self.config.get(self._config_holder.section_name, 'OTP_CUSTOTP_READ_KEY3')
            local_dict['OTP_CUSTOTP_READ_KEY3'] = int(opt_custotp_read_key3_str, 16)
        else:
            self.logger.info("OTP_CUSTOTP_READ_KEY3: 128-bit customer keybank read key Word3 Not Provided \n")
            return None, None

        # OTP_CUSTOTP_PROG_KEY0-3 - 128-bit customer keybank PROG key Word0-3
        if self.config.has_option(self._config_holder.section_name, 'OTP_CUSTOTP_PROG_KEY0'):
            opt_custotp_prog_key0_str = self.config.get(self._config_holder.section_name, 'OTP_CUSTOTP_PROG_KEY0')
            local_dict['OTP_CUSTOTP_PROG_KEY0'] = int(opt_custotp_prog_key0_str, 16)
        else:
            self.logger.info("OTP_CUSTOTP_PROG_KEY0: 128-bit Customer Keybank PROG Key Word0 Not Provided \n")
            return None, None

        if self.config.has_option(self._config_holder.section_name, 'OTP_CUSTOTP_PROG_KEY1'):
            opt_custotp_prog_key1_str = self.config.get(self._config_holder.section_name, 'OTP_CUSTOTP_PROG_KEY1')
            local_dict['OTP_CUSTOTP_PROG_KEY1'] = int(opt_custotp_prog_key1_str, 16)
        else:
            self.logger.info("OTP_CUSTOTP_PROG_KEY1: 128-bit Customer Keybank PROG Key Word1 Not Provided \n")
            return None, None

        if self.config.has_option(self._config_holder.section_name, 'OTP_CUSTOTP_PROG_KEY2'):
            opt_custotp_prog_key2_str = self.config.get(self._config_holder.section_name, 'OTP_CUSTOTP_PROG_KEY2')
            local_dict['OTP_CUSTOTP_PROG_KEY2'] = int(opt_custotp_prog_key2_str, 16)
        else:
            self.logger.info("OTP_CUSTOTP_PROG_KEY2: 128-bit Customer Keybank PROG Key Word2 Not Provided \n")
            return None, None

        if self.config.has_option(self._config_holder.section_name, 'OTP_CUSTOTP_PROG_KEY3'):
            opt_custotp_prog_key3_str = self.config.get(self._config_holder.section_name, 'OTP_CUSTOTP_PROG_KEY3')
            local_dict['OTP_CUSTOTP_PROG_KEY3'] = int(opt_custotp_prog_key3_str, 16)
        else:
            self.logger.info("OTP_CUSTOTP_PROG_KEY3: 128-bit Customer Keybank PROG Key Word3 Not Provided \n")
            return None, None

        # OTP_CUSTOTP_PROGLOCK: Customer keybank program lock
        if self.config.has_option(self._config_holder.section_name, 'OTP_CUSTOTP_PROGLOCK'):
            am_otp_prog_lock_str = self.config.get(self._config_holder.section_name, 'OTP_CUSTOTP_PROGLOCK')
            local_dict['OTP_CUSTOTP_PROGLOCK'] = int(am_otp_prog_lock_str, 16)
        else:
            self.logger.info("OTP_CUSTOTP_PROGLOCK: Customer Keybank Program Lock Not Provided \n")
            return None, None

        # OTP_CUSTOTP_RDLOCK: Customer keybank read lock
        if self.config.has_option(self._config_holder.section_name, 'OTP_CUSTOTP_RDLOCK'):
            am_otp_read_lock_str = self.config.get(self._config_holder.section_name, 'OTP_CUSTOTP_RDLOCK')
            local_dict['OTP_CUSTOTP_RDLOCK'] = int(am_otp_read_lock_str, 16)
        else:
            self.logger.info("OTP_CUSTOTP_RDLOCK: Customer Keybank Read Lock Not Provided \n")
            return None, None

        # KeyBanks (0x1c00-0x1fcc)
        # Check for the Key Bank 0 keys bin file path
        if self.config.has_option(self._config_holder.section_name, 'key-bank-0-File'):
            key_bank_0_str = self.config.get(self._config_holder.section_name, 'key-bank-0-File')
            print(key_bank_0_str)

            # check if this is a valid path
            if os.path.isfile(key_bank_0_str):
                fileSize = os.path.getsize(key_bank_0_str)
                # The key size is 16 bytes (128 bit )
                if fileSize != KEY_BANK_DATA_SIZE:
                    self.logger.info("option " + "Key Bank 0 - the key file has invalid size \n")
                    return None, None
                local_dict['key_bank_0_File'] = key_bank_0_str
            else:
                self.logger.info("option " + key_bank_0_str + " file not found \n")
                return None, None
        else:
            self.logger.info("key-bank-0-File not provided \n")
            return None, None

        # Check for the Key Bank 1 keys bin file path
        if self.config.has_option(self._config_holder.section_name, 'key-bank-1-File'):
            key_bank_1_str = self.config.get(self._config_holder.section_name, 'key-bank-1-File')
            print(key_bank_1_str)
            # check if this is a valid path
            if os.path.isfile(key_bank_1_str):
                fileSize = os.path.getsize(key_bank_1_str)
                # The key size is 16 bytes (128 bit )
                if fileSize != KEY_BANK_DATA_SIZE:
                    self.logger.info("option " + "Key Bank 1 - the key file has invalid size \n")
                    return None, None
                local_dict['key_bank_1_File'] = key_bank_1_str
            else:
                self.logger.info("option " + key_bank_1_str + " file not found \n")
                return None, None
        else:
            self.logger.info("key-bank-1-File not provided \n")
            return None, None

            # Check for the Key Bank 2 keys bin file path
        if self.config.has_option(self._config_holder.section_name, 'key-bank-2-File'):
            key_bank_2_str = self.config.get(self._config_holder.section_name, 'key-bank-2-File')
            print(key_bank_2_str)
            # check if this is a valid path
            if os.path.isfile(key_bank_2_str):
                fileSize = os.path.getsize(key_bank_2_str)
                # The key size is 16 bytes (128 bit )
                if fileSize != KEY_BANK_DATA_SIZE:
                    self.logger.info("option " + "Key Bank 2 - the key file has invalid size \n")
                    return None, None
                local_dict['key_bank_2_File'] = key_bank_2_str
            else:
                self.logger.info("option " + key_bank_2_str + " file not found \n")
                return None, None
        else:
            self.logger.info("key-bank-2-File not provided \n")
            return None, None

        # Check for the Key Bank 3 keys bin file path
        if self.config.has_option(self._config_holder.section_name, 'key-bank-3-File'):
            key_bank_3_str = self.config.get(self._config_holder.section_name, 'key-bank-3-File')
            print(key_bank_3_str)
            # check if this is a valid path
            if os.path.isfile(key_bank_3_str):
                fileSize = os.path.getsize(key_bank_3_str)
                # The key size is 16 bytes (128 bit )
                if fileSize != KEY_BANK_DATA_SIZE:
                    self.logger.info("option " + "Key Bank 3 - the key file has invalid size \n")
                    return None, None
                local_dict['key_bank_3_File'] = key_bank_3_str
            else:
                self.logger.info("option " + key_bank_3_str + " file not found \n")
                return None, None
        else:
            self.logger.info("key-bank-3-File not provided \n")
            return None, None

        return local_dict

##################################
# close files and exit script
##################################
def exit_main_func(log_file, config_file, rc):
    log_file.close()
    config_file.close()
    sys.exit(rc)

###########################################
# Main
#########################################
def main():
    # parse arguzments
    the_argument_parser = ArgumentParser()
    the_argument_parser.parse_arguments()
    # get logging up and running
    logger_config = loggerinitializer.LoggerInitializer(the_argument_parser.log_filename)
    logger = logging.getLogger()

    logger.info("OEM Specific Asset Generator started (Logging to " + LOG_FILENAME + ")")

    config_parser = AssetGenConfigParser(the_argument_parser.cfg_filename)

    # TODO: Remove the dictionaries and access values via config object getters
    data_dict = {}
    data_dict = config_parser.parse_config()

    if data_dict is None:
        logger.critical("Config file parsing is not successful")
        sys.exit(-1)

    if (data_dict != None):

        oemAsset_bin = bytearray(OEM_ASSET_SIZE)
        fill_data = int('0x00000000', 16)

        # Get the key bank data
        keyBank0_size, keyBank0_data = GetDataFromBinFile(data_dict['key_bank_0_File'])
        keyBank1_size, keyBank1_data = GetDataFromBinFile(data_dict['key_bank_1_File'])
        keyBank2_size, keyBank2_data = GetDataFromBinFile(data_dict['key_bank_2_File'])
        keyBank3_size, keyBank3_data = GetDataFromBinFile(data_dict['key_bank_3_File'])

        # OTP_SBL_WPROT0-7: Flash write-protection bits (0x1800 - 0x181c)
        # 32 bytes
        copyBytes(oemAsset_bin, data_dict['OTP_SBL_WPROT0'].to_bytes(4, byteorder="little"), 0, 4)
        #print('OTP_SBL_WPROT0: %s' % hex(int('0x1800',16) + 0))

        copyBytes(oemAsset_bin, data_dict['OTP_SBL_WPROT1'].to_bytes(4, byteorder="little"), 4, 4)
        #print('OTP_SBL_WPROT1: %s' % hex(int('0x1800',16) + 4))

        copyBytes(oemAsset_bin, data_dict['OTP_SBL_WPROT2'].to_bytes(4, byteorder="little"), 8, 4)
        #print('OTP_SBL_WPROT2: %s' % hex(int('0x1800', 16) + 8))

        copyBytes(oemAsset_bin, data_dict['OTP_SBL_WPROT3'].to_bytes(4, byteorder="little"), 12, 4)
        #print('OTP_SBL_WPROT3: %s' % hex(int('0x1800', 16) + 12))

        copyBytes(oemAsset_bin, data_dict['OTP_SBL_WPROT4'].to_bytes(4, byteorder="little"), 16, 4)
        #print('OTP_SBL_WPROT4: %s' % hex(int('0x1800', 16) + 16))

        copyBytes(oemAsset_bin, data_dict['OTP_SBL_WPROT5'].to_bytes(4, byteorder="little"), 20, 4)
        #print('OTP_SBL_WPROT5: %s' % hex(int('0x1800', 16) + 20))

        copyBytes(oemAsset_bin, data_dict['OTP_SBL_WPROT6'].to_bytes(4, byteorder="little"), 24, 4)
        #print('OTP_SBL_WPROT6: %s' % hex(int('0x1800', 16) + 24))

        copyBytes(oemAsset_bin, data_dict['OTP_SBL_WPROT7'].to_bytes(4, byteorder="little"), 28, 4)
        #print('OTP_SBL_WPROT7: %s' % hex(int('0x1800', 16) + 28))

        # OTP_SBL_RPROT0-7: Flash copy/read-protection bits (0x1820-0x183c)
        # 32 bytes
        copyBytes(oemAsset_bin, data_dict['OTP_SBL_RPROT0'].to_bytes(4, byteorder="little"), 32, 4)
        #print('OTP_SBL_RPROT0: %s' % hex(int('0x1800', 16) + 32))

        copyBytes(oemAsset_bin, data_dict['OTP_SBL_RPROT1'].to_bytes(4, byteorder="little"), 36, 4)
        #print('OTP_SBL_RPROT1: %s' % hex(int('0x1800', 16) + 36))

        copyBytes(oemAsset_bin, data_dict['OTP_SBL_RPROT2'].to_bytes(4, byteorder="little"), 40, 4)
        #print('OTP_SBL_RPROT2: %s' % hex(int('0x1800', 16) + 40))

        copyBytes(oemAsset_bin, data_dict['OTP_SBL_RPROT3'].to_bytes(4, byteorder="little"), 44, 4)
        #print('OTP_SBL_RPROT3: %s' % hex(int('0x1800', 16) + 44))

        copyBytes(oemAsset_bin, data_dict['OTP_SBL_RPROT4'].to_bytes(4, byteorder="little"), 48, 4)
        #print('OTP_SBL_RPROT4: %s' % hex(int('0x1800', 16) + 48))

        copyBytes(oemAsset_bin, data_dict['OTP_SBL_RPROT5'].to_bytes(4, byteorder="little"), 52, 4)
        #print('OTP_SBL_RPROT5: %s' % hex(int('0x1800', 16) + 52))

        copyBytes(oemAsset_bin, data_dict['OTP_SBL_RPROT6'].to_bytes(4, byteorder="little"), 56, 4)
        #print('OTP_SBL_RPROT6: %s' % hex(int('0x1800', 16) + 56))

        copyBytes(oemAsset_bin, data_dict['OTP_SBL_RPROT7'].to_bytes(4, byteorder="little"), 60, 4)
        #print('OTP_SBL_RPROT7: %s' % hex(int('0x1800', 16) + 60))

        # OTP_DCU_DISABLEOVERRIDE
        copyBytes(oemAsset_bin, data_dict['OTP_DCU_DISABLEOVERRIDE'].to_bytes(4, byteorder="little"), 64, 4)
        # print('OTP_DCU_DISABLEOVERRIDE: %s' % hex(int('0x1800', 16) + 64))

        # OTP_SEC_POL: Security policy (0x184c)
        copyBytes(oemAsset_bin, data_dict['OTP_SEC_POL'].to_bytes(4, byteorder="little"), 76, 4)
        #print('OTP_SEC_POL: %s' % hex(int('0x1800', 16) + 76))

        # OTP_BOOT_OVERRIDE: Boot override (0x1850)
        copyBytes(oemAsset_bin, data_dict['OTP_BOOT_OVERRIDE'].to_bytes(4, byteorder="little"), 80, 4)
        #print('OTP_BOOT_OVERRIDE: %s' % hex(int('0x1800', 16) + 80))

        # OTP_WIRED_CONFIG: Wired configuration (0x1854)
        copyBytes(oemAsset_bin, data_dict['OTP_WIRED_CONFIG'].to_bytes(4, byteorder="little"), 84, 4)
        #print('OTP_WIRED_CONFIG: %s' % hex(int('0x1800', 16) + 84))

        # OTP_CUST_WPROT0-7: Flash write-protection bits (0x1858-0x1878)
        # 32 bytes
        copyBytes(oemAsset_bin, data_dict['OTP_CUST_WPROT0'].to_bytes(4, byteorder="little"), 88, 4)
        #print('OTP_CUST_WPROT0: %s' % hex(int('0x1800', 16) + 88))

        copyBytes(oemAsset_bin, data_dict['OTP_CUST_WPROT1'].to_bytes(4, byteorder="little"), 92, 4)
        #print('OTP_CUST_WPROT1: %s' % hex(int('0x1800', 16) + 92))

        copyBytes(oemAsset_bin, data_dict['OTP_CUST_WPROT2'].to_bytes(4, byteorder="little"), 96, 4)
        #print('OOTP_CUST_WPROT2: %s' % hex(int('0x1800', 16) + 96))

        copyBytes(oemAsset_bin, data_dict['OTP_CUST_WPROT3'].to_bytes(4, byteorder="little"), 100, 4)
        #print('OOTP_CUST_WPROT3: %s' % hex(int('0x1800', 16) + 100))

        copyBytes(oemAsset_bin, data_dict['OTP_CUST_WPROT4'].to_bytes(4, byteorder="little"), 104, 4)
        #print('OTP_CUST_WPROT4: %s' % hex(int('0x1800', 16) + 104))

        copyBytes(oemAsset_bin, data_dict['OTP_CUST_WPROT5'].to_bytes(4, byteorder="little"), 108, 4)
        #print('OTP_CUST_WPROT5: %s' % hex(int('0x1800', 16) + 108))

        copyBytes(oemAsset_bin, data_dict['OTP_CUST_WPROT6'].to_bytes(4, byteorder="little"), 112, 4)
        #print('OOTP_CUST_WPROT6: %s' % hex(int('0x1800', 16) + 112))

        copyBytes(oemAsset_bin, data_dict['OTP_CUST_WPROT7'].to_bytes(4, byteorder="little"), 116, 4)
        #print('OOTP_CUST_WPROT7: %s' % hex(int('0x1800', 16) + 116))

        # OTP_CUSTOTP_PROGLOCK - Customer keybank program lock (0x1878)
        copyBytes(oemAsset_bin, data_dict['OTP_CUSTOTP_PROGLOCK'].to_bytes(4, byteorder="little"), 120, 4)
        #print('OTP_CUSTOTP_PROGLOCK: %s' % hex(int('0x1800', 16) + 120))

        # OTP_CUSTOTP_RDLOCK - Customer keybank read lock (0x187C)
        copyBytes(oemAsset_bin, data_dict['OTP_CUSTOTP_RDLOCK'].to_bytes(4, byteorder="little"), 124, 4)
        #print('OTP_CUSTOTP_RDLOCK: %s' % hex(int('0x1800', 16) + 124))

        # 40 bytes
        # OTP_CUSTOTP_READ_KEY0-3: 128-bit customer keybank read key Word0-3 (0x1a00 - 0x1a0c)
        copyBytes(oemAsset_bin, data_dict['OTP_CUSTOTP_READ_KEY0'].to_bytes(4, byteorder="little"), 512, 4)
        #print('OTP_CUSTOTP_READ_KEY0: %s' % hex(int('0x1800', 16) + 512))

        copyBytes(oemAsset_bin, data_dict['OTP_CUSTOTP_READ_KEY1'].to_bytes(4, byteorder="little"), 516, 4)
        #print('OTP_CUSTOTP_READ_KEY1: %s' % hex(int('0x1800', 16) + 516))

        copyBytes(oemAsset_bin, data_dict['OTP_CUSTOTP_READ_KEY2'].to_bytes(4, byteorder="little"), 520, 4)
        #print('OTP_CUSTOTP_READ_KEY2: %s' % hex(int('0x1800', 16) + 520))

        copyBytes(oemAsset_bin, data_dict['OTP_CUSTOTP_READ_KEY3'].to_bytes(4, byteorder="little"), 524, 4)
        #print('OTP_CUSTOTP_READ_KEY3: %s' % hex(int('0x1800', 16) + 524))

        # OTP_CUSTOTP_PROG_KEY0-3: 128-bit customer keybank PROG key Word0-3 (0x1a10 - 0x1a1c)
        copyBytes(oemAsset_bin, data_dict['OTP_CUSTOTP_PROG_KEY0'].to_bytes(4, byteorder="little"), 528, 4)
        #print('OTP_CUSTOTP_PROG_KEY0: %s' % hex(int('0x1800', 16) + 528))

        copyBytes(oemAsset_bin, data_dict['OTP_CUSTOTP_PROG_KEY1'].to_bytes(4, byteorder="little"), 532, 4)
        #print('OTP_CUSTOTP_PROG_KEY1: %s' % hex(int('0x1800', 16) + 532))

        copyBytes(oemAsset_bin, data_dict['OTP_CUSTOTP_PROG_KEY2'].to_bytes(4, byteorder="little"), 536, 4)
        #print('OTP_CUSTOTP_PROG_KEY2: %s' % hex(int('0x1800', 16) + 536))

        copyBytes(oemAsset_bin, data_dict['OTP_CUSTOTP_PROG_KEY3'].to_bytes(4, byteorder="little"), 540, 4)
        #print('OTP_CUSTOTP_PROG_KEY3: %s' % hex(int('0x1800', 16) + 540))

        # OTP_CUST_RPROT0-7: Flash copy/read-protection bits (0x1A20-0x1A40)
        # 32 bytes
        copyBytes(oemAsset_bin, data_dict['OTP_CUST_RPROT0'].to_bytes(4, byteorder="little"), 544, 4)
        #print('OTP_CUST_RPROT0: %s' % hex(int('0x1800', 16) + 544))

        copyBytes(oemAsset_bin, data_dict['OTP_CUST_RPROT1'].to_bytes(4, byteorder="little"), 548, 4)
        #print('OTP_CUST_RPROT1: %s' % hex(int('0x1800', 16) + 548))

        copyBytes(oemAsset_bin, data_dict['OTP_CUST_RPROT2'].to_bytes(4, byteorder="little"), 552, 4)
        #print('OTP_CUST_RPROT2: %s' % hex(int('0x1800', 16) + 552))

        copyBytes(oemAsset_bin, data_dict['OTP_CUST_RPROT3'].to_bytes(4, byteorder="little"), 556, 4)
        #print('OTP_CUST_RPROT3: %s' % hex(int('0x1800', 16) + 556))

        copyBytes(oemAsset_bin, data_dict['OTP_CUST_RPROT4'].to_bytes(4, byteorder="little"), 560, 4)
        #print('OTP_CUST_RPROT0: %s' % hex(int('0x1800', 16) + 560))

        copyBytes(oemAsset_bin, data_dict['OTP_CUST_RPROT5'].to_bytes(4, byteorder="little"), 564, 4)
        #print('OTP_CUST_RPROT1: %s' % hex(int('0x1800', 16) + 564))

        copyBytes(oemAsset_bin, data_dict['OTP_CUST_RPROT6'].to_bytes(4, byteorder="little"), 568, 4)
        #print('OTP_CUST_RPROT2: %s' % hex(int('0x1800', 16) + 568))

        copyBytes(oemAsset_bin, data_dict['OTP_CUST_RPROT7'].to_bytes(4, byteorder="little"), 572, 4)
        #print('OTP_CUST_RPROT3: %s' % hex(int('0x1800', 16) + 572))

        # 0x00001844: OTP_RSVD613 - Reserved space: 2 words, word offsets # 0x611-# 0x613, byte offsets # 0x1844 - # 0x184C
        # 8 bytes
        word_offset = 4
        for dword in range(2):
            copyBytes(oemAsset_bin, fill_data.to_bytes(4, byteorder="little"), 64 + word_offset, 4)
            #print('Reseverd: %s' % hex(int('0x1800', 16) + 64 + word_offset))
            word_offset = word_offset + 4

        # 0x00001880: OTP_RSVD67F - Reserved space: 96 words, word offsets # 0x620-# 0x67F, byte offsets # 0x1880 - # 0x1A00
        # 384 bytes
        word_offset = 4
        for dword in range(96):
            copyBytes(oemAsset_bin, fill_data.to_bytes(4, byteorder="little"), 124 + word_offset, 4)
            #print('Reseverd: %s' %s' % hex(int('0x1800', 16) + 124 + word_offset))
            word_offset = word_offset + 4

        # 0x00001878: OTP_RSVD6FF - Reserved space: 111 words, word offsets # 0x690-# 0x6FF, byte offsets # 0x1A40 - # 0x1C00
        # 448 bytes
        word_offset = 4
        for dword in range(112):
            copyBytes(oemAsset_bin, fill_data.to_bytes(4, byteorder="little"), 572 + word_offset, 4)
            #print('Reseverd: %s' % hex(int('0x1800', 16) + 572 + word_offset))
            word_offset = word_offset + 4

        # 1024 bytes
        copyBytes(oemAsset_bin, keyBank0_data, 1024, keyBank0_size)
        #print('keyBank0_data: %s' % hex(int('0x1800', 16) + 1024))

        copyBytes(oemAsset_bin, keyBank1_data, 1280, keyBank1_size)
        #print('keyBank1_data: %s' % hex(int('0x1800', 16) + 1280))

        copyBytes(oemAsset_bin, keyBank2_data, 1536, keyBank2_size)
        #print('keyBank2_data: %s' % hex(int('0x1800', 16) + 1536))

        copyBytes(oemAsset_bin, keyBank3_data, 1792, keyBank3_size)
        #print('keyBank3_data: %s' % hex(int('0x1800', 16) + 1792))

        with open(OUTPUT_DIR_NAME + "oem_asset_test_data.bin", "wb") as file:
            file.write(oemAsset_bin)

        logger.info("**** OEM Asset Generation completed successfully ****")
    else:
        logger.critical("**** Invalid config file ****")

#############################
if __name__ == "__main__":
    main()
