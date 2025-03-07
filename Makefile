include make/helpers.mk
include make/local_overrides.mk
include make/neuralspot_config.mk
include make/neuralspot_toolchain.mk
include make/jlink.mk

# NeuralSPOT is structured to ease adding modules
# Adding a module is as simple as creating a directory,
# populating it with a modules.mk, and adding it to the
# 'modules' list below.

# There are certain hardcoded assumptions that must be followed
# When adding a module. Module directories must look something like this:
# .../mymodule/
#   module.mk
#   src/ <- contains C, CC, S, and private H files
#   includes-api/ <- contains public header files
#
# Build artifacts (.o, .d, etc) are placed
# in the $BINDIR directory (defaults to 'build') which is
# created when needed and remove by 'make clean'
#
# The module.mk contents differ slightly depending on
# the type of module (example, extern, or library), but
# generally work by adding values to a set of variables
# (see below). When adding a new module, it is best
# to copy an existing module.mk and modifying it.

# NeuralSPOT Library Modules
modules      := neuralspot/ns-core
modules      += neuralspot/ns-harness
modules      += neuralspot/ns-peripherals
modules      += neuralspot/ns-ipc
modules      += neuralspot/ns-audio
modules      += neuralspot/ns-utils
modules 	 += neuralspot/ns-uart
modules      += neuralspot/ns-rpc

modules      += neuralspot/ns-i2c
ifneq ($(ARCH),apollo3)
modules      += neuralspot/ns-spi
modules      += neuralspot/ns-camera
endif
modules      += neuralspot/ns-nnsp
modules      += neuralspot/ns-features

ifeq ($(USB_PRESENT),1)
	modules      += neuralspot/ns-usb
endif

ifeq ($(BLE_SUPPORTED),1)
modules      += neuralspot/ns-ble
endif

# External Component Modules
modules      += extern/AmbiqSuite/$(AS_VERSION)
modules 	 += extern/CMSIS/CMSIS-DSP-1.15.0
# modules 	 += extern/CMSIS/CMSIS-NN
modules      += extern/tensorflow/$(TF_VERSION)
# modules      += extern/SEGGER_RTT/$(SR_VERSION)
modules 	 += extern/codecs/opus-precomp

ifeq ($(BLE_SUPPORTED),1)
modules      += extern/AmbiqSuite/$(AS_VERSION)/third_party/cordio
endif

modules      += extern/erpc/$(ERPC_VERSION)

# Example (executable binary) Modules

ifeq ($(AUTODEPLOY),1)
# For tools/ns_autodeploy, we have a single module in projects/models
# This is used for tflm_validator, genLib, and power binaries
	modules      += $(ADPATH)/$(EXAMPLE)
else
	ifeq ($(EXAMPLE),all)
		modules      += examples/basic_tf_stub
		modules      += examples/har
# modules      += examples/nnse2
		modules	     += examples/uart
		modules      += examples/rpc_server

		ifeq ($(BOARD),apollo4p)
			modules += examples/rpc_client
		else ifeq ($(BOARD),apollo5b)
			modules += examples/rpc_client
		endif	
		ifeq ($(BLE_SUPPORTED),1)
#			modules      += examples/web_ble
# 			modules      += examples/audio_codec
			modules	     += examples/nnse
			modules	     += examples/nnid
		endif

		ifeq ($(USB_PRESENT),1)
			modules      += examples/vision		
			modules 	 += examples/ic
			modules      += examples/quaternion
			ifneq ($(ARCH),apollo3)
				modules      += examples/mpu_data_collection
			endif
# 			ifneq ($(BLE_SUPPORTED),1)
# # Don't include it twice
# 				modules  += examples/audio_codec
# 			endif
		endif
	else
		modules 	 += examples/$(EXAMPLE)
	endif
endif

# The following variables are filled in by module.mk include files
#
# To create binaries. add axf and bin targets to 'examples'
# For code with a main(), add the object file for it to 'mains'
# For libraries that need to be built, add them to 'libraries'
# 	(note that this is automatically done by make-library)
# For pre-built libraries (usually part of extern) add to 'lib_prebuilt'
# Add all your source files to 'sources'
# Add any locally needed pre-processing defines to 'pp_defines'
# Add your local build dir to 'bindirs'
# Add any needed include paths to 'includes_api'

examples     :=
mains        :=
libraries    :=
lib_prebuilt :=
sources      :=
includes_api :=
pp_defines   := $(DEFINES)
bindirs      := $(BINDIR)

obs = $(call source-to-object,$(sources))
dependencies = $(subst .o,.d,$(obs))
objects      = $(filter-out $(mains),$(call source-to-object,$(sources)))

CFLAGS     += $(addprefix -D,$(pp_defines))
CFLAGS     += $(addprefix -I ,$(includes_api))

.PHONY: all
all:

include $(addsuffix /module.mk,$(modules))
# $(info sources is $(objects))

all: $(bindirs) $(libraries) $(examples) $(override_libraries) $(exampexample_librariesle) $(BINDIR)/board.svd

.SECONDARY:
.PHONY: libraries
libraries: $(libraries)

.PHONY: clean
clean:
ifeq ($(OS),Windows_NT)
	$(Q) $(RM) -rf $(BINDIRROOT) $(JLINK_CF) $(NESTDIR)
else
	$(Q) $(RM) -rf $(BINDIRROOT) $(JLINK_CF) $(NESTDIR)
endif

# lint: $(LINTSOURCES)
# 	$(LINT) $< -- $(LINTINCLUDES)

ifneq "$(MAKECMDGOALS)" "clean"
 include $(dependencies)
endif

include make/nest-helper.mk

# Compute stuff for doc creation
doc_sources = $(addprefix ../../,$(sources))
doc_sources += $(addprefix ../../,$(includes_api))

# Find the full path of $(TARGET) for deploy:
deploy_target = $(filter %$(TARGET).bin, $(examples))
# $(info deploy_target: $(deploy_target))
# $(info TARGET: $(TARGET))
$(info examples: $(examples))

$(bindirs):
	$(Q) $(MKD) -p $@

$(BINDIR)/board.svd:
	$(Q) @cp ./extern/AmbiqSuite/$(AS_VERSION)/pack/svd/$(PART).svd $(BINDIR)/board.svd

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

ifeq ($(TOOLCHAIN),arm)
%.bin: %.axf
	@echo " Copying $(COMPILERNAME) $@..."
	$(Q) $(MKD) -p $(@D)
	$(Q) $(CP) $(CPFLAGS) $@ $<
	$(Q) $(OD) $(ODFLAGS) $< --output $*.txt
else
%.bin: %.axf
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

.PHONY: nest
nest: all
	@echo " Building Nest without src/ at $(NESTDIR) based on $< ..."
	$(Q) $(MKD) -p $(NESTDIR)
	$(Q) $(MKD) -p $(NESTDIR)/src
	$(Q) $(MKD) -p $(NESTDIR)/src/ns-core
	$(Q) $(MKD) -p $(NESTDIR)/srcpreserved
	$(Q) $(MKD) -p $(NESTDIR)/libs
	$(Q) $(MKD) -p $(NESTDIR)/make
	$(Q) $(MKD) -p $(NESTDIR)/pack/svd
	@for target in $(nest_dirs); do \
		"mkdir" -p $(NESTDIR)"/includes/"$$target ; \
		cp -R $$target/. $(NESTDIR)"/includes/"$$target 2>/dev/null || true; \
	done
	@for target in $(nest_lib_dirs); do \
		"mkdir" -p $(NESTDIR)"/"$$target ; \
	done

	@for file in $(nest_libs); do \
		cp $(BINDIRROOT)/$$file $(NESTDIR)/libs/$$file ; \
	done
	@for file in $(nest_prebuilt_libs); do \
		cp $$file $(NESTDIR)/libs/$$file ; \
	done

	@cp -R $(NESTDIR)/src $(NESTDIR)/srcpreserved/ 2>/dev/null || true
	@cp -R neuralspot/ns-core/src/* $(NESTDIR)/src/ns-core

# @cp $(LINKER_FILE) $(NESTDIR)/libs
	@cp make/nest-makefile.mk $(NESTDIR)/Makefile.suggested
	@cp $(BINDIR)/board.svd $(NESTDIR)/pack/svd
	@echo "# Autogenerated file - created by NeuralSPOT make nest" > $(NESTDIR)/autogen_$(BOARD)_$(EVB)_$(TOOLCHAIN).mk
	@echo "INCLUDES += $(includes_api)" >> $(NESTDIR)/autogen_$(BOARD)_$(EVB)_$(TOOLCHAIN).mk
	@echo "libraries += $(nest_dest_libs)" >> $(NESTDIR)/autogen_$(BOARD)_$(EVB)_$(TOOLCHAIN).mk
	@echo "override_libraries += $(nest_dest_override_libs)" >> $(NESTDIR)/autogen_$(BOARD)_$(EVB)_$(TOOLCHAIN).mk
	@echo "local_app_name := $(NESTEGG)" >> $(NESTDIR)/autogen_$(BOARD)_$(EVB)_$(TOOLCHAIN).mk

.PHONY: nestcomponent
nestcomponent:
	@echo " Building Nestcomponent only copying $(NESTCOMP)..."
	@for target in $(nest_component_includes); do \
		"mkdir" -p $(NESTDIR)"/includes/"$$target ; \
		cp -R $$target/. $(NESTDIR)"/includes/"$$target 2>/dev/null || true; \
	done
	@for file in $(nest_component_libs); do \
		cp $$file $(NESTDIR)"/libs/" ; \
	done

.PHONY: nestall
nestall: nest
	@echo " Building Nestall including src/ at $(NESTDIR) based on $< ..."
	@cp -R $(NESTSOURCEDIR) $(NESTDIR)
	@cp make/helpers.mk $(NESTDIR)/make
	@cp make/neuralspot_config.mk $(NESTDIR)/make
	@cp make/neuralspot_toolchain.mk $(NESTDIR)/make
	@cp make/jlink.mk $(NESTDIR)/make
	@cp make/nest-makefile.mk $(NESTDIR)/Makefile

$(JLINK_CF):
	@echo " Creating JLink command sequence input file... $(deploy_target)"
	$(Q) echo "ExitOnError 1" > $@
# $(Q) echo "Reset" >> $@
# $(Q) echo "LoadFile $(deploy_target), $(JLINK_PF_ADDR)" >> $@
# $(Q) echo "Exit" >> $@
	$(Q) echo "connect" >> $@
	$(Q) echo "loadbin $(deploy_target), $(JLINK_PF_ADDR)" >> $@
	$(Q) echo "sleep 250" >> $@
# $(Q) echo "r" >> $@
# $(Q) echo "sleep 500" >> $@
# $(Q) echo "r" >> $@
# $(Q) echo "sleep 500" >> $@
	$(Q) echo "r" >> $@	
	$(Q) echo "q" >> $@

$(JLINK_RESET_CF):
	@echo " Creating JLink command reset file... $(deploy_target)"
	$(Q) echo "ExitOnError 1" > $@
	$(Q) echo "r1" >> $@
	$(Q) echo "sleep 250" >> $@
	$(Q) echo "r0" >> $@
	$(Q) echo "sleep 250" >> $@
	$(Q) echo "Exit" >> $@

.PHONY: reset
reset: $(JLINK_RESET_CF)
	@echo " Reset EVB via Jlink..."
	$(Q) $(JLINK) $(JLINK_RESET_CMD)
	$(Q) $(RM) $(JLINK_RESET_CF)

.PHONY: deploy
deploy: $(JLINK_CF)
	@echo " Deploying $< to device (ensure JLink USB connected and powered on)..."
	$(Q) $(JLINK) $(JLINK_CMD)
	$(Q) $(RM) $(JLINK_CF)

.PHONY: view
view:
	@echo " Printing SWO output (ensure JLink USB connected and powered on)..."
	$(Q) $(JLINK_SWO) $(JLINK_SWO_CMD)

%.d: ;
