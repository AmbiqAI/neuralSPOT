ifeq ($(ARCH),apollo4)
# see: https://wiki.segger.com/Apollo4
JLINK_DEVICE = AMAP42KK-KBR
JLINK_IF_SPEED = 4000
JLINK_PF_ADDR = 0x18000
JLINK_CPUFREQ = 96105000
JLINK_SWOFREQ = 1000000
JLINK_CF = $(BINDIR)/flash_cmds.jlink
JLINK_RESET_CF = $(BINDIR)/reset_cmds.jlink
JLINK_UNRESET_CF = $(BINDIR)/unreset_cmds.jlink
else ifeq ($(ARCH),apollo3)
JLINK_DEVICE = AMA3B2KK-KCR
JLINK_IF_SPEED = 4000
JLINK_PF_ADDR = 0xc000
JLINK_CPUFREQ = 48855000
JLINK_SWOFREQ = 1000000
JLINK_CF = $(BINDIR)/flash_cmds.jlink
JLINK_RESET_CF = $(BINDIR)/reset_cmds.jlink
JLINK_UNRESET_CF = $(BINDIR)/unreset_cmds.jlink
else ifeq ($(ARCH),apollo5)
JLINK_IF_SPEED = 4000
ifeq ($(BOOTLOADER),nbl)
JLINK_PF_ADDR = 0x00400000
JLINK_DEVICE = AMAP54KK-KBR-nbl
else
JLINK_PF_ADDR = 0x00410000
JLINK_DEVICE = AP510NFA-CBR
endif
JLINK_CPUFREQ = 96105000
JLINK_SWOFREQ = 1000000
JLINK_CF = $(BINDIR)/flash_cmds.jlink
JLINK_RESET_CF = $(BINDIR)/reset_cmds.jlink
JLINK_UNRESET_CF = $(BINDIR)/unreset_cmds.jlink
endif

ifeq ($(OS),Windows_NT) # for windows
  JLINK = JLink
  JLINK_SWO = JLinkSWOViewerCL
else ifndef EXEEXT # for linux

# ifeq ($(PLATFORM),apollo510_evb)
#   JLINK = JLinkExe.794$(EXEEXT)
# else
  JLINK = JLinkExe$(EXEEXT)
# endif

  UNAME_S := $(shell uname -s)
  ifeq ($(UNAME_S),Darwin)
    JLINK_SWO = JLinkSWOViewerCL
  else
    JLINK_SWO = JLinkSWOViewer_CL
  endif
else
  JLINK = JLink.exe
  JLINK_SWO = JLinkSWOViewerCL.exe
endif

JLINK_CMD = -nogui 1 -device $(JLINK_DEVICE) -if SWD -speed $(JLINK_IF_SPEED) -commandfile $(JLINK_CF)
JLINK_RESET_CMD = -nogui 1 -device $(JLINK_DEVICE) -if SWD -speed $(JLINK_IF_SPEED) -commandfile $(JLINK_RESET_CF)
JLINK_UNRESET_CMD = -nogui 1 -device $(JLINK_DEVICE) -if SWD -speed $(JLINK_IF_SPEED) -commandfile $(JLINK_UNRESET_CF)
ifeq ($(PLATFORM),apollo510_evb)
JLINK_SWO_CMD = -device AP510NFA-CBR  -cpufreq $(JLINK_CPUFREQ) -swofreq $(JLINK_SWOFREQ) -itmport 0
else
JLINK_SWO_CMD = -device $(JLINK_DEVICE)  -cpufreq $(JLINK_CPUFREQ) -swofreq $(JLINK_SWOFREQ) -itmport 0
endif
