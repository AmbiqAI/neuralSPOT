# Compute stuff for EdgeImpulse nest creation
# nest_files = $(call rwildcard,extern/tensorflow/$(TF_VERSION)/.,*.h)
# nest_files += $(call rwildcard,extern/tensorflow/$(TF_VERSION)/.,*.hpp)
ei_nest_files += $(call rwildcard,extern/AmbiqSuite/$(AS_VERSION)/.,*.h)
ei_nest_files += $(call rwildcard,extern/AmbiqSuite/$(AS_VERSION)/.,*.hpp)
ei_nest_files += $(call rwildcard,extern/SEGGER_RTT/$(SR_VERSION)/.,*.h)
ei_nest_files += $(call rwildcard,extern/SEGGER_RTT/$(SR_VERSION)/.,*.hpp)
ei_nest_files += $(call rwildcard,extern/erpc/$(ERPC_VERSION)/.,*.h)
ei_nest_files += $(call rwildcard,extern/erpc/$(ERPC_VERSION)/.,*.hpp)
# nest_files += $(call rwildcard,extern/CMSIS/$(CMSIS_VERSION)/.,*.h)
# nest_files += $(call rwildcard,extern/CMSIS/$(CMSIS_VERSION)/.,*.hpp)
ei_nest_dirs = $(sort $(dir $(ei_nest_files)))
ei_nest_dirs += $(call FILTER_IN,neuralspot,$(includes_api))

# Filter out CMSIS and TensorFlow from includes_api
# $(info includes_api: $(includes_api))
ei_includes_api := $(call FILTER_OUT,extern/CMSIS,$(includes_api))
# $(info ei_includes_api: $(ei_includes_api))
ei_includes_api := $(call FILTER_OUT,extern/tensorflow,$(ei_includes_api))
# $(info ei_includes_api2: $(ei_includes_api))

# $(info ei_includes_api: $(ei_includes_api))
# $(info ei_nest_dirs: $(ei_nest_dirs))


# This is the list of all lib directories to be created in nest (is nest_lib_dirs used?)
ei_nest_lib_dirs := $(sort $(dir $(libraries)))
ei_nest_lib_dirs += $(sort $(dir $(override_libraries)))
ei_nest_lib_dirs += $(addprefix libs/,$(sort $(dir $(lib_prebuilt))))

# Nests have their own ns_core, so filter that lib out
ei_nest_lib_dirs := $(call FILTER_OUT,ns-core,$(ei_nest_lib_dirs))

# Filter out CMSIS and TensorFlow
ei_nest_lib_dirs := $(call FILTER_OUT,extern/CMSIS,$(ei_nest_lib_dirs))
ei_nest_lib_dirs := $(call FILTER_OUT,extern/tensorflow,$(ei_nest_lib_dirs))

# Add lib dir
ei_nest_lib_dirs := $(subst $(BINDIRROOT)/,libs/,$(ei_nest_lib_dirs))

# This is a list of all the libraries, including the full path minus the build root
ei_nest_libs := $(subst $(BINDIRROOT)/,,$(libraries))
ei_nest_libs += $(subst $(BINDIRROOT)/,,$(override_libraries))

# Filter out ns-core
ei_nest_libs := $(call FILTER_OUT,ns-core,$(ei_nest_libs))

ei_nest_prebuilt_libs += $(lib_prebuilt)

# Filter out CMSIS and TensorFlow
ei_nest_prebuilt_libs := $(call FILTER_OUT,CMSIS,$(ei_nest_prebuilt_libs))
ei_nest_prebuilt_libs := $(call FILTER_OUT,libtensorflow,$(ei_nest_prebuilt_libs))
# $(info ei_nest_prebuilt_libs: $(ei_nest_prebuilt_libs))

# These lists contain the full path in the nest directory
ei_nest_dest_libs := $(addprefix libs/,$(ei_nest_libs))
ei_nest_dest_libs += $(addprefix libs/,$(ei_nest_prebuilt_libs))
ei_nest_dest_override_libs := $(subst $(BINDIRROOT)/,libs/,$(override_libraries))
# Filter out ns-core
ei_nest_dest_override_libs := $(call FILTER_OUT,ns-core,$(ei_nest_dest_override_libs))

# List of libs matching NESTCOMP
ei_nest_component_includes = $(call FILTER_IN,$(NESTCOMP),$(nest_dirs))
ei_nest_component_libs = $(call FILTER_IN,$(NESTCOMP),$(libraries))
ei_nest_component_libs += $(call FILTER_IN,$(NESTCOMP),$(lib_prebuilt))
