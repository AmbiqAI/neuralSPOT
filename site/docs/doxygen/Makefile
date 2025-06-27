#******************************************************************************
#
# Makefile - Rules for building the libraries, examples and docs.
#
# ${copyright}
#
# This is part of revision ${version} of the AmbiqSuite Development Package.
#
#******************************************************************************

SRC_TOP = demo1_main.c project.c

SRC_DIR = ./

#SRC_BSP_DIR = ../boards/brazo/am_bsp

SRCS_HAL  = $(wildcard $(SRC_DIR)/am_hal_stm/am_hal*.c)
INCLS_HAL = $(wildcard $(SRC_DIR)/am_hal_stm/am_hal*.h)

SRCS_BSP  = $(wildcard $(SRC_DIR)/bsp/am_bsp*.c)
INCLS_BSP = $(wildcard $(SRC_DIR)/bsp/am_bsp*.h)

INCLUDES = $(wildcard $(SRC_DIR)/include/*.h)

SRCS  = $(SRCS_HAL)  $(SRCS_BSP) ./*.c
INCLS = $(INCLS_HAL) $(INCLS_HAL) $(INCLUDES)

ALL_SRC = $(SRCS) $(INCLS)

OBJECTS_HAL  = $(patsubst $(SRC_DIR)/hal/%.c,objects/%.o,$(SRCS_HAL))
OBJECTS_BSP  = $(patsubst $(SRC_DIR)/bsp/%.c,objects/%.o,$(SRCS_BSP))
OBJECTS  = $(OBJECTS_HAL)  $(OBJECTS_BSP)

MAIN_OBJECTS = objects/user_main.o

#all: trial run_doxy $(OBJECTS) $(MAIN_OBJECTS) ./final_mergedfile.pdf

#all: trial run_doxy generate_pdf ./final_mergedfile.pdf unit_tests


all: ./doxy_out/rtf/refman.rtf ./final_mergedfile.pdf

quick: ./doxy_out/rtf/refman.rtf

../docs/user_guide/html ../docs/user_guide:
	@mkdir -p $@

./doxy_out/rtf/refman.rtf: Doxyfile RTFextensionFile ../docs/user_guide/html
	doxygen Doxyfile   | tee doxy.log
	doxygen Doxyfile
	cp ./custom_html/ambiqmicro_logo.png ../docs/user_guide/html
	cp ./custom_html/user_guide.html ../docs/user_guide/

#generate_pdf:

./doxy_out/rtf/refman.pdf: ./doxy_out/rtf/refman.rtf
	$(MAKE) -C ./doxy_out/rtf

./final_mergedfile.pdf: ./doxy_out/rtf/refman.pdf
	@echo  creating final_final_mergedfile.pdf from ./doxy_out/rtf/refman.pdf
	pdftk ./doxy_out/rtf/refman.pdf cat output ../docs/user_guide/user_guide.pdf


#unit_tests:
#	$(MAKE) -C $(SRC_BSP_DIR)/unit_tests

clean:
	-@/bin/rm -rf ../docs/user_guide/html
	-@/bin/rm -rf ./doxy_out/rtf/*.pdf
	-@/bin/rm -rf ./doxy_out/rtf/*.rtf
	-@/bin/rm -rf ./warnings.log
	-@/bin/rm -rf ./doxy.log

realclean: clean
	-@/bin/rm -rf ../docs/user_guide/html
	-@/bin/rm -rf ../docs/user_guide/*.pdf
	-@/bin/rm -rf ./doxy_out/html
