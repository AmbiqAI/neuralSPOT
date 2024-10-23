# Initialize rootdir to be cwd
rootdir := $(shell pwd)

# Shell command to check if the current directory is not neuralSPOT
rootdir := $(shell \
    while [ "$$(basename $$PWD)" != "neuralSPOT" ]; do \
        cd ..; \
    done; \
    pwd \
)


# Print the value of rootdir
$(info rootdir: $(rootdir))


# Include the necessary files

include $(rootdir)/make/helpers.mk
include $(rootdir)/make/neuralspot_config.mk
include $(rootdir)/make/local_overrides.mk
include $(rootdir)/make/neuralspot_toolchain.mk
include $(rootdir)/make/jlink.mk

build := $(BINDIRROOT)/$(BOARDROOT)_$(EVB)/$(TOOLCHAIN)
includes_api :=
pp_defines   := $(DEFINES)

libraries := lib/model_lib.a
includes_api += $(rootdir)/neuralspot/ns-model/includes-api
includes_api += lib

include $(rootdir)/extern/tensorflow/$(TF_VERSION)/module.mk
objects := $(build)/model_model.o
objects += $(build)/model_example.o
ifeq ($(TOOLCHAIN),arm)
objects += $(build)/armclang/startup_armclang.o
else
objects += $(build)/gcc/startup_gcc.o
endif

CFLAGS     += $(addprefix -D,$(pp_defines))
CFLAGS     += $(addprefix -I ,$(includes_api))

all: $(build) $(build)/model_example.bin $(build)/model_example.axf

$(build):
	$(Q) $(MKD) -p $@

lib/model_lib.a: $(build)/model_model.o
	@echo " Archiving $< to make $@"
	$(Q) $(AR) rsc $@ $^

$(build)/%.o: src/%.c
	@echo " Compiling $(COMPILERNAME) $< to make $@"
	$(Q) $(MKD) -p $(@D)
	$(Q) $(CC) -c $(CFLAGS) $(CONLY_FLAGS) $< -o $@

$(build)/%.o: src/%.cc
	@echo " Compiling $(COMPILERNAME) $< to make $@"
	$(Q) $(MKD) -p $(@D)
	$(Q) $(CC) -c $(CFLAGS) $(CCFLAGS) $< -o $@

$(build)/%.o: %.s
	@echo " Assembling $(COMPILERNAME) $<"
	$(Q) $(MKD) -p $(@D)
	$(Q) $(CC) -c $(ASMFLAGS) $< -o $@

$(build)/%.axf: $(objects) $(libraries)
	@echo " Linking $(COMPILERNAME) $@"
	$(Q) $(CC) -Wl,-T,src/linker_script.ld -o $@ $(objects) $(LFLAGS)

$(build)/%.bin: $(build)/%.axf
	@echo " Copying $(COMPILERNAME) $@..."
	$(Q) $(MKD) -p $(@D)
	$(Q) $(CP) $(CPFLAGS) $< $@
	$(Q) @cp $(rootdir)/extern/AmbiqSuite/$(AS_VERSION)/pack/svd/$(PART).svd $(build)/board.svd
	$(Q) $(OD) $(ODFLAGS) $< > $*.lst
	$(Q) $(SIZE) $(objects) $(lib_prebuilt) $< > $*.size

.PHONY: clean
clean:
	$(Q) $(RM) -rf $(build)
	$(Q) $(RM) -f lib/model_lib.a
