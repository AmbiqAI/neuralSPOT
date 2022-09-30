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

# External Component Modules
modules      := extern/AmbiqSuite/$(AS_VERSION)
modules      += extern/tensorflow/$(TF_VERSION)
modules      += extern/SEGGER_RTT/$(SR_VERSION)
modules      += extern/erpc/$(ERPC_VERSION)

# NeuralSPOT Library Modules
modules      += neuralspot/ns-harness 
modules      += neuralspot/ns-peripherals 
modules      += neuralspot/ns-ipc
modules      += neuralspot/ns-audio
modules      += neuralspot/ns-usb
modules      += neuralspot/ns-utils
modules      += neuralspot/ns-rpc
modules      += neuralspot/ns-i2c

# Example (executable binary) Modules
modules      += examples/s2i
modules      += examples/basic_tf_stub
modules      += examples/har
modules      += examples/mpu_data_collection
modules      += examples/rpc_client_example
modules      += examples/rpc_server_example

NESTSOURCE    := $(BINDIR)/examples/basic_tf_stub/src/main.d
NESTSOURCEDIR := examples/basic_tf_stub/src

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
all_includes = $(shell awk '/^ .*\.h/ {print $$1}' $(NESTSOURCE))
nest_dirs = $(sort $(dir $(all_includes)))
nest_libs = $(addprefix libs/,$(notdir $(libraries)))
nest_libs += $(addprefix libs/,$(notdir $(lib_prebuilt)))

nest_component_includes = $(call FILTER_IN,$(NESTCOMP),$(all_includes))
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
	@mkdir -p $(@D)
	$(Q) $(CC) -c $(CFLAGS) $(CCFLAGS) $< -o $@

$(BINDIR)/%.o: %.cpp
	@echo " Compiling $(COMPILERNAME) $< to make $@"
	@mkdir -p $(@D)
	$(Q) $(CC) -c $(CFLAGS) $(CCFLAGS) $< -o $@

$(BINDIR)/%.o: %.c
	@echo " Compiling $(COMPILERNAME) $< to make $@"
	@mkdir -p $(@D)
	$(Q) $(CC) -c $(CFLAGS) $(CONLY_FLAGS) $< -o $@

$(BINDIR)/%.o: %.s
	@echo " Assembling $(COMPILERNAME) $<"
	@mkdir -p $(@D)
	$(Q) $(CC) -c $(CFLAGS) $< -o $@

%.axf: src/%.o $(objects) $(libraries)
	@echo " Linking $(COMPILERNAME) $@"
	@mkdir -p $(@D)
	$(Q) $(CC) -Wl,-T,$(LINKER_FILE) -o $@  $< $(objects) $(LFLAGS)

%.bin: %.axf 
	@echo " Copying $(COMPILERNAME) $@..."
	@mkdir -p $(@D)
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
nest: all $(NESTSOURCE)
	@echo " Building Nest without src/ at $(NESTDIR) based on $< ..."
	@mkdir -p $(NESTDIR)
	@mkdir -p $(NESTDIR)/src	
	@mkdir -p $(NESTDIR)/src/preserved
	@mkdir -p $(NESTDIR)/libs	
	@mkdir -p $(NESTDIR)/make	
	@mkdir -p $(NESTDIR)/pack/svd	
	@for target in $(nest_dirs); do \
		mkdir -p $(NESTDIR)"/includes/"$$target ; \
	done
	@for file in $(all_includes); do \
		cp $$file $(NESTDIR)"/includes/"$$file ; \
	done
	@for file in $(libraries); do \
		cp $$file $(NESTDIR)"/libs/" ; \
	done	
	@for file in $(lib_prebuilt); do \
		cp $$file $(NESTDIR)"/libs/" ; \
	done
	@cp -R $(NESTDIR)/src $(NESTDIR)/src/preserved/ 2>/dev/null || true

	@cp $(LINKER_FILE) $(NESTDIR)/libs
	@cp make/nest-makefile.mk $(NESTDIR)/Makefile.suggestedfornest
	@cp $(BINDIR)/board.svd $(NESTDIR)/pack/svd
	@echo "# Autogenerated file - created by NeuralSPOT make nest" > $(NESTDIR)/autogen.mk
	@echo "DEFINES += $(pp_defines)" >> $(NESTDIR)/autogen.mk
	@echo "INCLUDES += $(includes_api)" >> $(NESTDIR)/autogen.mk
	@echo "libraries += $(nest_libs)" >> $(NESTDIR)/autogen.mk

.PHONY: nestcomponent
nestcomponent:
	@echo " Building Nestcomponent only copying $(NESTCOMP)..."
	@for file in $(nest_component_includes); do \
		cp $$file $(NESTDIR)"/includes/"$$file ; \
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
