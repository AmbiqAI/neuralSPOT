import optparse
import configparser
import sys
import struct
import os
import logging
sys.path.append(os.path.join(sys.path[0], "..", ".."))
from common_utils import loggerinitializer
from common_utils import cryptolayer
LOG_FILENAME = "asset_prov.log"
ASSET_TYPE_OEM_DATA = 3
DMPU_HBK1_SIZE_IN_WORDS = 4
DMPU_HBK_SIZE_IN_WORDS  = 8
ASSET_SIZE = 16
ASSET_PKG_WORD_SIZE = 16
AES_KEY_NO_KEY = 0
AES_KEY_PLAIN = 1
AES_KEY_ASSET_PKG = 2

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

class ConfigParsingError(Exception):
    """Raised when trying to load a misconfigured CFG file"""
    pass
class AssetProvisionConfig:
    CFG_SECTION_NAME = "DMPU-DATA-CFG"
    def __init__(self):
        self._hbk_1_filename = None
        self._kcp_data_type = None
        self._kcp_filename = None
        self._kce_data_type = None
        self._kce_filename = None
        self._oem_min_version = None
        self._oem_dcu_default_0 = None
        self._oem_dcu_default_1 = None
        self._oem_dcu_default_2 = None
        self._oem_dcu_default_3 = None
        self._oem_gpc_cfg_bits = None
        self._oem_data_type = None
        self._oem_prop_data_filename = None
        self._dmpu_data_filename = None
    @property
    def section_name(self):
        return self.CFG_SECTION_NAME

    @property
    def hbk_1_filename(self):
        return self._hbk_1_filename

    @hbk_1_filename.setter
    def hbk_1_filename(self, value):
        if isinstance(value, str) is False:
            raise TypeError("Config parameter hbk-1-filename must be a string")
        elif value == "":
            raise ValueError("Config parameter hbk-1-filename cannot be an empty string!")
        else:
            self._hbk_1_filename = value

    @property
    def kcp_data_type(self):
        return self._kcp_data_type

    @kcp_data_type.setter
    def kcp_data_type(self, value):
        if isinstance(value, int) is False:
            raise TypeError("Config parameter kcp-data-type must be an integer!")
        elif value not in [0, 1, 2]:
            raise ValueError("invalid input value for config parameter kcp-data-type")
        else:
            self._kcp_data_type = value

    @property
    def kcp_filename(self):
        return self._kcp_filename

    @kcp_filename.setter
    def kcp_filename(self, value):
        if isinstance(value, str) is False:
            raise TypeError("Config parameter kcp-filename must be a string")
        elif value == "":
            raise ValueError("Config parameter kcp-filename cannot be an empty string!")
        else:
            self._kcp_filename = value

    @property
    def kce_data_type(self):
        return self._kce_data_type

    @kce_data_type.setter
    def kce_data_type(self, value):
        if isinstance(value, int) is False:
            raise TypeError("Config parameter kce-data-type must be an integer!")
        elif value not in [0, 1, 2]:
            raise ValueError("invalid input value for config parameter kce-data-type")
        else:
            self._kce_data_type = value
    @property
    def kce_filename(self):
        return self._kce_filename
    @kce_filename.setter
    def kce_filename(self, value):
        if isinstance(value, str) is False:
            raise TypeError("Config parameter kce-filename must be a string")
        elif value == "":
            raise ValueError("Config parameter kce-filename cannot be an empty string!")
        else:
            self._kce_filename = value

    @property
    def oem_min_version(self):
        return self._oem_min_version

    @oem_min_version.setter
    def oem_min_version(self, value):
        if isinstance(value, int) is False:
            raise TypeError("Config parameter oem-min-version must be an integer!")
        # elif value not in [...]:  TODO: Is there a range or accepted value here?
        #     raise ValueError("invalid input value for config parameter oem-min-version")
        else:
            self._oem_min_version = value

    @property
    def oem_dcu_default_0(self):
        return self._oem_dcu_default_0

    @oem_dcu_default_0.setter
    def oem_dcu_default_0(self, value):
        if isinstance(value, int) is False:
            raise TypeError("Config parameter oem-dcu_default_0 must be an integer!")
        # elif value not in [...]:  TODO: Is there a range or accepted value here?
        #     raise ValueError("invalid input value for config parameter oem-dcu_default_0")
        else:
            self._oem_dcu_default_0 = value

    @property
    def oem_dcu_default_1(self):
        return self._oem_dcu_default_1

    @oem_dcu_default_1.setter
    def oem_dcu_default_1(self, value):
        if isinstance(value, int) is False:
            raise TypeError("Config parameter oem-dcu_default_1 must be an integer!")
        # elif value not in [...]:  TODO: Is there a range or accepted value here?
        #     raise ValueError("invalid input value for config parameter oem-dcu_default_1")
        else:
            self._oem_dcu_default_1 = value

    @property
    def oem_dcu_default_2(self):
        return self._oem_dcu_default_2

    @oem_dcu_default_2.setter
    def oem_dcu_default_2(self, value):
        if isinstance(value, int) is False:
            raise TypeError("Config parameter oem-dcu_default_2 must be an integer!")
        # elif value not in [...]:  TODO: Is there a range or accepted value here?
        #     raise ValueError("invalid input value for config parameter oem-dcu_default_2")
        else:
            self._oem_dcu_default_2 = value

    @property
    def oem_dcu_default_3(self):
        return self._oem_dcu_default_3

    @oem_dcu_default_3.setter
    def oem_dcu_default_3(self, value):
        if isinstance(value, int) is False:
            raise TypeError("Config parameter oem-dcu_default_3 must be an integer!")
        # elif value not in [...]:  TODO: Is there a range or accepted value here?
        #     raise ValueError("invalid input value for config parameter oem-dcu_default_3")
        else:
            self._oem_dcu_default_3 = value

    @property
    def oem_gpc_cfg_bits(self):
        return self._oem_gpc_cfg_bits

    @oem_gpc_cfg_bits.setter
    def oem_gpc_cfg_bits(self, value):
        if isinstance(value, int) is False:
            raise TypeError("Config parameter oem-gpc-cfg-bits must be an integer!")
        # elif value not in [...]:  TODO: Is there a range or accepted value here?
        #     raise ValueError("invalid input value for config parameter oem-gpc-cfg-bits")
        else:
            self._oem_gpc_cfg_bits = value
    @property
    def oem_data_type(self):
        return self._oem_data_type

    @oem_data_type.setter
    def oem_data_type(self, value):
        if isinstance(value, int) is False:
            raise TypeError("Config parameter oem-data-type must be an integer!")
        else:
            self._oem_data_type = value

    @property
    def oem_prop_data_filename(self):
        return self._oem_prop_data_filename

    @oem_prop_data_filename.setter
    def oem_prop_data_filename(self, value):
        if isinstance(value, str) is False:
            raise TypeError("Config parameter oem-prop-data-filename must be a string")
        elif value == "":
            raise ValueError("Config parameter oem-prop-data-filename cannot be an empty string!")
        else:
            self._oem_prop_data_filename = value
    @property
    def dmpu_data_filename(self):
        return self._dmpu_data_filename

    @dmpu_data_filename.setter
    def dmpu_data_filename(self, value):
        if isinstance(value, str) is False:
            raise TypeError("Config parameter dmpu-data-filename must be a string")
        elif value == "":
            raise ValueError("Config parameter dmpu-data-filename cannot be an empty string!")
        else:
            self._dmpu_data_filename = value
class AssetProvisionConfigParser:
    HBK1_SIZE   = 16
    AES_KEY_SIZE_PLAIN = 16
    AES_KEY_SIZE_ASSET_PKG  = 64
    OEM_DATA_SIZE_MAX = 2096
    AES_KEY_NO_KEY = 0
    AES_KEY_PLAIN = 1
    AES_KEY_ASSET_PKG = 2
    def __init__(self, config_filename):
        """
        Parses the DMPU-DATA-CFG cfg file.
        Raises ConfigParsingError when cfg has been written incorrectly.
        Raises TypeError or ValueError when a cfg value is incorrect on its own.
        :param str config_filename: name of cfg file to parse
        """
        self.config_filename = config_filename
        self.config = configparser.ConfigParser()
        self.logger = logging.getLogger()
        self._config_holder = AssetProvisionConfig()
    def get_config(self):
        return self._config_holder
    def parse_config (self):
        self.logger.info("Parsing config file: " + self.config_filename)
        self.config.read(self.config_filename)
        if not self.config.has_section(self._config_holder.section_name):
            self.logger.warning("section [" + self._config_holder.section_name + "] wasn't found in cfg file")
            return False

        if not self.config.has_option(self._config_holder.section_name, 'hbk-1-filename'):
            self.logger.warning("hbk-1-filename not found")
            return False
        else:
            self._config_holder.hbk_1_filename = self.config.get(self._config_holder.section_name, 'hbk-1-filename')
        if not self.config.has_option(self._config_holder.section_name, 'kcp-data-type'):
            self.logger.warning("kcp-data-type not found")
            return False
        else:
            self._config_holder.kcp_data_type = int(self.config.get(self._config_holder.section_name, 'kcp-data-type'), 0)
        if not self.config.has_option(self._config_holder.section_name, 'kcp-filename'):
            self.logger.warning("kcp-filename not found")
            return False
        else:
            self._config_holder.kcp_filename = self.config.get(self._config_holder.section_name, 'kcp-filename')
        if not self.config.has_option(self._config_holder.section_name, 'kce-data-type'):
            self.logger.warning("kce-data-type not found")
            return False
        else:
            self._config_holder.kce_data_type = int(self.config.get(self._config_holder.section_name, 'kce-data-type'), 0)
        if not self.config.has_option(self._config_holder.section_name, 'kce-filename'):
            self.logger.warning("kce-filename not found")
            return False
        else:
            self._config_holder.kce_filename = self.config.get(self._config_holder.section_name, 'kce-filename')
        if not self.config.has_option(self._config_holder.section_name, 'oem-min-version'):
            self.logger.warning("oem-min-version not found")
            return False
        else:
            self._config_holder.oem_min_version = int(self.config.get(self._config_holder.section_name, 'oem-min-version'), 0)
        if not self.config.has_option(self._config_holder.section_name, 'oem-dcu-default_0'):
            self.logger.warning("oem-dcu-default_0 not found")
            return False
        else:
            self._config_holder.oem_dcu_default_0 = int(self.config.get(self._config_holder.section_name, 'oem-dcu-default_0'), 0)
        if not self.config.has_option(self._config_holder.section_name, 'oem-dcu-default_1'):
            self.logger.warning("oem-dcu-default_1 not found")
            return False
        else:
            self._config_holder.oem_dcu_default_1 = int(self.config.get(self._config_holder.section_name, 'oem-dcu-default_1'), 0)
        if not self.config.has_option(self._config_holder.section_name, 'oem-dcu-default_2'):
            self.logger.warning("oem-dcu-default_2 not found")
            return False
        else:
            self._config_holder.oem_dcu_default_2 = int(self.config.get(self._config_holder.section_name, 'oem-dcu-default_2'), 0)
        if not self.config.has_option(self._config_holder.section_name, 'oem-dcu-default_3'):
            self.logger.warning("oem-dcu-default_3 not found")
            return False
        else:
            self._config_holder.oem_dcu_default_3 = int(self.config.get(self._config_holder.section_name, 'oem-dcu-default_3'), 0)
        if not self.config.has_option(self._config_holder.section_name, 'oem-gpc-cfg-bits'):
            self.logger.warning("oem-gpc-cfg-bits not found")
            return False
        else:
            self._config_holder._oem_gpc_cfg_bits = int(self.config.get(self._config_holder.section_name, 'oem-gpc-cfg-bits'), 0)
        if not self.config.has_option(self._config_holder.section_name, 'oem-data-type'):
            self.logger.warning("oem-data-type not found")
            return False
        else:
            self._config_holder._oem_data_type = int(self.config.get(self._config_holder.section_name, 'oem-data-type'), 0)

        if ((self._config_holder._oem_data_type != AES_KEY_NO_KEY) or (self._config_holder._oem_data_type == AES_KEY_ASSET_PKG) or (self._config_holder._oem_data_type == AES_KEY_PLAIN)):
            self.logger.info("oem-data-type: " + str(self._config_holder._oem_data_type))
        else:
            self.logger.warning("Invalid oem-data-type")
            return False
        if not self.config.has_option(self._config_holder.section_name, 'oem-prop-data-filename'):
            self.logger.warning("oem-prop-data-filename not found")
            return False
        else:
            self._config_holder.oem_prop_data_filename = self.config.get(self._config_holder.section_name, 'oem-prop-data-filename')
        if not self.config.has_option(self._config_holder.section_name, 'dmpu-data-filename'):
            self.logger.warning("dmpu-data-filename not found")
            return False
        else:
            self._config_holder.dmpu_data_filename = self.config.get(self._config_holder.section_name, 'dmpu-data-filename')
        with open(self._config_holder.hbk_1_filename, "rb") as hbk1_file:
            hbk1_data = hbk1_file.read()
        hbk1_data_size = len(hbk1_data)
        if hbk1_data_size != self.HBK1_SIZE:
            self.logger.warning("Invalid hbk size: " + str(hbk1_data_size))
            return False
        
        if ((self._config_holder.kcp_data_type == self.AES_KEY_NO_KEY) or (self._config_holder.kcp_data_type == self.AES_KEY_ASSET_PKG) or (self._config_holder.kce_data_type == self.AES_KEY_PLAIN)):
            self.logger.info("kcp-data-type: " + str(self._config_holder.kcp_data_type))
        else:
            self.logger.warning("Invalid kcp-data-type")
            return False
        if ((self._config_holder.kce_data_type == self.AES_KEY_NO_KEY) or (self._config_holder.kce_data_type == self.AES_KEY_ASSET_PKG) or (self._config_holder.kce_data_type == self.AES_KEY_PLAIN)):
            self.logger.info("kce-data-type: " + str(self._config_holder.kce_data_type))
        else:
            self.logger.warning("Invalid kce-data-type")
            return False
        # Get kcp file name
        if(self._config_holder.kcp_data_type != self.AES_KEY_NO_KEY):
            filePath_str = self._config_holder.kcp_filename
            if os.path.isfile(filePath_str):
                fileSize = os.path.getsize(filePath_str)
                if (((self._config_holder.kcp_data_type == self.AES_KEY_ASSET_PKG) and (fileSize == self.AES_KEY_SIZE_ASSET_PKG)) or
                    ((self._config_holder.kcp_data_type == self.AES_KEY_PLAIN) and (fileSize == self.AES_KEY_SIZE_PLAIN))):
                    self._config_holder.kcp_filename = filePath_str
                    self.logger.info("kcp-filename: " + str(self._config_holder.kcp_filename))
                else:
                    self.logger.warning("kcp key size Invalid")
                    return False
            else:
                self.logger.info("kcp-filename: " + str(filePath_str) + " File Not Found !!!")
                return False
        else:
            self.logger.warning("\n NO KCP KEY IN PROVISIONING DATA !!! \n")
            #TODO Should we allow to continue if kcp_data_type != self.AES_KEY_NO_KEY. If I'm able to return false here then I don't need to re-check in generate_blob() and dont have to have global var AES_KEY_NO_KEY
        # Get kce file name
        if(self._config_holder.kce_data_type != self.AES_KEY_NO_KEY):
            filePath_str = self._config_holder.kce_filename
            if os.path.isfile(filePath_str):
                fileSize = os.path.getsize(filePath_str)
                if (((self._config_holder.kce_data_type == self.AES_KEY_ASSET_PKG) and (fileSize == self.AES_KEY_SIZE_ASSET_PKG)) or
                    ((self._config_holder.kce_data_type == self.AES_KEY_PLAIN) and (fileSize == self.AES_KEY_SIZE_PLAIN))):
                    self._config_holder.kce_filename = filePath_str
                    self.logger.info("kce-filename: " + str(self._config_holder.kce_filename))
                else:
                    self.logger.warning("kce key size Invalid")
                    return False
            else:
                self.logger.warning("kce-filename: " + str(filePath_str) + " File Not Found !!!")
                return False
        else:
            self.logger.warning("NO KCE KEY IN PROVISIONING DATA !!! \n")
            #TODO Should we allow to continue if kce_data_type != self.AES_KEY_NO_KEY. If I'm able to return false here then I don't need to re-check in generate_blob() and dont have to have global var AES_KEY_NO_KEY
        # Get oem-min-version
        self.logger.info("oem-min-version: " + hex(self._config_holder.oem_min_version))
        # Get oem DCU default data
        self.logger.info("oem-dcu-default_0: " + hex(self._config_holder.oem_dcu_default_0))
        self.logger.info("oem-dcu-default_1: " + hex(self._config_holder.oem_dcu_default_1))
        self.logger.info("oem-dcu-default_2: " + hex(self._config_holder.oem_dcu_default_2))
        self.logger.info("oem-dcu-default_3: " + hex(self._config_holder.oem_dcu_default_3))
        # OEM GPC Configuration and ignore the byte 0, that is reserved for ICV
        self._config_holder.oem_gpc_cfg_bits = (self._config_holder.oem_gpc_cfg_bits & 0xFFFFFF00)
        self.logger.info("oem-gpc-cfg-bits: " + hex(self._config_holder.oem_gpc_cfg_bits))

        # Get oem Prop Data file name
        filePath_str = self._config_holder.oem_prop_data_filename
        if os.path.isfile(filePath_str):
            fileSize = os.path.getsize(filePath_str)
            self.logger.info("fileSize: " + str(fileSize))
            if fileSize > self.OEM_DATA_SIZE_MAX:
                self.logger.warning("Oem Prop Data size more than max limit")
                return False
            self._config_holder.oem_prop_data_filename = filePath_str
            self.logger.info("oem-prop-data-filename: " + str(self._config_holder.oem_prop_data_filename))
        else:
            self.logger.warning("oem-prop-data-filename: " + str(filePath_str) + " File Not Found !!!")
            return False

        # Get the output file name
        self.logger.info("dmpu-data-filename: " + str(self._config_holder.dmpu_data_filename))
        return True

class CCDmpuHbkBuff:
    def __init__(self):
        self.hbk = [0] * (DMPU_HBK_SIZE_IN_WORDS * 4)    # Full Hbk buffer
        self.hbk1 = self.hbk[:DMPU_HBK1_SIZE_IN_WORDS * 4]  # Hbk1 buffer

class DmpuData:
    def __init__(self):
        self.hbkType = 0
        self.kcpDataType = 0
        self.kceDataType = 0
        self.oemMinVersion = 0
        self.oemDcuDefaultLock = [0] * 4
        self.kcp = {'pkgAsset': [0] * ASSET_PKG_WORD_SIZE, 'plainAsset': [0] * ASSET_PKG_WORD_SIZE}
        self.kce = {'pkgAsset': [0] * ASSET_PKG_WORD_SIZE, 'plainAsset': [0] * ASSET_PKG_WORD_SIZE}
        self.hbkBuff = CCDmpuHbkBuff()

class OempuData:
    def __init__(self):
        self.hbkType = 0
        self.hbkBuff = CCDmpuHbkBuff()
        self.oemAssetDataType = AES_KEY_NO_KEY
        self.oemDataBuff = [0] * (ASSET_SIZE // 4)
        
class AmoemProvData:
    def __init__(self):
        self.dmpuData = DmpuData()
        self.oemGpcCfg = 0
        # Calculate reserved size based on size of dmpuData and oemGpcCfg
        dmpu_size = (sys.getsizeof(self.dmpuData))
        self.reserved = [0] * (256 - ((dmpu_size * 4) + 4 ))
        self.oempuData = OempuData()

class AssetProvisionBlobCreator:
    # TODO remove redef. here
    AES_KEY_NO_KEY = 0
    def __init__(self, data_gen_cfg):
        self.config = data_gen_cfg
        self.logger = logging.getLogger()

    # The GetDataFromBinFile gets the data from a binary file
    def GetDataFromBinFile(self, fileName):
        binStr = str()
        try:
            # Open a binary file and write the data to it
            FileObj = open(fileName, "rb")
            binStr = FileObj.read()
            binSize = len(binStr)
            FileObj.close()
        except IOError as Error7:
            (errno, strerror) = Error7.args
            self.logger.critical("Error in openning file - %s" %fileName)
            sys.exit(1)
        return binSize, binStr
    
    def get_key_assets(self, keyfile, keyBuff):
    
        with open(keyfile, "rb") as file:
            binary_data = file.read()

            num_words = len(binary_data) // 4  # Each word is 4 bytes
            remaining_bytes = len(binary_data) % 4

            # If there are remaining bytes that don't form a complete word, handle them
            if remaining_bytes != 0:
                # Pad the binary data with zeros to make it a multiple of 4 bytes
                binary_data += b'\x00' * (4 - remaining_bytes)
                num_words += 1

            # Unpack the binary data into 32-bit unsigned integers
            # Use the number of words we actually have
            unpacked_data = list(struct.unpack(f'{num_words}I', binary_data))

            # Copy the unpacked data into the `data` list
            for i in range(min(len(unpacked_data), len(keyBuff))):
                keyBuff[i] = unpacked_data[i]

            return keyBuff
    
    def generate_blob(self):
        oemProvData = AmoemProvData()
        # Get assets and encrypted key from files
        kcpBuff = [0] * ASSET_PKG_WORD_SIZE
        if(self.config.kcp_data_type != self.AES_KEY_NO_KEY):
            kcpAsset = self.get_key_assets(self.config.kcp_filename, kcpBuff)

        kceBuff = [0] * ASSET_PKG_WORD_SIZE
        if(self.config.kce_data_type != self.AES_KEY_NO_KEY):    
            kceAsset = self.get_key_assets(self.config.kce_filename, kceBuff)
        
        #check if we have OEM Prop Data 
        with open(self.config.oem_prop_data_filename, "rb") as oem_prop_file:
            oemPropAsset = oem_prop_file.read()
        oemPropAssetSize = len(oemPropAsset)
        if(self.config.oem_data_type != AES_KEY_NO_KEY):
            with open(self.config.oem_prop_data_filename, "rb") as oem_prop_file:
                oemPropAsset = oem_prop_file.read()
            oemPropAssetSize = len(oemPropAsset)
        with open(self.config.hbk_1_filename, "rb") as hbk1_file:
            hbk1Data = hbk1_file.read()
        hbk1Size = len(hbk1Data)
        
        hbk1Type = 1 # TODO FIX HARD CODE
        oemProvData.dmpuData.hbkType = hbk1Type
        oemProvData.dmpuData.kcpDataType = self.config.kcp_data_type
        oemProvData.dmpuData.kceDataType = self.config.kce_data_type
        oemProvData.dmpuData.oemMinVersion = self.config.oem_min_version
        oemProvData.dmpuData.oemDcuDefaultLock[0] = self.config.oem_dcu_default_0
        oemProvData.dmpuData.oemDcuDefaultLock[1] = self.config.oem_dcu_default_1
        oemProvData.dmpuData.oemDcuDefaultLock[2] = self.config.oem_dcu_default_2
        oemProvData.dmpuData.oemDcuDefaultLock[3] = self.config.oem_dcu_default_3
        oemProvData.oemGpcCfg = self.config.oem_gpc_cfg_bits
        oemProvData.oempuData.oemAssetDataType = self.config.oem_data_type

        # Get KCP Assets
        if self.config.kcp_data_type == AES_KEY_ASSET_PKG:
            oemProvData.dmpuData.kcp['pkgAsset'] = kcpAsset
        elif self.config.kcp_data_type == AES_KEY_PLAIN:
            oemProvData.dmpuData.kcp['plainAsset'] = kcpAsset
        # Get KCE Assets
        if self.config.kce_data_type == AES_KEY_ASSET_PKG:
            oemProvData.dmpuData.kce['pkgAsset'] = kceAsset
        elif self.config.kce_data_type == AES_KEY_PLAIN:
            oemProvData.dmpuData.kce['plainAsset'] = kceAsset

        # Get HBK1
        oemProvData.dmpuData.hbkBuff.hbk[:DMPU_HBK1_SIZE_IN_WORDS *4] = hbk1Data[:DMPU_HBK1_SIZE_IN_WORDS * 4]

        self.logger.info("prop asset Size " + str(oemPropAssetSize))
        # OEM Prop Asset
        if oemPropAsset and oemPropAssetSize != 0:
            oemProvData.oempuData.oemDataBuff = oemPropAsset[:oemPropAssetSize // 4]

        # TODO: Clean this up, maybe add all this data to an array and then pack it??
        dmpu_blob = struct.pack('<I', oemProvData.dmpuData.hbkType)

        for pkg_word in oemProvData.dmpuData.hbkBuff.hbk:
            dmpu_blob += struct.pack('B', pkg_word)
        
        # Add kcp data type and kcp data to dmpu image
        dmpu_blob += struct.pack('<I', oemProvData.dmpuData.kcpDataType)
        for pkg_word in kcpAsset:
            dmpu_blob += struct.pack('<I', pkg_word)
        
        # Add kce data type and kce data to dmpu image
        dmpu_blob += struct.pack('<I', oemProvData.dmpuData.kceDataType)
        for pkg_word in kceAsset:
            dmpu_blob += struct.pack('<I', pkg_word)

        # Add min version and oem_dcu_default_0-3 and gpc cfg bits to dmpu image
        dmpu_blob += struct.pack('<I', oemProvData.dmpuData.oemMinVersion)

        # Add oem_dcu_default_0-3 to dmpu image
        for dcu in oemProvData.dmpuData.oemDcuDefaultLock:
             dmpu_blob += struct.pack('<I', dcu)

        # Add gpc cfg bits to dmpu image
        dmpu_blob += struct.pack('<I', oemProvData.oemGpcCfg)
        
        # Add reserved bytes as padding to the dmpu image
        resv = bytearray(oemProvData.reserved)
        dmpu_blob += resv

        # Add hbk type to dmpu image 
        dmpu_blob += struct.pack('<I', oemProvData.dmpuData.hbkType)
         
        # Add hbk data to dmpu image 
        for pkg_word in oemProvData.dmpuData.hbkBuff.hbk:
            dmpu_blob += struct.pack('B', pkg_word)

        dmpu_blob += struct.pack('<I', oemProvData.oempuData.oemAssetDataType)  

        for word in oemPropAsset:
            dmpu_blob += struct.pack('B', word)

        # Write the asset package into bin file
        try:
            with open(self.config.dmpu_data_filename, 'wb') as f:
                f.write(dmpu_blob)
                self.logger.info(f"End rc {0}")

        except IOError as e:
            logging.error(f"failed to write to file {self.config.dmpu_data_filename}, error: {e}")
            sys.exit(-1)
        cmd = "xxd -i " +  self.config.dmpu_data_filename + " > dmpu_prov_data.h"
        os.system(cmd)
def main():
    if not (sys.version_info.major == 3 and sys.version_info.minor == 10 and sys.version_info.micro == 12):
        print("The script has been tested with Python 3.10.12!")
    if (sys.version_info.major == 3 and sys.version_info.minor == 9):
        print("*** Ambiq recommends against using Python3.9 ***")
        exit(1)
    # parse arguments
    the_argument_parser = ArgumentParser()
    the_argument_parser.parse_arguments()
    # get logging up and running
    logger_config = loggerinitializer.LoggerInitializer(the_argument_parser.log_filename)
    logger = logging.getLogger()
    # get util configuration parameters
    config_parser = AssetProvisionConfigParser(the_argument_parser.cfg_filename)
    if config_parser.parse_config() is False:
        logger.critical("Config file parsing is not successful")
        sys.exit(-1)

    asset_provisioner = AssetProvisionBlobCreator(config_parser.get_config())
    asset_provisioner.generate_blob()
    logger.info("**** DMPU Data Blob Generate completed successfully ****")
#############################
if __name__ == "__main__":
    main()