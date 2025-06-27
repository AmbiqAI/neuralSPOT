include make/helpers.mk
include make/neuralspot_config.mk
include make/neuralspot_toolchain.mk
include make/jlink.mk
include autogen_$(BOARD)_$(EVB)_$(TOOLCHAIN).mk

ifeq ($(TOOLCHAIN),arm)
COMPDIR := armclang
else ifeq ($(TOOLCHAIN),arm-none-eabi)
COMPDIR := gcc
endif

# local_app_name := main <-- moved to autogen
TARGET = $(local_app_name)
sources := $(wildcard src/*.c)
sources += $(wildcard src/*.cc)
sources += $(wildcard src/*.cpp)
sources += $(wildcard src/*.s)
sources += $(wildcard src/ns-core/*.c)
sources += $(wildcard src/ns-core/*.cc)
sources += $(wildcard src/ns-core/*.cpp)
sources += $(wildcard src/ns-core/*.s)
sources += $(wildcard src/ns-core/$(BOARD)/*.c)
sources += $(wildcard src/ns-core/$(BOARD)/*.cc)
sources += $(wildcard src/ns-core/$(BOARD)/*.cpp)
sources += $(wildcard src/ns-core/$(BOARD)/*.s)
sources += $(wildcard src/ns-core/$(BOARD)/$(COMPDIR)/*.c)
sources += $(wildcard src/ns-core/$(BOARD)/$(COMPDIR)/*.cc)
sources += $(wildcard src/ns-core/$(BOARD)/$(COMPDIR)/*.cpp)
sources += $(wildcard src/ns-core/$(BOARD)/$(COMPDIR)/*.s)


targets  := $(BINDIR)/$(local_app_name).axf
targets  += $(BINDIR)/$(local_app_name).bin

objects      = $(call source-to-object,$(sources))
dependencies = $(subst .o,.d,$(objects))

CFLAGS     += $(addprefix -D,$(DEFINES))
CFLAGS     += $(addprefix -I includes/,$(INCLUDES))

ifeq ($(ARCH),apollo5)
ifeq ($(TOOLCHAIN),arm)
LINKER_FILE := src/ns-core/$(BOARD)/$(COMPDIR)/linker_script_$(BOOTLOADER).sct
else ifeq ($(TOOLCHAIN),arm-none-eabi)
LINKER_FILE := src/ns-core/$(BOARD)/$(COMPDIR)/linker_script_$(BOOTLOADER).ld
endif
else
ifeq ($(TOOLCHAIN),arm)
LINKER_FILE := src/ns-core/$(BOARD)/$(COMPDIR)/linker_script.sct
else ifeq ($(TOOLCHAIN),arm-none-eabi)
LINKER_FILE := src/ns-core/$(BOARD)/$(COMPDIR)/linker_script.ld
endif
endif

all: $(BINDIR) $(objects) $(targets)

.PHONY: clean
clean:
ifeq ($(OS),Windows_NT)
	@echo "Windows_NT"
	@echo $(Q) $(RM) -rf $(BINDIR)/*
	$(Q) $(RM) -rf $(BINDIR)/*
else
	$(Q) $(RM) -rf $(BINDIR) $(JLINK_CF)
endif

ifneq "$(MAKECMDGOALS)" "clean"
  include $(dependencies)
endif

$(BINDIR):
	$(Q) $(MKD) -p $@

$(BINDIR)/%.o: %.cc
	@echo " Compiling $(COMPILERNAME) $< to make $@"
	$(Q) $(MKD) -p $(@D)
	$(Q) $(CC) -c $(CFLAGS) $(CCFLAGS) $< -o $@

$(BINDIR)/%.o: %.cpp
	@echo " Compiling $(COMPILERNAME) $< to make $@"
	$(Q) $(MKD) -p $(@D)
	$(Q) $(CC) -c $(CFLAGS) $(CCFLAGS) $< -o $@

$(BINDIR)/%.o: %.c
	@echo " Compiling $(COMPILERNAME) $< to make $@"
	$(Q) $(MKD) -p $(@D)
	$(Q) $(CC) -c $(CFLAGS) $(CONLY_FLAGS) $< -o $@

$(BINDIR)/%.o: %.s
	@echo " Assembling $(COMPILERNAME) $<"
	$(Q) $(MKD) -p $(@D)
	$(Q) $(CC) -c $(ASMFLAGS) $< -o $@

# $(eval $(call make-axf, $(BINDIR)/$(local_app_name), $(sources)))

$(BINDIR)/$(local_app_name).axf: $(objects) $(libraries) $(lib_prebuilt) $(override_libraries)
	@echo " Linking $(COMPILERNAME) $@"
	$(Q) $(MKD) -p $(@D)
ifeq ($(TOOLCHAIN),arm)
	$(Q) $(LD) $^ $(LFLAGS) --list=$*.map -o $@
else
	$(Q) $(CC) -Wl,-T,$(LINKER_FILE) -o $@ $^ $(LFLAGS)
endif
# $(Q) $(CC) -Wl,-T,$(LINKER_FILE) -o $@ $(objects) $(LFLAGS)

# $(BINDIR)/$(local_app_name).bin: $(BINDIR)/$(local_app_name).axf
# 	@echo " Copying $(COMPILERNAME) $@..."
# 	$(Q) $(MKD) -p $(@D)
# 	$(Q) $(CP) $(CPFLAGS) $< $@
# 	$(Q) $(OD) $(ODFLAGS) $< > $(BINDIR)/$(local_app_name).lst
# 	$(Q) $(SIZE) $(objects) $(lib_prebuilt) $< > $(BINDIR)/$(local_app_name).size
ifeq ($(TOOLCHAIN),arm)
$(BINDIR)/$(local_app_name).bin: $(BINDIR)/$(local_app_name).axf
	@echo " Copying $(COMPILERNAME) $@..."
	$(Q) $(MKD) -p $(@D)
	$(Q) $(CP) $(CPFLAGS) $@ $<
	$(Q) $(OD) $(ODFLAGS) $< --output $*.txt
else
$(BINDIR)/$(local_app_name).bin: $(BINDIR)/$(local_app_name).axf
	@echo " Copying $(COMPILERNAME) $@..."
	$(Q) $(MKD) -p $(@D)
	$(Q) $(CP) $(CPFLAGS) $< $@
	$(Q) $(OD) $(ODFLAGS) $< > $*.lst
# $(foreach OBJ,$(objects),$(shell echo "${OBJ}">>$*.sizeinput;))
# $(Q) $(SIZE) @$*.sizeinput $< > $*.size
endif

# %.size: %.axf
# 	@echo " Generating size information for $(COMPILERNAME) $@..."
# 	$(Q) $(MKD) -p $(@D)
# 	$(foreach OBJ,$(objects),$(shell echo "${OBJ}">>$*.sizeinput;))

# 	$(Q) $(SIZE) $< > $@


$(JLINK_CF):
	@echo " Creating JLink command sequence input file..."
	$(Q) echo "ExitOnError 1" > $@
	$(Q) echo "Reset" >> $@
	$(Q) echo "LoadFile $(BINDIR)/$(TARGET).bin, $(JLINK_PF_ADDR)" >> $@
	$(Q) echo "Exit" >> $@

.PHONY: deploy
deploy: $(JLINK_CF)
	@echo " Deploying $< to device (ensure JLink USB connected and powered on)..."
	$(Q) $(JLINK) $(JLINK_CMD)
	$(Q) $(RM) $(JLINK_CF)

.PHONY: view
view:
	@echo " Printing SWO output (ensure JLink USB connected and powered on)..."
	$(Q) $(JLINK_SWO) $(JLINK_SWO_CMD)
	$(Q) $(RM) $(JLINK_CF)

%.d: ;
