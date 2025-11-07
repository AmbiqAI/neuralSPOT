Background:
This tool helps debug power-related issues on Apollo5.
The tool back-end is made up of one .C file and one .h files from <SDK root>/utils/ directory.
The front-end consists of Python files located in the project folder: AmbiqPP_AP5.
PP stands for Power Profiling.

Hardware Prerequires:
a. Ambiq Apollo5 EB with VCOM IOX settings
b. USB cable to the board and PC

Software Prerequires:
a. Ambiq SDK (AmbiqSuite)
b. Python (It is strongly recommended to use 3.8.10 as this was the version tested).
   The Python 3.8.10 installations can be found at https://www.python.org/downloads/release/python-3810/


API definition:
void am_util_pp_snapshot(bool bSingleShot, uint32_t uNumber, bool bStreamNow)
* bSingleShot is a boolean flag to turn on/off the single shot mode. On = single shot is
recommended. OFF=continuous capture mode.
* uNumber is user-defined snapshot number for reference. For sequential captures,
please use continuous numbers, for example, 5000, 5001, 5002 etc. Capture tool
will automatically put sequential column of the date in the XLS file up to 5
continuous snapshots.
* bStreamNow is a Boolean flag to turn the serial port on/off to stream the captured
JSON data out. Please note, it has to be set to false for the initial capture. The
second call for the API can be right after the first call to stream the
just captured data. Or it can be called somewhere after. This scenario is useful for
time-sensitive routines such as inside the interrupt.

Hardware setup:
Open "Device Manager" on your PC and find the assigned serial port under "Ports (COM & LPT)."
Go to the project folder C:\AmbiqPP_AP5.
Open the AmbiqPPConfig.ini file and update the COMPORT setting to match the correct UART port.

Back-end Software Setup:
am_util_pp .c and .h files required for this tool are already integrated into <SDK root>/utils/ directory.
Link your example with these files to build your example.

Please make sure the 'Source' in 'config-template.ini' is using the correct util file:
- Use am_util_pp.c when your target device is Apollo510
- Use am_util_pp_ap510L.c when your target device is Apollo510L.


For reference Coremark example that has integrated the tool:
Set REGDUMP_ENABLE to 1 in ambiq_core_config.h
You can rename this new coremark to your choosing instead of overwriting your current coremark
directory. Clean and make this newly imported example project. When ready Flash the newly built
binary to the board.

Front-end software setup:
Make sure you've installed latest Python3.x already. If not, please download it from here:
https://www.python.org/downloads/
After installation, open a command window and run python to make sure it was installed
correctly.
After installation, please run the following command inside the installed front-end folder,
"c:\AmbiqPP_AP5" in above example. This provides all necessary pre-compiled libraries from Python.
$ pip install -r requirements.txt
If the install is a success, run
$pip freeze

Match the COM port with your PC's port as described in the hardware setup.
Open the AmbiqPPConfig.ini file and update the COMPORT setting to match your PC's port.
Ensure the board's IOX setting is set to provide UART VCOM.

Start the python application code - 'Python AmbiqPP5.py' for Apollo510 and 'Python AmbiqPP510L' for Apollo510L.
Please wait for the prompt "Ready to capture" before resetting your Apollo5 Engineering Board.
Then after you reset your board. You should see serial output and actions from the power profiling
tool in the terminal. The program will keep running until you exit it manually.


Interpret your captured data:

Go to the installed Ambiq Power Profiling Tool output folder, "c:\AmbiqPP_AP5\output", then
double click the just captured XLS file with the snapshot number.
There are two types of data here:
1. "capture.JSON", This is the raw capture file used by the tools to parse the data.
You can share this file with Ambiq FAE for support.
2. "Snapshot_snapnumber_mmdd_time.xlxs" This file contains the parsed data, with highlighted
differences between snapshots.

The most important file to review is the Snapshot_snapnumber_mmdd_time.xlsx. Here, check and compare
power-related registers. The tool automatically highlights any changes between consecutive snapshots
in red, helping you verify if the register settings match the expected or "golden" values.
