#******************************************************************************
#
# Makefile - Rules for building the libraries, examples and docs.
#
# Copyright (c) 2022, Ambiq Micro, Inc.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
# this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright
# notice, this list of conditions and the following disclaimer in the
# documentation and/or other materials provided with the distribution.
#
# 3. Neither the name of the copyright holder nor the names of its
# contributors may be used to endorse or promote products derived from this
# software without specific prior written permission.
#
# Third party software included in this distribution is subject to the
# additional license terms as defined in the /docs/licenses directory.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#
# This is part of revision release_sdk_4_3_0-0ca7d78a2b of the AmbiqSuite Development Package.
#
#******************************************************************************
TARGET := ble_freertos_locator
COMPILERNAME := iar
PROJECT := ble_freertos_locator_iar
CONFIG := bin
AM_SoftwareRoot ?= ../../../../..

SHELL:=/bin/bash
#### Required Executables ####
K := $(shell type -p IarBuild.exe)
RM = $(shell which rm 2>/dev/null)

ifeq ($(K),)
all clean:
	$(info Tools w/$(COMPILERNAME) not installed.)
	$(RM) -rf bin
else

LIBS = ${libraries}
INCS = ${incs}

all: directories $(CONFIG)/$(TARGET).bin

# Source Dependencies must be defined before they are used.
SRCS = ../../../../../../third_party/cordio/ble-host/sources/stack/l2c/l2c_coc.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/stack/l2c/l2c_main.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/stack/l2c/l2c_master.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/stack/l2c/l2c_slave.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/stack/att/attc_disc.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/stack/att/attc_main.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/stack/att/attc_proc.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/stack/att/attc_read.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/stack/att/attc_sign.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/stack/att/attc_write.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/stack/att/atts_ccc.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/stack/att/atts_csf.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/stack/att/atts_dyn.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/stack/att/atts_ind.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/stack/att/atts_main.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/stack/att/atts_proc.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/stack/att/atts_read.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/stack/att/atts_sign.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/stack/att/atts_write.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/stack/att/att_main.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/stack/att/att_uuid.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/stack/cfg/cfg_stack.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/sec/uecc/sec_ecc.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/stack/hci/hci_main.c
SRCS += ../../../../../../third_party/cordio/ble-profiles/sources/profiles/gatt/gatt_main.c
SRCS += ../../../../../../third_party/FreeRTOSv10.1.1/Source/portable/IAR/AMapollo4/port.c
SRCS += ../../../../../../third_party/FreeRTOSv10.1.1/Source/portable/IAR/AMapollo4/portasm.s
SRCS += ../../../../../../third_party/cordio/ble-host/sources/hci/ambiq/hci_cmd.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/hci/ambiq/hci_cmd_ae.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/hci/ambiq/hci_cmd_cte.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/hci/ambiq/hci_cmd_past.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/hci/ambiq/hci_cmd_phy.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/hci/ambiq/hci_core.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/hci/ambiq/hci_core_ps.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/hci/ambiq/hci_evt.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/hci/ambiq/hci_tr.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/hci/ambiq/hci_vs_ae.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/stack/dm/dm_adv.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/stack/dm/dm_adv_ae.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/stack/dm/dm_adv_leg.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/stack/dm/dm_conn.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/stack/dm/dm_conn_cte.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/stack/dm/dm_conn_master.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/stack/dm/dm_conn_master_ae.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/stack/dm/dm_conn_master_leg.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/stack/dm/dm_conn_slave.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/stack/dm/dm_conn_slave_ae.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/stack/dm/dm_conn_slave_leg.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/stack/dm/dm_conn_sm.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/stack/dm/dm_dev.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/stack/dm/dm_dev_priv.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/stack/dm/dm_main.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/stack/dm/dm_past.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/stack/dm/dm_phy.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/stack/dm/dm_priv.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/stack/dm/dm_scan.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/stack/dm/dm_scan_ae.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/stack/dm/dm_scan_leg.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/stack/dm/dm_sec.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/stack/dm/dm_sec_lesc.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/stack/dm/dm_sec_master.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/stack/dm/dm_sec_slave.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/stack/dm/dm_sync_ae.c
SRCS += ../../../../../../third_party/cordio/ble-profiles/sources/profiles/hrps/hrps_main.c
SRCS += ../../../../../../third_party/cordio/wsf/sources/util/bda.c
SRCS += ../../../../../../third_party/cordio/wsf/sources/util/bstream.c
SRCS += ../../../../../../third_party/cordio/wsf/sources/util/calc128.c
SRCS += ../../../../../../third_party/cordio/wsf/sources/util/crc32.c
SRCS += ../../../../../../third_party/cordio/wsf/sources/util/print.c
SRCS += ../../../../../../third_party/cordio/wsf/sources/util/terminal.c
SRCS += ../../../../../../third_party/cordio/wsf/sources/util/wstr.c
SRCS += ../../../../../../third_party/cordio/wsf/sources/port/freertos/wsf_assert.c
SRCS += ../../../../../../third_party/cordio/wsf/sources/port/freertos/wsf_buf.c
SRCS += ../../../../../../third_party/cordio/wsf/sources/port/freertos/wsf_efs.c
SRCS += ../../../../../../third_party/cordio/wsf/sources/port/freertos/wsf_math.c
SRCS += ../../../../../../third_party/cordio/wsf/sources/port/freertos/wsf_msg.c
SRCS += ../../../../../../third_party/cordio/wsf/sources/port/freertos/wsf_os.c
SRCS += ../../../../../../third_party/cordio/wsf/sources/port/freertos/wsf_queue.c
SRCS += ../../../../../../third_party/cordio/wsf/sources/port/freertos/wsf_timer.c
SRCS += ../../../../../../third_party/cordio/wsf/sources/port/freertos/wsf_trace.c
SRCS += ../../../../../../third_party/cordio/ble-profiles/sources/apps/app/common/app_db.c
SRCS += ../../../../../../third_party/cordio/ble-profiles/sources/apps/app/common/app_hw.c
SRCS += ../../../../../../third_party/cordio/ble-profiles/sources/apps/app/common/app_ui.c
SRCS += ../../../../../../third_party/cordio/ble-profiles/sources/apps/app/common/ui_console.c
SRCS += ../../../../../../third_party/cordio/ble-profiles/sources/apps/app/common/ui_lcd.c
SRCS += ../../../../../../third_party/cordio/ble-profiles/sources/apps/app/common/ui_main.c
SRCS += ../../../../../../third_party/cordio/ble-profiles/sources/apps/app/common/ui_platform.c
SRCS += ../../../../../../third_party/cordio/ble-profiles/sources/apps/app/common/ui_timer.c
SRCS += ../../../../../../third_party/cordio/ble-profiles/sources/apps/hidapp/hidapp_main.c
SRCS += ../src/am_resources.c
SRCS += ../src/ble_freertos_locator.c
SRCS += ../src/radio_task.c
SRCS += ../src/rtos.c
SRCS += ../../../../../../third_party/cordio/ble-profiles/sources/profiles/atpc/atpc_main.c
SRCS += ../../../../../../third_party/cordio/ble-profiles/sources/profiles/rscp/rscps_main.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/hci/ambiq/cooper/hci_dbg_trc.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/hci/ambiq/cooper/hci_drv_cooper.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/hci/ambiq/cooper/hci_vs_cooper.c
SRCS += ../../../../../../third_party/cordio/ble-profiles/sources/apps/locator/locator_main.c
SRCS += ../../../../../../third_party/uecc/uECC.c
SRCS += ../../../../../../third_party/uecc/uECC_ll.c
SRCS += ../../../../../../third_party/cordio/ble-profiles/sources/profiles/bas/bas_main.c
SRCS += ../../../../../../third_party/cordio/ble-profiles/sources/profiles/gap/gap_main.c
SRCS += ../../../../../../third_party/cordio/ble-profiles/sources/profiles/hid/hid_main.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/sec/common/sec_aes.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/sec/common/sec_aes_rev.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/sec/common/sec_ccm_hci.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/sec/common/sec_cmac_hci.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/sec/common/sec_ecc_debug.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/sec/common/sec_ecc_hci.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/sec/common/sec_main.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/stack/smp/smpi_act.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/stack/smp/smpi_sc_act.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/stack/smp/smpi_sc_sm.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/stack/smp/smpi_sm.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/stack/smp/smpr_act.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/stack/smp/smpr_sc_act.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/stack/smp/smpr_sc_sm.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/stack/smp/smpr_sm.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/stack/smp/smp_act.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/stack/smp/smp_db.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/stack/smp/smp_main.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/stack/smp/smp_non.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/stack/smp/smp_sc_act.c
SRCS += ../../../../../../third_party/cordio/ble-host/sources/stack/smp/smp_sc_main.c
SRCS += ../../../../../../utils/am_util_ble_cooper.c
SRCS += ../../../../../../utils/am_util_debug.c
SRCS += ../../../../../../utils/am_util_delay.c
SRCS += ../../../../../../utils/am_util_faultisr.c
SRCS += ../../../../../../utils/am_util_id.c
SRCS += ../../../../../../utils/am_util_stdio.c
SRCS += ../../../../../../devices/am_devices_button.c
SRCS += ../../../../../../devices/am_devices_cooper.c
SRCS += ../../../../../../devices/am_devices_led.c
SRCS += ../../../../../../third_party/cordio/ble-profiles/sources/apps/app/app_disc.c
SRCS += ../../../../../../third_party/cordio/ble-profiles/sources/apps/app/app_main.c
SRCS += ../../../../../../third_party/cordio/ble-profiles/sources/apps/app/app_master.c
SRCS += ../../../../../../third_party/cordio/ble-profiles/sources/apps/app/app_master_ae.c
SRCS += ../../../../../../third_party/cordio/ble-profiles/sources/apps/app/app_master_leg.c
SRCS += ../../../../../../third_party/cordio/ble-profiles/sources/apps/app/app_server.c
SRCS += ../../../../../../third_party/cordio/ble-profiles/sources/apps/app/app_slave.c
SRCS += ../../../../../../third_party/cordio/ble-profiles/sources/apps/app/app_slave_ae.c
SRCS += ../../../../../../third_party/cordio/ble-profiles/sources/apps/app/app_slave_leg.c
SRCS += ../../../../../../third_party/cordio/ble-profiles/sources/apps/app/app_terminal.c
SRCS += ../../../../../../third_party/FreeRTOSv10.1.1/Source/event_groups.c
SRCS += ../../../../../../third_party/FreeRTOSv10.1.1/Source/list.c
SRCS += ../../../../../../third_party/FreeRTOSv10.1.1/Source/queue.c
SRCS += ../../../../../../third_party/FreeRTOSv10.1.1/Source/tasks.c
SRCS += ../../../../../../third_party/FreeRTOSv10.1.1/Source/timers.c
SRCS += ../../../../../../third_party/FreeRTOSv10.1.1/Source/portable/MemMang/heap_4.c
SRCS += ../../../../../../third_party/cordio/ble-profiles/sources/services/svc_alert.c
SRCS += ../../../../../../third_party/cordio/ble-profiles/sources/services/svc_batt.c
SRCS += ../../../../../../third_party/cordio/ble-profiles/sources/services/svc_bps.c
SRCS += ../../../../../../third_party/cordio/ble-profiles/sources/services/svc_core.c
SRCS += ../../../../../../third_party/cordio/ble-profiles/sources/services/svc_cps.c
SRCS += ../../../../../../third_party/cordio/ble-profiles/sources/services/svc_cscs.c
SRCS += ../../../../../../third_party/cordio/ble-profiles/sources/services/svc_cte.c
SRCS += ../../../../../../third_party/cordio/ble-profiles/sources/services/svc_dis.c
SRCS += ../../../../../../third_party/cordio/ble-profiles/sources/services/svc_gls.c
SRCS += ../../../../../../third_party/cordio/ble-profiles/sources/services/svc_gyro.c
SRCS += ../../../../../../third_party/cordio/ble-profiles/sources/services/svc_hid.c
SRCS += ../../../../../../third_party/cordio/ble-profiles/sources/services/svc_hrs.c
SRCS += ../../../../../../third_party/cordio/ble-profiles/sources/services/svc_hts.c
SRCS += ../../../../../../third_party/cordio/ble-profiles/sources/services/svc_ipss.c
SRCS += ../../../../../../third_party/cordio/ble-profiles/sources/services/svc_plxs.c
SRCS += ../../../../../../third_party/cordio/ble-profiles/sources/services/svc_px.c
SRCS += ../../../../../../third_party/cordio/ble-profiles/sources/services/svc_rscs.c
SRCS += ../../../../../../third_party/cordio/ble-profiles/sources/services/svc_scpss.c
SRCS += ../../../../../../third_party/cordio/ble-profiles/sources/services/svc_temp.c
SRCS += ../../../../../../third_party/cordio/ble-profiles/sources/services/svc_time.c
SRCS += ../../../../../../third_party/cordio/ble-profiles/sources/services/svc_uricfg.c
SRCS += ../../../../../../third_party/cordio/ble-profiles/sources/services/svc_wdxs.c
SRCS += ../../../../../../third_party/cordio/ble-profiles/sources/services/svc_wp.c
SRCS += ../../../../../../third_party/cordio/ble-profiles/sources/services/svc_wss.c

$(CONFIG)/$(TARGET).bin: $(LIBS) $(INCS) $(SRCS)
	IarBuild.exe ble_freertos_locator.ewp -make Debug -log info

directories: $(CONFIG)

$(CONFIG):
	@mkdir -p $@


clean:
	@echo Cleaning... ;\
	IarBuild.exe ble_freertos_locator.ewp -clean Debug -log all

endif
.PHONY: all clean directories

