################################################################################
# \file recipe_common.mk
#
# \brief
# Common variables and targets for recipe.mk
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


################################################################################
# Compiler and linker arguments
################################################################################

#
# Flags construction
#
CY_RECIPE_CFLAGS?=\
	$(CFLAGS)\
	$(CY_TOOLCHAIN_CFLAGS)

CY_RECIPE_CXXFLAGS?=\
	$(CXXFLAGS)\
	$(CY_TOOLCHAIN_CXXFLAGS)

CY_RECIPE_ASFLAGS?=\
	$(ASFLAGS)\
	$(CY_TOOLCHAIN_ASFLAGS)

CY_RECIPE_ARFLAGS?=\
	$(CY_TOOLCHAIN_ARFLAGS)

CY_RECIPE_LDFLAGS?=\
	$(LDFLAGS)\
	$(CY_TOOLCHAIN_LDFLAGS)\
	$(CY_RECIPE_LSFLAG)

#
# Defines construction
#
ifneq ($(DEFINES),)
CY_RECIPE_USER_DEFINES=$(addprefix -D,$(DEFINES))
endif
ifneq ($(LIBNAME),)
CY_RECIPE_USER_NAME=-DCY_LIBNAME_$(subst -,_,$(LIBNAME))
else
CY_RECIPE_USER_NAME=-DCY_APPNAME_$(subst -,_,$(APPNAME))
endif

CY_RECIPE_DEFINES=\
	$(CY_RECIPE_USER_DEFINES)\
	$(CY_RECIPE_USER_NAME)\
	-D$(subst -,_,$(DEVICE))\
	-DCY_TARGET_DEVICE=$(subst -,_,$(DEVICE))\
	-DTARGET_$(subst -,_,$(TARGET))\
	-DCY_TARGET_BOARD=$(subst -,_,$(TARGET))\
	$(foreach feature,$(CY_COMPONENT_LIST),-DCOMPONENT_$(subst -,_,$(feature)))\
	$(CY_TOOLCHAIN_DEBUG_FLAG)\
	$(CY_TOOLCHAIN_DEFINES)

#
# Includes construction
#
CY_RECIPE_INCLUDES=\
	$(addprefix -I,$(INCLUDES))\
	$(addprefix -I,$(CY_SEARCH_APP_INCLUDES))\
	$(addprefix -I,$(CY_TOOLCHAIN_INCLUDES))

#
# Sources construction
#
CY_RECIPE_SOURCE=$(CY_SEARCH_APP_SOURCE)

#
# Libraries construction
#
CY_RECIPE_LIBS=$(LDLIBS) $(CY_SEARCH_APP_LIBS)

#
# Generate source step
#
ifneq ($(CY_SEARCH_RESOURCE_FILES),)
CY_RECIPE_RESOURCE_FILES=$(CY_SEARCH_RESOURCE_FILES)
CY_RECIPE_GENERATED_FLAG=TRUE

# Define the generated source file. Use := for better performance
CY_RECIPE_GENERATED:=$(addprefix $(CY_GENERATED_DIR)/,$(addsuffix .$(CY_TOOLCHAIN_SUFFIX_C),\
					$(basename $(notdir $(subst .,_,$(CY_SEARCH_RESOURCE_FILES))))))

CY_RECIPE_GENSRC=\
	bash --norc --noprofile\
	$(CY_BASELIB_CORE_PATH)/make/scripts/genresources.bash\
	$(CY_BASELIB_CORE_PATH)/make/scripts\
	$(CY_GENERATED_DIR)/resources.cyrsc\
	$(CY_INTERNAL_APP_PATH)\
	$(CY_GENERATED_DIR)\
	"MEM"
endif

#
# Prebuild step
#
CY_RECIPE_PREBUILD?=

#
# Postbuild step
#
ifeq ($(LIBNAME),)

CY_RECIPE_OBJCOPY=$(CY_INTERNAL_TOOL_arm-none-eabi-objcopy_EXE)

ifeq ($(TOOLCHAIN),A_Clang)
CY_RECIPE_POSTBUILD?=$(CY_RECIPE_ACLANG_POSTBUILD)

else ifeq ($(TOOLCHAIN),ARM)
ifeq ($(CY_COMPILER_PATH),)
CY_RECIPE_POSTBUILD?=$(CY_COMPILER_ARM_DIR)/bin/fromelf --output $(CY_CONFIG_DIR)/$(APPNAME).$(CY_TOOLCHAIN_SUFFIX_PROGRAM) --i32combined $(CY_CONFIG_DIR)/$(APPNAME).$(CY_TOOLCHAIN_SUFFIX_TARGET)
else
CY_RECIPE_POSTBUILD?=$(CY_COMPILER_PATH)/bin/fromelf --output $(CY_CONFIG_DIR)/$(APPNAME).$(CY_TOOLCHAIN_SUFFIX_PROGRAM) --i32combined $(CY_CONFIG_DIR)/$(APPNAME).$(CY_TOOLCHAIN_SUFFIX_TARGET)
endif

else ifeq ($(TOOLCHAIN),IAR)
CY_RECIPE_POSTBUILD?=$(CY_RECIPE_OBJCOPY) -O ihex $(CY_CONFIG_DIR)/$(APPNAME).$(CY_TOOLCHAIN_SUFFIX_TARGET) $(CY_CONFIG_DIR)/$(APPNAME).$(CY_TOOLCHAIN_SUFFIX_PROGRAM)

else ifeq ($(TOOLCHAIN),GCC_ARM)
ifeq ($(CY_COMPILER_PATH),)
CY_RECIPE_POSTBUILD?=$(CY_RECIPE_OBJCOPY) -O ihex $(CY_CONFIG_DIR)/$(APPNAME).$(CY_TOOLCHAIN_SUFFIX_TARGET) $(CY_CONFIG_DIR)/$(APPNAME).$(CY_TOOLCHAIN_SUFFIX_PROGRAM)
else
CY_RECIPE_POSTBUILD?=$(CY_COMPILER_PATH)/bin/arm-none-eabi-objcopy -O ihex $(CY_CONFIG_DIR)/$(APPNAME).$(CY_TOOLCHAIN_SUFFIX_TARGET) $(CY_CONFIG_DIR)/$(APPNAME).$(CY_TOOLCHAIN_SUFFIX_PROGRAM)
endif
endif

endif


################################################################################
# Memory Consumption
################################################################################

CY_RECIPE_READELF=$(CY_INTERNAL_TOOL_arm-none-eabi-readelf_EXE)

ifeq ($(TOOLCHAIN),A_Clang)
CY_GEN_READELF=
CY_MEMORY_CALC=
else
CY_GEN_READELF=$(CY_RECIPE_READELF) -Sl $(CY_CONFIG_DIR)/$(APPNAME).$(CY_TOOLCHAIN_SUFFIX_TARGET) > $(CY_CONFIG_DIR)/$(APPNAME).readelf
CY_MEM_CALC=\
	bash --norc --noprofile\
	$(CY_BASELIB_CORE_PATH)/make/scripts/memcalc.bash\
	$(CY_CONFIG_DIR)/$(APPNAME).readelf\
	$(CY_MEMORY_FLASH)\
	$(CY_MEMORY_SRAM)\
	$(CY_START_FLASH)\
	$(CY_START_SRAM)
endif

memcalc:
ifeq ($(LIBNAME),)
	$(CY_NOISE)echo Calculating memory consumption: $(DEVICE) $(TOOLCHAIN) $(CY_TOOLCHAIN_OPTIMIZATION)
	$(CY_NOISE)echo
	$(CY_NOISE)$(CY_GEN_READELF)
	$(CY_NOISE)$(CY_MEM_CALC)
	$(CY_NOISE)echo
endif

#
# Identify the phony targets
#
.PHONY: memcalc
