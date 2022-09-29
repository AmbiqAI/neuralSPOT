#
# Helper script for jlinkrttlogger
#
import subprocess
import time

timestr = time.strftime("%Y%m%d-%H-%M-%S")

#file name for pcm raw data capture
bin_name = timestr + ".pcm"

#run rtt logger
#fixed _SEGGER_RTT control block starting address for IAR compiler as example
subprocess.run(["jlinkrttlogger.exe", "-Device", 'AMAP42KK-KBR', "-If", "SWD", "-speed", "4000", "-RTTAddress", "0x10060804", "-RTTChannel", "1", bin_name])



