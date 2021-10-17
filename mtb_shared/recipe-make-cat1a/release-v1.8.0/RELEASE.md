# PSoC 6 GNU make Build System Release Notes
This repo provides the build recipe make files and scripts for building and programming PSoC 6 applications. Builds can be run either through a command-line interface (CLI) or through the ModusToolbox Integrated Development Environment (IDE).

### What's Included?
The this release of the PSoC 6 GNU make build recipe includes complete support for building, programming, and debugging PSoC 6 application projects. It is expected that a code example contains a top level make file for itself and references a Board Support Package (BSP) that defines specifics items, like the PSoC 6 part, for the target board. Supported functionality includes the following:
* Supported operations:
    * Build
    * Program
    * Debug
    * IDE Integration (Eclipse, VS Code, IAR, uVision)
    * BSP Generation
* Supported toolchains:
    * GCC
    * IAR
    * ARMv6

This also includes the getlibs.bash script that can be used directly, or via the make target to download additional git repo based libraries for the application.

### What Changed?
#### v1.8.0
* Added support for cys06xxa linker script
* Updated launch configs for PSoC6 256K devices
* Fixed a compatibility bug introduced with make vscode in core-make-1.8.0
#### v1.7.2
* Fix an issue causing make bsp to not include co-processor into custom bsp's design.modus
#### v1.7.1
* Fix an issue with make progtool
* Improved eclipse and vscode launch configuration
#### v1.7.0
* Added ml-configurator
#### v1.5.1
* Fixed bug in PSoC 6 CM0+ Eclipse programming launch config file
#### v1.5.0
* Added support for finding patched versions of the ModusToolbox tools
* Added support for .mtb files
* Improved support for sharing code between projects
#### v1.4.1
* Added support for PSoC 6 S2 & S3 single core devices
* Fixed timing problem starting debugger for some PSoC 64 parts
#### v1.4.0
* Improved compatibility with MTB 2.0 and 2.1 tools
* Improved support for J-Link
* Improved support for different toolchains in VSCode
* Improved support for PSoC 64 based boards
* Fixed issue with output formatting on Mac
#### v1.3.1
* Updated IAR to include '--threaded_lib' when building for FreeRTOS.
NOTE: This requires the IAR System lock functions to be defined. Implementations for these are provided by the [clib-support](https://github.com/cypresssemiconductorco/clib-support) library.
#### v1.3.0
* Separated build process into two stages to improve robustness
* Added support for building dependent apps for CM0+ & MCU Boot
* Added support for exporting to uvision5
#### v1.2.1
* Added support for PSoC 64 S3 parts
* Improved description for some targets
#### v1.2.0
* Added support for generating new BSPs
* Added support for launching Library Manager
* Added support for integrating with Embedded Workbench (beta)
* Improved support for integrating with Eclipse
* Improved file discovery performance
#### v1.1.0
* Improved support for Multi-Core boards
* Minor Bug fixes
#### v1.0.1
* Minor improvement for vscode target
#### v1.0.0
* Initial release supporting build/program/debug on gcc/iar/armv6 toolchains

### Product/Asset Specific Instructions
Builds require that the ModusToolbox tools be installed on your machine. This comes with the ModusToolbox install. On Windows machines, it is recommended that CLI builds be executed using the Cygwin.bat located in ModusToolBox/tools_x.y/modus-shell install directory. This guarantees a consistent shell environment for your builds.

To list the build options, run the "help" target by typing "make help" in CLI. For a verbose documentation on a specific subject type "make help CY_HELP={variable/target}", where "variable" or "target" is one of the listed make variables or targets.

### Supported Software and Tools
This version of the PSoC 6 build system was validated for compatibility with the following Software and Tools:

| Software and Tools                        | Version |
| :---                                      | :----:  |
| ModusToolbox Software Environment         | 2.3     |
| GCC Compiler                              | 9.2     |
| IAR Compiler                              | 8.4     |
| ARM Compiler                              | 6.11    |

Minimum required ModusToolbox Software Environment: v2.0

### More information
Use the following links for more information, as needed:
* [Cypress Semiconductor, an Infineon Technologies Company](http://www.cypress.com)
* [Cypress Semiconductor GitHub](https://github.com/cypresssemiconductorco)
* [ModusToolbox](https://www.cypress.com/products/modustoolbox-software-environment)

---
© Cypress Semiconductor Corporation, 2019-2020.