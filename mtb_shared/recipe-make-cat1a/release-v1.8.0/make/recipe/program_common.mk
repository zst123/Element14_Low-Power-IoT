################################################################################
# \file program_common.mk
#
# \brief
# Common variables and targets for program.mk
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

CY_GDB_CLIENT=$(CY_INTERNAL_TOOL_arm-none-eabi-gdb_EXE)
CY_OPENOCD_EXE=$(CY_INTERNAL_TOOL_openocd_EXE)
CY_OPENOCD_SCRIPTS=-s $(CY_INTERNAL_TOOL_openocd_scripts_SCRIPT)

CY_OPENOCD_INTERFACE=source [find interface/kitprog3.cfg];
CY_OPENOCD_TARGET=source [find target/$(CY_OPENOCD_DEVICE_CFG)];
ifeq ($(CY_OPENOCD_QSPI_CFG_PATH),)
CY_OPENOCD_QSPI=
else
CY_OPENOCD_QSPI=-s $(CY_OPENOCD_QSPI_CFG_PATH)
endif

erase:
	$(CY_NOISE)echo;\
	echo "Erasing target device... ";\
	$(CY_OPENOCD_EXE) $(CY_OPENOCD_ERASE_ARGS)

program: build qprogram

qprogram: memcalc
ifeq ($(LIBNAME),)
	$(CY_NOISE)echo;\
	echo "Programming target device... ";\
	$(CY_OPENOCD_EXE) $(CY_OPENOCD_PROGRAM_ARGS)
else
	$(CY_NOISE)echo "Library application detected. Skip programming... ";\
	echo
endif

debug: program qdebug

qdebug: qprogram
ifeq ($(LIBNAME),)
	$(CY_NOISE)echo;\
	echo ==============================================================================;\
	echo "Instruction:";\
	echo "Open a separate shell and run the attach target (make attach)";\
	echo "to start the GDB client. Then use the GDB commands to debug.";\
	echo ==============================================================================;\
	echo;\
	echo "Opening GDB port ... ";\
	$(CY_OPENOCD_EXE) $(CY_OPENOCD_DEBUG_ARGS)
else
	$(CY_NOISE)echo "Library application detected. Skip debug... ";\
	echo
endif

attach:
	$(CY_NOISE)echo;\
	echo "Starting GDB Client... ";\
	$(CY_GDB_CLIENT) $(CY_OPENOCD_SYMBOL_IMG) -x $(CY_GDB_ARGS)


.PHONY: erase program qprogram debug qdebug attach
