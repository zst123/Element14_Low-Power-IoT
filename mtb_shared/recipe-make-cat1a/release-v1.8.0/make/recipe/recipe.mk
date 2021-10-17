################################################################################
# \file recipe.mk
#
# \brief
# Set up a set of defines, includes, software components, linker script, 
# Pre and Post build steps and call a macro to create a specific ELF file.
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

include $(CY_INTERNAL_BASELIB_PATH)/make/recipe/recipe_common.mk

#
# linker script construction
#
ifeq ($(LINKER_SCRIPT),)
LINKER_SCRIPT=$(CY_TARGET_DIR)/COMPONENT_$(CORE)/TOOLCHAIN_$(TOOLCHAIN)/$(CY_LINKER_SCRIPT_NAME)_$(CY_LINKERSCRIPT_SUFFIX).$(CY_TOOLCHAIN_SUFFIX_LS)

# Old path to linker script
ifeq ($(wildcard $(LINKER_SCRIPT)),)
LINKER_SCRIPT=$(CY_TARGET_DIR)/linker/TOOLCHAIN_$(TOOLCHAIN)/$(CY_LINKER_SCRIPT_NAME)_$(CY_LINKERSCRIPT_SUFFIX).$(CY_TOOLCHAIN_SUFFIX_LS)
endif
endif

ifeq ($(wildcard $(LINKER_SCRIPT)),)
$(call CY_MACRO_ERROR,The specified linker script could not be found at "$(LINKER_SCRIPT)")
endif

ifeq ($(TOOLCHAIN),A_Clang)
include $(LINKER_SCRIPT)
else
CY_RECIPE_LSFLAG=$(CY_TOOLCHAIN_LSFLAGS)$(LINKER_SCRIPT)
endif

# Aclang arguments must match the symbols in the PDL makefile
CY_RECIPE_ACLANG_POSTBUILD=\
	$(CY_TOOLCHAIN_M2BIN)\
	--verbose --vect $(VECT_BASE_CM4) --text $(TEXT_BASE_CM4) --data $(RAM_BASE_CM4) --size $(TEXT_SIZE_CM4)\
	$(CY_CONFIG_DIR)/$(APPNAME).mach_o\
	$(CY_CONFIG_DIR)/$(APPNAME).bin

################################################################################
# Programmer tool
################################################################################

CY_PROGTOOL_FW_LOADER=$(CY_INTERNAL_TOOL_fw-loader_EXE)
progtool:
	$(CY_NOISE)echo;\
	echo ==============================================================================;\
	echo "Available commands";\
	echo ==============================================================================;\
	echo;\
	"$(CY_PROGTOOL_FW_LOADER)" --help | sed s/'	'/' '/g;\
	echo ==============================================================================;\
	echo "Connected device(s)";\
	echo ==============================================================================;\
	echo;\
	deviceList=$$("$(CY_PROGTOOL_FW_LOADER)" --device-list | grep "FW Version" | sed s/'	'/' '/g);\
	if [[ ! -n "$$deviceList" ]]; then\
		echo "ERROR: Could not find any connected devices";\
		echo;\
		exit 1;\
	else\
		echo "$$deviceList";\
		echo;\
	fi;\
	echo ==============================================================================;\
	echo "Input command";\
	echo ==============================================================================;\
	echo;\
	echo " Specify the command (and optionally the device name).";\
	echo " E.g. --mode kp3-daplink KitProg3 CMSIS-DAP HID-0123456789ABCDEF";\
	echo;\
	read -p " > " -a params;\
	echo;\
	echo ==============================================================================;\
	echo "Run command";\
	echo ==============================================================================;\
	echo;\
	paramsSize=$${#params[@]};\
	if [[ $$paramsSize > 2 ]]; then\
		if [[ $${params[1]} == "kp3-"* ]]; then\
			deviceName="$${params[@]:2:$$paramsSize}";\
			"$(CY_PROGTOOL_FW_LOADER)" $${params[0]} $${params[1]} "$$deviceName" | sed s/'	'/' '/g;\
		else\
			deviceName="$${params[@]:1:$$paramsSize}";\
			"$(CY_PROGTOOL_FW_LOADER)" $${params[0]} "$$deviceName" | sed s/'	'/' '/g;\
		fi;\
	else\
		"$(CY_PROGTOOL_FW_LOADER)" "$${params[@]}" | sed s/'	'/' '/g;\
	fi;

.PHONY: progtool
