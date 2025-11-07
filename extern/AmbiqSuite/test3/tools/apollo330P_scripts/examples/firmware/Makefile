INPUT:= $(wildcard *.bin)
OUTPUT:= $(INPUT:.bin=_ota.bin)

all: $(OUTPUT)

%_ota.bin: %.bin firmware.ini keys.ini
	python3 ../../create_cust_image_blob.py -c firmware.ini --bin $< -o $@

clean:
	rm -rf a $(OUTPUT)
