all: nema_font_convert

CC     = gcc
CFLAGS = -std=c99 -g -Wall `pkg-config --cflags freetype2`
LIBS   = -lm -ldl `pkg-config --libs freetype2`

nema_font_convert: main.c font_convert.c
	$(CC) $(CFLAGS) main.c font_convert.c $(LIBS) -o $@
#	strip $@

clean:
	rm -f nema_font_convert
