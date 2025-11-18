.PHONY: all

CPPCHECK?=cppcheck
PLATFORM?=zc70x_linux
PATH_TO_GFX_FILE=./platforms/$(PLATFORM)/NemaGFX/nemagfx.mk
PATH_TO_VG_FILE=./platforms/$(PLATFORM)/NemaVG/nemavg.mk
PATH_TO_3D_FILE=./platforms/$(PLATFORM)/Nema3D/nema3d.mk
PATH_TO_DC_FILE=./platforms/$(PLATFORM)/NemaDC/nemadc.mk
PATH_TO_KRN_FILE=./Private/Makefile.kernels

%:
ifneq ("$(wildcard $(PATH_TO_GFX_FILE))","")
	@$(MAKE) -f Makefile.nemagfx $@
else
	echo "NemaGFX not valid for this platform"
endif
ifneq ("$(wildcard $(PATH_TO_VG_FILE))","")
	@$(MAKE) -f Makefile.nemavg $@
else
	echo "NemaVG not valid for this platform"
endif
ifneq ("$(wildcard $(PATH_TO_3D_FILE))","")
	@$(MAKE) -f Makefile.nema3d $@
else
	echo "Nema3D not valid for this platform"
endif
ifneq ("$(wildcard $(PATH_TO_DC_FILE))","")
	@$(MAKE) -f Makefile.nemadc $@
else
	echo "NemaDC not valid for this platform"
endif

all: kernels nemagfx nemavg nemadc nema3d

nemagfx:
ifneq ("$(wildcard $(PATH_TO_GFX_FILE))","")
	@$(MAKE) -f Makefile.nemagfx
else
	echo "NemaGFX not valid for this platform"
endif

nemavg:
ifneq ("$(wildcard $(PATH_TO_VG_FILE))","")
	@$(MAKE) -f Makefile.nemavg
else
	echo "NemaVG not valid for this platform"
endif

nemadc:
ifneq ("$(wildcard $(PATH_TO_DC_FILE))","")
	@$(MAKE) -f Makefile.nemadc
else
	echo "NemaDC not valid for this platform"
endif

nema3d:
ifneq ("$(wildcard $(PATH_TO_3D_FILE))","")
	@$(MAKE) -f Makefile.nema3d
else
	echo "Nema3D not valid for this platform"
endif

kernels:
ifneq ("$(wildcard $(PATH_TO_KRN_FILE))","")
	@$(MAKE) -f Private/Makefile.kernels gen_kernels
else
	@echo "continue"
endif

gfx_kernels:
ifneq ("$(wildcard $(PATH_TO_KRN_FILE))","")
	@$(MAKE) -f Private/Makefile.kernels gen_gfx_kernels
else
	@echo "continue"
endif

vg_kernels:
ifneq ("$(wildcard $(PATH_TO_KRN_FILE))","")
	@$(MAKE) -f Private/Makefile.kernels gen_vg_kernels
else
	@echo "continue"
endif

3d_kernels:
ifneq ("$(wildcard $(PATH_TO_KRN_FILE))","")
	@$(MAKE) -f Private/Makefile.kernels gen_3d_kernels
else
	@echo "continue"
endif

cppcheck:
	$(CPPCHECK) -iPrivate --quiet --enable=all --error-exitcode=-1 --inconclusive --std=c99 --language=c --suppressions-list=suppressions.txt .
