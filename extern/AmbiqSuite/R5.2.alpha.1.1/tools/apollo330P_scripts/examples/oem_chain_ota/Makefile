OUTPUT:= oem_chain_ota.bin

all: $(OUTPUT)

%.bin: %.bin oem_chain.ini keys.ini
	python3 ../../create_cust_image_blob.py -c oem_chain.ini --bin $< -o $@
clean:
	rm -rf a $(OUTPUT)