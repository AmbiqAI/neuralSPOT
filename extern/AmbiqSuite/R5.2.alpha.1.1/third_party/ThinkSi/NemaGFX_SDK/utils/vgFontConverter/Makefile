NEMAGFX_SDK_PATH?=../..
INCLUDE_NEMAVG=$(NEMAGFX_SDK_PATH)/include/tsi/NemaVG

INCPATHS+=$(INCLUDE_NEMAVG)

INCFLAGS+=$(foreach TMP,$(INCPATHS),-I$(TMP))


all: nema_vg_font_convert

CC     = gcc
CFLAGS = -std=c99 -g -Wall `pkg-config --cflags freetype2`
LIBS   = -lm -ldl `pkg-config --libs freetype2`
INC    = -I
nema_vg_font_convert: main.c vg_font_convert.c
	$(CC) $(CFLAGS) $(INCFLAGS) main.c vg_font_convert.c $(LIBS) -o $@
#	strip $@

clean:
	rm -f nema_vg_font_convert
