include make/helpers.mk
include make/neuralspot_config.mk
include make/neuralspot_toolchain.mk
#include make/jlink.mk

# Collect information from each module in these four variables.
# Initialize them here as simple variables.
modules      := extern/AmbiqSuite/$(AS_VERSION)
modules      += extern/tensorflow/$(TF_VERSION)

modules      += neuralspot/ns-harness 
modules      += neuralspot/ns-peripherals 
modules      += neuralspot/ns-ipc
modules      += neuralspot/ns-audio

modules      += examples/hello_world
modules      += examples/s2i
modules      += examples/basic_tf_stub

NESTSOURCE := examples/basic_tf_stub/build/main.d

# These are filled in by subdir include files, pulled in below
examples     :=
mains        :=
libraries    :=
lib_prebuilt :=
sources      :=
includes_api :=
pp_defines   := $(DEFINES)
bindirs      := $(BINDIR)

objs         = $(filter-out $(mains),$(call source-to-object,$(sources)))
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

all_includes = $(shell awk '/^ .*\.h/ {print $$1}' examples/s2i/build/s2i_example.d)
nest_dirs = $(dir $(all_includes))

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

.PHONY: nest
nest: $(NESTSOURCE)
	@echo " Building Nest at $(NESTDIR) based on $< ..."
	@mkdir -p $(NESTDIR)
	@mkdir -p $(NESTDIR)/deploy	
	@mkdir -p $(NESTDIR)/libs	
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

%.d: ;
