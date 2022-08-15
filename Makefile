include make/helpers.mk
include make/neuralspot_config.mk
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

# NeuralSPOT Library Modules
modules      += neuralspot/ns-harness 
modules      += neuralspot/ns-peripherals 
modules      += neuralspot/ns-ipc
modules      += neuralspot/ns-audio

# Example (executable binary) Modules
modules      += examples/hello_world
modules      += examples/s2i
modules      += examples/basic_tf_stub

NESTSOURCE    := examples/basic_tf_stub/build/main.d
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

all: $(bindirs) $(libraries) $(examples)

.PHONY: libraries
libraries: $(libraries)

.PHONY: clean
clean:
ifeq ($(OS),Windows_NT)
	@echo "Windows_NT"
	@echo $(Q) $(RM) -rf $(CONFIG)/*
	$(Q) $(RM) -rf $(bindirs)/*
else
	$(Q) $(RM) -rf $(bindirs) $(JLINK_CF)
endif

ifneq "$(MAKECMDGOALS)" "clean"
  include $(dependencies)
endif

# Compute stuff for nest creation
all_includes = $(shell awk '/^ .*\.h/ {print $$1}' $(NESTSOURCE))
nest_dirs = $(sort $(dir $(all_includes)))
nest_libs = $(addprefix libs/,$(notdir $(libraries)))
nest_libs += $(addprefix libs/,$(notdir $(lib_prebuilt)))

# Compute stuff for doc creation
doc_sources = $(addprefix ../../,$(sources))
doc_sources += $(addprefix ../../,$(includes_api))

$(bindirs):
	@mkdir -p $@

%.o: ../src/%.cc
	@echo " Compiling $(COMPILERNAME) $< to make $@"
	@mkdir -p $(@D)
	$(Q) $(CC) -c $(CFLAGS) $(CCFLAGS) $< -o $@

%.o: ../src/%.c
	@echo " Compiling $(COMPILERNAME) $< to make $@"
	@mkdir -p $(@D)
	$(Q) $(CC) -c $(CFLAGS) $(CONLY_FLAGS) $< -o $@

%.o: ../src/%.s
	@echo " Assembling $(COMPILERNAME) $<"
	@mkdir -p $(@D)
	$(Q) $(CC) -c $(CFLAGS) $< -o $@

%.axf: %.o $(objects) $(libraries)
	@echo " Linking $(COMPILERNAME) $@"
	@mkdir -p $(@D)
	$(Q) $(CC) -Wl,-T,$(LINKER_FILE) -o $@ $< $(objects) $(LFLAGS)

%.bin: %.axf 
	@echo " Copying $(COMPILERNAME) $@..."
	@mkdir -p $(@D)
	$(Q) $(CP) $(CPFLAGS) $< $@
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
	@echo " Building Nest at $(NESTDIR) based on $< ..."
	@mkdir -p $(NESTDIR)
	@mkdir -p $(NESTDIR)/src	
	@mkdir -p $(NESTDIR)/libs	
	@mkdir -p $(NESTDIR)/make	
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
	@cp $(NESTSOURCEDIR)/* $(NESTDIR)/src
	@cp make/helpers.mk $(NESTDIR)/make
	@cp make/neuralspot_config.mk $(NESTDIR)/make
	@cp make/neuralspot_toolchain.mk $(NESTDIR)/make
	@cp make/jlink.mk $(NESTDIR)/make
	@cp $(LINKER_FILE) $(NESTDIR)/libs
	@cp make/nest-makefile.mk $(NESTDIR)/Makefile
	@echo "# Autogenerated file - created by NeuralSPOT make nest" > $(NESTDIR)/autogen.mk
	@echo "DEFINES += $(pp_defines)" >> $(NESTDIR)/autogen.mk
	@echo "INCLUDES += $(includes_api)" >> $(NESTDIR)/autogen.mk
	@echo "libraries += $(nest_libs)" >> $(NESTDIR)/autogen.mk

$(JLINK_CF):
	@echo " Creating JLink command sequence input file..."
	$(Q) echo "ExitOnError 1" > $@
	$(Q) echo "Reset" >> $@
	$(Q) echo "LoadFile examples/basic_tf_stub/$(BINDIR)/$(TARGET).bin, $(JLINK_PF_ADDR)" >> $@
	$(Q) echo "Exit" >> $@

.PHONY: deploy
# deploy: examples/basic_tf_stub/$(BINDIR)/$(TARGET).bin $(JLINK_CF)
deploy: $(JLINK_CF)
	@echo " Deploying $< to device (ensure JLink USB connected and powered on)..."
	$(Q) $(JLINK) $(JLINK_CMD)

.PHONY: view
view:
	@echo " Printing SWO output (ensure JLink USB connected and powered on)..."
	$(Q) $(JLINK_SWO) $(JLINK_SWO_CMD)

%.d: ;
