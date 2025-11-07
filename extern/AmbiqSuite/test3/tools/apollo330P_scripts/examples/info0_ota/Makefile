INPUT:= $(wildcard *.bin)
OUTPUT:= $(INPUT:.bin=_ota.bin)

all: $(OUTPUT)

%_ota.bin: %.bin info0_ota.ini keys.ini
	python3 ../../create_cust_image_blob.py -c info0_ota.ini --bin $< -o $@

clean:
	rm -rf a $(OUTPUT)
