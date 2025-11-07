#!/usr/bin/env python3
#
# Copyright (c) 2001-2020, Arm Limited and Contributors. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause OR Arm’s non-OSI source license
#

import optparse
import configparser
import sys
import os
import logging
import struct
sys.path.append(os.path.join(sys.path[0], "..", ".."))

from common_utils import loggerinitializer
from common_cert_lib.developercertificateconfig import DeveloperDebugCertificateConfig
from common_cert_lib.developerdebugarmcertificate import DeveloperDebugArmCertificate

# Adding the utility python scripts to the python PATH
CURRENT_PATH = sys.path[0]
DEFAULT_OUTPUT_DIR_NAME = CURRENT_PATH + '/am_debug_cert_output/'

# Util's log file
LOG_FILENAME = "sb_dbg2_cert.log"

# find proj.cfg
if "proj.cfg" in os.listdir(sys.path[0]):
    PROJ_CONFIG_PATH = os.path.join(sys.path[0], "proj.cfg")
elif "proj.cfg" in os.listdir(sys.path[-1]):
    PROJ_CONFIG_PATH = os.path.join(sys.path[-1], "proj.cfg")
else:
    PROJ_CONFIG_PATH = os.path.join(os.getcwd(), "proj.cfg")

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

class DeveloperDebugCertificateGenerator:
    AM_SBR_OEM_DBG_CERT_SIGN = 0x5A7B318E

    def __init__(self, developer_dbg_cfg, certificate_version):
        self.config = developer_dbg_cfg
        self.logger = logging.getLogger()
        self.cert_version = certificate_version
        self._certificate = None

    def create_certificate(self):
        self.logger.info("**** Generate debug certificate ****")
        serialized_cert_data = b''
        self._certificate = DeveloperDebugArmCertificate(self.config, self.cert_version)
        output_dir = ""

        if self.config.enabler_cert_pkg is not None and self.config.enabler_cert_pkg != "":
            with open(self.config.enabler_cert_pkg, "rb") as enabler_cert_input_file:
                enabler_certificate_data = enabler_cert_input_file.read()

        dbgCertBlob = (struct.pack('<I', self.AM_SBR_OEM_DBG_CERT_SIGN)
                       + struct.pack('<I', len(enabler_certificate_data) + len(self._certificate.certificate_data) ))

        serialized_cert_data += dbgCertBlob

        serialized_cert_data += enabler_certificate_data

        serialized_cert_data += self._certificate.certificate_data

        if os.path.isabs(self.config.cert_pkg):
            os.makedirs(os.path.dirname(self.config.cert_pkg), exist_ok=True)
            if os.path.isdir(os.path.dirname(self.config.cert_pkg)):
                output_dir = os.path.abspath(os.path.dirname(self.config.cert_pkg))
                bin_filename = os.path.basename(self.config.cert_pkg)
        else:
            output_dir = DEFAULT_OUTPUT_DIR_NAME
            os.makedirs(output_dir, exist_ok=True)
            bin_filename = self.config.cert_pkg

        self.logger.info("Write the certificate to file: " + os.path.join(output_dir, bin_filename))

        with open(os.path.join(output_dir, bin_filename), "wb") as bin_output_file:
            bin_output_file.write(serialized_cert_data)

        cmd = "xxd -i " + os.path.join(output_dir, bin_filename)  + " > " + output_dir + "oemDeveloperCert.h"
        os.system(cmd)

if __name__ == "__main__":
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
    # Get the project configuration values
    project_config = configparser.ConfigParser()
    with open(PROJ_CONFIG_PATH, 'r') as project_config_file:
        config_string = '[PROJ-CFG]\n' + project_config_file.read()
    project_config.read_string(config_string)
    cert_version = [project_config.getint("PROJ-CFG", "CERT_VERSION_MAJOR"),
                    project_config.getint("PROJ-CFG", "CERT_VERSION_MINOR")]
    # get util configuration parameters
    developer_certificate_config = DeveloperDebugCertificateConfig(the_argument_parser.cfg_filename)
    # create certificate
    cert_creator = DeveloperDebugCertificateGenerator(developer_certificate_config, cert_version)
    cert_creator.create_certificate()
    logger.info("**** Certificate file creation has been completed successfully ****")
