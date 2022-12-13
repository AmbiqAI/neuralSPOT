include make/helpers.mk
include make/neuralspot_config.mk
include make/local_overrides.mk
include make/neuralspot_toolchain.mk
include make/jlink.mk

# NeuralSPOT is structured to ease adding modules
# Adding a module is as simple as creating a directory,
# populating it with a modules.mk, and adding it to the
# 'modules' list below.

# There are certain hardcoded assumptions that must be followed
# When adding a module. Module directories must look like this:
# .../mymodule/
#   module.mk
#   src/ <- contains C, CC, S, and private H files
#   includes-api/ <- contains public header files
#
# Build artifacts (.o, .d, etc) are placed adjacent to src/
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
modules      += neuralspot/ns-usb
modules      += neuralspot/ns-utils
modules      += neuralspot/ns-rpc
modules      += neuralspot/ns-i2c

# External Component Modules
modules      += extern/AmbiqSuite/$(AS_VERSION)
modules      += extern/CMSIS/$(CMSIS_VERSION)
modules      += extern/tensorflow/$(TF_VERSION)
modules      += extern/SEGGER_RTT/$(SR_VERSION)
modules      += extern/erpc/$(ERPC_VERSION)

# Example (executable binary) Modules
modules      += examples/basic_tf_stub
modules      += examples/mpu_data_collection
modules      += examples/rpc_client_example
modules      += examples/rpc_server_example

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

examples     :=
mains        :=
libraries    :=
lib_prebuilt :=
sources      :=
includes_api :=
pp_defines   := $(DEFINES)
bindirs      := $(BINDIR)

objects      = $(filter-out $(mains),$(call source-to-object,$(sources)))
dependencies = $(subst .o,.d,$(objects))

CFLAGS     += $(addprefix -D,$(pp_defines))
CFLAGS     += $(addprefix -I ,$(includes_api))

.PHONY: all
all:

include $(addsuffix /module.mk,$(modules))

# LINTINCLUDES = $(addprefix -I ,$(includes_api))
# LINTINCLUDES += $(addprefix -D,$(pp_defines))
# LINTIGNORE = extern
# LINTSOURCES =  $(call FILTER_OUT,extern, $(sources))

# #LINTSOURCES = $(filter-out $(wildcard extern/*)), $(sources))
# $(info --$(sources))
# $(info --$(LINTSOURCES))

all: $(bindirs) $(libraries) $(examples)

.SECONDARY:
.PHONY: libraries
libraries: $(libraries)

.PHONY: clean
clean:
ifeq ($(OS),Windows_NT)
	@echo "Windows_NT"
	@echo $(Q) $(RM) -rf $(bindirs)/*
	$(Q) $(RM) -rf $(bindirs)/*
else
	$(Q) $(RM) -rf $(bindirs) $(JLINK_CF) $(NESTDIR)
endif

# lint: $(LINTSOURCES)
# 	$(LINT) $< -- $(LINTINCLUDES)

ifneq "$(MAKECMDGOALS)" "clean"
  include $(dependencies)
endif

# Compute stuff for nest creation
nest_files = $(shell find extern/tensorflow/$(TF_VERSION)/. -type f -name "*.h" -o -name "*.hpp")
nest_files += $(shell find extern/AmbiqSuite/$(AS_VERSION)/. -type f -name "*.h" -o -name "*.hpp")
nest_files += $(shell find extern/SEGGER_RTT/$(SR_VERSION)/. -type f -name "*.h" -o -name "*.hpp")
nest_files += $(shell find extern/erpc/$(ERPC_VERSION)/. -type f -name "*.h" -o -name "*.hpp")
nest_files += $(shell find extern/CMSIS/$(CMSIS_VERSION)/. -type f -name "*.h" -o -name "*.hpp")
nest_dirs = $(sort $(dir $(nest_files)))
nest_dirs += $(call FILTER_IN,neuralspot,$(includes_api))

nest_libs = $(addprefix libs/,$(notdir $(libraries)))
nest_libs += $(addprefix libs/,$(notdir $(lib_prebuilt)))

nest_component_includes = $(call FILTER_IN,$(NESTCOMP),$(nest_dirs))
nest_component_libs = $(call FILTER_IN,$(NESTCOMP),$(libraries))
nest_component_libs += $(call FILTER_IN,$(NESTCOMP),$(lib_prebuilt))

# Compute stuff for doc creation
doc_sources = $(addprefix ../../,$(sources))
doc_sources += $(addprefix ../../,$(includes_api))

# Find the full path of $(TARGET) for deploy:
deploy_target = $(filter %$(TARGET).bin, $(examples))

$(bindirs):
	@mkdir -p $@

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
	$(Q) $(CC) -c $(CFLAGS) $< -o $@

%.axf: src/%.o $(objects) $(libraries)
	@echo " Linking $(COMPILERNAME) $@"
	@mkdir -p $(@D)
	$(Q) $(CC) -Wl,-T,$(LINKER_FILE) -o $@  $< $(objects) $(LFLAGS)

%.bin: %.axf
	@echo " Copying $(COMPILERNAME) $@..."
	$(Q) $(MKD) -p $(@D)
	$(Q) $(CP) $(CPFLAGS) $< $@
	$(Q) @cp ./extern/AmbiqSuite/$(AS_VERSION)/pack/svd/$(PART).svd $(BINDIR)/board.svd
	$(Q) $(OD) $(ODFLAGS) $< > $*.lst
	$(Q) $(SIZE) $(objects) $(lib_prebuilt) $< > $*.size

.PHONY: docs
docs:
	@echo " Making Documents"
	@echo "INCLUDES = $(doc_sources)"
	@echo "EXCLUDE_PATTERNS = */AmbiqSuite/* */tensorflow/* */$(BINDIR)/* */$(NESTDIR)/*"
#	@echo "H_INCLUDES = $(all_includes)"
#	$(Q) $(DOX) docs/Doxyfile

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
	@for file in $(libraries); do \
		cp $$file $(NESTDIR)"/libs/" ; \
	done
	@for file in $(lib_prebuilt); do \
		cp $$file $(NESTDIR)"/libs/" ; \
	done
	@cp -R $(NESTDIR)/src $(NESTDIR)/srcpreserved/ 2>/dev/null || true

	@cp $(LINKER_FILE) $(NESTDIR)/libs
	@cp make/nest-makefile.mk $(NESTDIR)/Makefile.suggested
	@cp $(BINDIR)/board.svd $(NESTDIR)/pack/svd
	@echo "# Autogenerated file - created by NeuralSPOT make nest" > $(NESTDIR)/autogen.mk
	@echo "INCLUDES += $(includes_api)" >> $(NESTDIR)/autogen.mk
	@echo "libraries += $(nest_libs)" >> $(NESTDIR)/autogen.mk
	@echo "local_app_name := $(NESTEGG)" >> $(NESTDIR)/autogen.mk

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
	@cp neuralspot/ns-core/src/* $(NESTDIR)/src/ns-core
	@cp make/helpers.mk $(NESTDIR)/make
	@cp make/neuralspot_config.mk $(NESTDIR)/make
	@cp make/neuralspot_toolchain.mk $(NESTDIR)/make
	@cp make/jlink.mk $(NESTDIR)/make
	@cp make/nest-makefile.mk $(NESTDIR)/Makefile

$(JLINK_CF):
	@echo " Creating JLink command sequence input file... $(deploy_target)"
	$(Q) echo "ExitOnError 1" > $@
	$(Q) echo "Reset" >> $@
	$(Q) echo "LoadFile $(deploy_target), $(JLINK_PF_ADDR)" >> $@
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
