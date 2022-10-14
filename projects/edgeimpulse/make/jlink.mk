# see: https://wiki.segger.com/Apollo4
JLINK_DEVICE = AMAP42KK-KBR
JLINK_IF_SPEED = 4000
JLINK_PF_ADDR = 0x18000
JLINK_CPUFREQ = 96105000
JLINK_SWOFREQ = 1000000
JLINK_CF = $(BINDIR)/flash_cmds.jlink

ifeq ($(OS),Windows_NT) # for windows
  JLINK = JLink
  JLINK_SWO = JLinkSWOViewerCL
else # for linux
  JLINK = JLinkExe
  UNAME_S := $(shell uname -s)
  ifeq ($(UNAME_S),Darwin)
    JLINK_SWO = JLinkSWOViewerCL
  else	
    JLINK_SWO = JLinkSWOViewer_CL
  endif
endif

JLINK_CMD = -nogui 1 -device $(JLINK_DEVICE) -if SWD -speed $(JLINK_IF_SPEED) -commandfile $(JLINK_CF)
JLINK_SWO_CMD = -device $(JLINK_DEVICE) -cpufreq $(JLINK_CPUFREQ) -swofreq $(JLINK_SWOFREQ) -itmport 0