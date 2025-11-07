# Copyright (c) 2001-2020, Arm Limited and Contributors. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause OR Arm’s non-OSI source license
#

import sys
import logging
from pathlib import Path

class LoggerInitializer:
    def __init__(self, log_filename, logging_level=logging.INFO):
        # type: (str, int) -> None
        """
        Instantiates the RootLogger which records messages to stdout and log_filename with logging.INFO logging level
        as default.

        :param log_filename: (str) the filename to be used for the created logfile
        :param logging_level: (int) or (str) the desired log level
        """
        if not isinstance(log_filename, str):
            raise TypeError("Parameter log_filename must be a string")
        self.log_filename = log_filename
        self.root_logger = logging.getLogger()
        self.log_formatter = logging.Formatter("%(asctime)s - %(levelname)s: %(message)s")
        self.log_file_handler = logging.FileHandler(str(Path(sys.path[0]).joinpath(self.log_filename)))
        self.log_file_handler.setFormatter(self.log_formatter)
        log_console_handler = logging.StreamHandler(sys.stdout)
        log_console_handler.setFormatter(self.log_formatter)
        self.root_logger.addHandler(self.log_file_handler)
        self.root_logger.addHandler(log_console_handler)
        self.root_logger.setLevel(logging_level)
        self.root_logger.info(" ".join(["Logging of application", "###", sys.path[0], "###",
                                        "started. (Logging to file:", self.log_filename, ")"
                                        ]))

    def get_logger(self):
        # type: () -> logging.Logger
        """
        Returns the root logger. Optional, since calling logging.getLogger() without any parameters also returns it.
        :return: the root logger instance of logging.Logger
        """
        return self.root_logger
