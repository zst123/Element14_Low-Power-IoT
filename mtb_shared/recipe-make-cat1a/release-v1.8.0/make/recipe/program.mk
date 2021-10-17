################################################################################
# \file program.mk
#
# \brief
# This make file is called recursively and is used to build the
# resoures file system. It is expected to be run from the example directory.
#
################################################################################
# \copyright
# Copyright 2018-2020 Cypress Semiconductor Corporation
# SPDX-License-Identifier: Apache-2.0
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
################################################################################

ifeq ($(WHICHFILE),true)
$(info Processing $(lastword $(MAKEFILE_LIST)))
endif

include $(CY_INTERNAL_BASELIB_PATH)/make/recipe/program_common.mk

CY_GDB_ARGS=$(CY_INTERNAL_BASELIB_PATH)/make/scripts/$(CY_GDBINIT_FILE)

ifeq ($(TOOLCHAIN),A_Clang)
CY_OPENOCD_PROGRAM_IMG=$(CY_CONFIG_DIR)/$(APPNAME).bin $(TOOLCHAIN_VECT_BASE_CM4)
else
CY_OPENOCD_SYMBOL_IMG=$(CY_CONFIG_DIR)/$(APPNAME).$(CY_TOOLCHAIN_SUFFIX_TARGET)
CY_OPENOCD_PROGRAM_IMG=$(CY_CONFIG_DIR)/$(APPNAME).$(CY_TOOLCHAIN_SUFFIX_PROGRAM)
endif

ifeq (,$(findstring $(DEVICE),$(CY_DEVICES_SECURE)))
CY_OPENOCD_CUSTOM_COMMAND?=psoc6 allow_efuse_program off;
endif

CY_OPENOCD_ERASE=init; reset init; psoc6 sflash_restrictions 1; erase_all; exit;
CY_OPENOCD_PROGRAM=psoc6 sflash_restrictions 1; program $(CY_OPENOCD_PROGRAM_IMG) verify reset exit;
CY_OPENOCD_DEBUG=$(CY_OPENOCD_CHIP_NAME).cpu.$(CY_OPENOCD_CORE) configure -rtos auto -rtos-wipe-on-reset-halt 1; $(CY_OPENOCD_EXTRA_PORT_FLAG); init; reset init;

CY_OPENOCD_ERASE_ARGS=$(CY_OPENOCD_SCRIPTS) -c \
					"$(CY_OPENOCD_INTERFACE) $(CY_OPENOCD_CM0_DISABLE_FLAG); $(CY_OPENOCD_SMIF_DISABLE); $(CY_OPENOCD_TARGET) $(CY_OPENOCD_CUSTOM_COMMAND) $(CY_OPENOCD_ERASE)"
CY_OPENOCD_PROGRAM_ARGS=$(CY_OPENOCD_SCRIPTS) $(CY_OPENOCD_QSPI) -c \
					"$(CY_OPENOCD_INTERFACE) $(CY_OPENOCD_CM0_DISABLE_FLAG); $(CY_OPENOCD_TARGET) $(CY_OPENOCD_CUSTOM_COMMAND) $(CY_OPENOCD_PROGRAM)"
CY_OPENOCD_DEBUG_ARGS=$(CY_OPENOCD_SCRIPTS) $(CY_OPENOCD_QSPI) -c \
					"$(CY_OPENOCD_INTERFACE) $(CY_OPENOCD_CM0_DISABLE_FLAG); $(CY_OPENOCD_TARGET) $(CY_OPENOCD_CUSTOM_COMMAND) $(CY_OPENOCD_DEBUG)"
