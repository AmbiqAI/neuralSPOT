INPUT = $(wildcard *.bin)
CERT = $(INPUT:.bin=.cert)
SOTA = $(INPUT:.bin=_ota.bin)
WIRED = $(INPUT:.bin=_ota_wired.bin)

INPUT_DATA_PATH = ../../oem_tools_pkg/cert_utils/cert_gen_utils/inputData
CERT_UTIL_PATH = ../../oem_tools_pkg/cert_utils/cert_gen_utils

all: $(SOTA) $(CERT) $(WIRED)

%.cert: %.bin am_oem_cnt_cert.cfg images_table_template.tbl
	sed "s/INPUT_FILE/\.\/inputData\/$</" images_table_template.tbl > images_table.tbl
	cp images_table.tbl $(INPUT_DATA_PATH)
	cp $(INPUT) $(INPUT_DATA_PATH)
	cd $(CERT_UTIL_PATH) && python3 am_cert_content_util.py ./am_config/am_oem_cnt_cert.cfg
	cp $(CERT_UTIL_PATH)/am_cert_content_util_output/content_cert.bin $@

%_ota.bin: %.bin %.cert secure-firmware.ini keys.ini
	python3 ../../create_cust_image_blob.py -c secure-firmware.ini --bin $< --certificate $(<:.bin=.cert) -o $@

%_ota_wired.bin: %_ota.bin wired_ota.ini
	python3 ../../create_cust_image_blob.py -c wired_ota.ini --bin $< -o $@

clean:
	rm -rf $(WIRED) $(SOTA) $(CERT) images_table.tbl *.log
