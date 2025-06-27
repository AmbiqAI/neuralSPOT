# Compute stuff for nest creation
nest_files = $(call rwildcard,extern/tensorflow/$(TF_VERSION)/.,*.h)
nest_files += $(call rwildcard,extern/tensorflow/$(TF_VERSION)/.,*.hpp)
nest_files += $(call rwildcard,extern/AmbiqSuite/$(AS_VERSION)/.,*.h)
nest_files += $(call rwildcard,extern/AmbiqSuite/$(AS_VERSION)/.,*.hpp)
nest_files += $(call rwildcard,extern/SEGGER_RTT/$(SR_VERSION)/.,*.h)
nest_files += $(call rwildcard,extern/SEGGER_RTT/$(SR_VERSION)/.,*.hpp)
nest_files += $(call rwildcard,extern/erpc/$(ERPC_VERSION)/.,*.h)
nest_files += $(call rwildcard,extern/erpc/$(ERPC_VERSION)/.,*.hpp)
# nest_files += $(call rwildcard,extern/CMSIS/$(CMSIS_VERSION)/.,*.h)
# nest_files += $(call rwildcard,extern/CMSIS/$(CMSIS_VERSION)/.,*.hpp)
nest_files += $(call rwildcard,extern/CMSIS/.,*.h)
nest_files += $(call rwildcard,extern/CMSIS/.,*.hpp)
nest_dirs = $(sort $(dir $(nest_files)))
nest_dirs += $(call FILTER_IN,neuralspot,$(includes_api))

# nest_libs = $(addprefix libs/,$(notdir $(libraries)))
# nest_libs += $(addprefix libs/,$(notdir $(lib_prebuilt)))
# nest_override_libs = $(addprefix libs/,$(notdir $(override_libraries)))
# nest_libs = $(addprefix libs/,$(libraries))
# nest_libs += $(addprefix libs/,$(lib_prebuilt))
# nest_override_libs = $(addprefix libs/,$(notdir $(override_libraries)))

# This is the list of all lib directories to be created in nest
nest_lib_dirs := $(sort $(dir $(libraries)))
nest_lib_dirs += $(sort $(dir $(override_libraries)))
nest_lib_dirs += $(addprefix libs/,$(sort $(dir $(lib_prebuilt))))
nest_lib_dirs := $(subst $(BINDIRROOT)/,libs/,$(nest_lib_dirs))

# This is a list of all the libraries, including the full path minus the build root
nest_libs := $(subst $(BINDIRROOT)/,,$(libraries))
nest_libs += $(subst $(BINDIRROOT)/,,$(override_libraries))
nest_prebuilt_libs += $(lib_prebuilt)

# These lists contain the full path in the nest directory
nest_dest_libs := $(addprefix libs/,$(nest_libs))
nest_dest_libs += $(addprefix libs/,$(nest_prebuilt_libs))
nest_dest_override_libs := $(subst $(BINDIRROOT)/,libs/,$(override_libraries))

# List of libs matching NESTCOMP
nest_component_includes = $(call FILTER_IN,$(NESTCOMP),$(nest_dirs))
nest_component_libs = $(call FILTER_IN,$(NESTCOMP),$(libraries))
nest_component_libs += $(call FILTER_IN,$(NESTCOMP),$(lib_prebuilt))