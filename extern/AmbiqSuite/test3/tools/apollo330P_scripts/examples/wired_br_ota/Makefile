INPUT:= $(wildcard *.bin)
INTERMEDIATE := $(INPUT:.bin=_ota.bin)
OUTPUT:= $(INPUT:.bin=_ota_wired.bin)

all: $(OUTPUT)

%_ota.bin: %.bin firmware.ini keys.ini
	python3 ../../create_cust_image_blob.py -c firmware.ini --bin $< -o $@

%_ota_wired.bin: %_ota.bin
	python3 ../../create_cust_image_blob.py -c wired_br_ota.ini --bin $< -o $@

clean:
	rm -rf a $(INTERMEDIATE) $(OUTPUT)
