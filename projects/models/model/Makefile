rootdir := ../../..

include $(rootdir)/make/helpers.mk
include $(rootdir)/make/neuralspot_config.mk
include $(rootdir)/make/local_overrides.mk
include $(rootdir)/make/neuralspot_toolchain.mk
include $(rootdir)/make/jlink.mk

build := build
includes_api :=
pp_defines   := $(DEFINES)

libraries := $(rootdir)/build/neuralspot/ns-model/ns-model.a
includes_api += $(rootdir)/neuralspot/ns-model/includes-api
includes_api += lib

include $(rootdir)/extern/tensorflow/$(TF_VERSION)/module.mk
objects := $(build)/model_model.o
objects += $(build)/model_example.o
objects += $(build)/startup_gcc.o

CFLAGS     += $(addprefix -D,$(pp_defines))
CFLAGS     += $(addprefix -I ,$(includes_api))

all: $(build) $(build)/model_lib.a $(build)/model_example.axf

$(build):
	$(Q) $(MKD) -p $@

$(build)/model_lib.a: $(build)/model_model.o $(libraries)
	@echo " Archiving $< to make $@"
	$(Q) $(AR) rsc $@ $^

$(BINDIR)/%.o: src/%.c
	@echo " Compiling $(COMPILERNAME) $< to make $@"
	$(Q) $(MKD) -p $(@D)
	$(Q) $(CC) -c $(CFLAGS) $(CONLY_FLAGS) $< -o $@

$(build)/%.o: src/%.cc
	@echo " Compiling $(COMPILERNAME) $< to make $@"
	$(Q) $(MKD) -p $(@D)
	$(Q) $(CC) -c $(CFLAGS) $(CCFLAGS) $< -o $@

$(build)/%.axf: $(objects) $(libraries)
	@echo " Linking $(COMPILERNAME) $@"
	$(Q) $(CC) -Wl,-T,src/linker_script.ld -o $@ $(objects) $(LFLAGS)

$(build)/%.bin: %.axf
	@echo " Copying $(COMPILERNAME) $@..."
	$(Q) $(MKD) -p $(@D)
	$(Q) $(CP) $(CPFLAGS) $< $@
	$(Q) @cp $(rootdir)extern/AmbiqSuite/$(AS_VERSION)/pack/svd/$(PART).svd $(build)/board.svd
	$(Q) $(OD) $(ODFLAGS) $< > $*.lst
	$(Q) $(SIZE) $(objects) $(lib_prebuilt) $< > $*.size

.PHONY: clean
clean:
ifeq ($(OS),Windows_NT)
	@echo "Windows_NT"
	$(Q) $(RM) -rf $(build)
else
	$(Q) $(RM) -rf $(build)
endif