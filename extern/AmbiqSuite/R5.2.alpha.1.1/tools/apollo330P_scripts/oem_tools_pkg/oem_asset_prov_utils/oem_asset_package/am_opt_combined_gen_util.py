import optparse
import configparser
import sys
import os
import logging
sys.path.append(os.path.join(sys.path[0], "..", ".."))

# CURRENT_PATH = sys.path[0]
# # In case the scripts were run from current directory
# CURRENT_PATH_SCRIPTS = os.sep +  ".." + os.sep + "common"
# # this is the scripts local path, from where the program was called
# sys.path.append(CURRENT_PATH+CURRENT_PATH_SCRIPTS)

import configparser
from common_utils import loggerinitializer

LOG_FILENAME = "opt_combined.log"

class ArgumentParser:
    def __init__(self):
        self.cfg_filename = None
        self.log_filename = LOG_FILENAME
        self.parser = optparse.OptionParser(usage="usage: %prog cfg_file [log_filename]")

    # TODO: Ensure that the indexing of these args matches wthe original script
    def parse_arguments(self):
        (options, args) = self.parser.parse_args()
        if len(args) > 2 or len(args) < 1:
            self.parser.error("incorrect number of positional arguments")
        elif len(args) == 2:
            self.log_filename = args[1]
        self.cfg_filename = args[0]

class CombinedGenConfig:
    section_name = "OPT_IMAGE_CFG"

    def __init__(self):
        self._opt_image_filename = None
        self._dmpu_data_filename = None
        self._opt_combined_filename = None

    @property
    def opt_image_filename(self):
        return self._opt_image_filename

    @opt_image_filename.setter
    def opt_image_filname(self, value):
        if isinstance(value, str) is False:
            raise TypeError("Config parameter opt-image-filname must be a string")
        elif value == "":
            raise ValueError("Config parameter opt-image-filname cannot be an empty string!")
        else:
            self._opt_image_filename = value

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

    @property
    def opt_combined_filename(self):
        return self._opt_combined_filename

    @opt_combined_filename.setter
    def opt_combined_filename(self, value):
        if isinstance(value, str) is False:
            raise TypeError("Config parameter opt-combined-filename must be a string")
        elif value == "":
            raise ValueError("Config parameter opt-combined-filename cannot be an empty string!")
        else:
            self._opt_combined_filename = value

class CombinedGenConfigParser:

    def __init__(self, config_filename):
        self.config_filename = config_filename
        self.config = configparser.ConfigParser()
        self.logger = logging.getLogger()
        self._config_holder = CombinedGenConfig()

    def get_config(self):
        return self._config_holder

    def parse_config(self):
        self.logger.info("Parsing config file: " + self.config_filename)
        self.config.read(self.config_filename)

        if not self.config.has_section(self._config_holder.section_name):
            self.logger.warning("section [" + self._config_holder.section_name + "] wasn't found in cfg file\n")
            return False

        if not self.config.has_option(self._config_holder.section_name, 'opt-image-filename'):
            self.logger.warning("opt-image-filename not found")
            return False
        else:
            self._config_holder.opt_image_filename = self.config.get(self._config_holder.section_name, 'opt-image-filename')

        if not self.config.has_option(self._config_holder.section_name, 'dmpu-data-filename'):
            self.logger.warning("dmpu-data-filename not found")
            return False
        else:
            self._config_holder.opt_image_filename = self.config.get(self._config_holder.section_name, 'dmpu-data-filename')

        if not self.config.has_option(self._config_holder.section_name, 'opt-combined-filename'):
            self.logger.warning("opt-combined-filename not found")
            return False
        else:
            self._config_holder.opt_image_filename = self.config.get(self._config_holder.section_name, 'opt-combined-filename')

        return True

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
    config_parser = CombinedGenConfigParser(the_argument_parser.cfg_filename)
    if config_parser.parse_config() is False:
        logger.critical("Config file parsing is not successful")
        sys.exit(-1)

    logger.info("**** Combining opt executable and data file utility started ****")

    config = config_parser.get_config()

    with open(config.opt_image_filename, "r+") as opt_file:
        fo = opt_file.read()
        print(fo)

    # truncate to 0x7000 bytes
    fo.truncate(28672)

    open(config.opt_combined_filename,'wb').write(open(config.opt_image_filename,'rb').read()+open(config.dmpu_data_filename,'rb').read())

    logger.info("**** Truncate completed successfully ****")

#############################
if __name__ == "__main__":
    main()
