# PSoC 6 MCU: Find Me BLE Example

This is an application implementing a BLE Immediate Service Alert.
## Requirements

- [ModusToolbox® software](https://www.cypress.com/products/modustoolbox-software-environment) v2.3
- Board Support Package (BSP) minimum required version: 2.0.0  
- Programming Language: C
- Associated Parts: - [PSoC 6 BLE Prototyping Kit](https://www.cypress.com/CY8CPROTO-063-BLE) (`CY8CPROTO-063-BLE`)

## Supported Toolchains (make variable 'TOOLCHAIN')

- GNU Arm® Embedded Compiler v9.3.1 (`GCC_ARM`) - Default value of `TOOLCHAIN`
- Arm compiler v6.11 (`ARM`)
- IAR C/C++ compiler v8.42.2 (`IAR`)

## Design and Implementation

This template has two applications - *cm0p_app* and *cm4_app*. Their makefiles were modified to establish a dependency between the two applications, so both images built are merged into one. 

In the *cm0p_app* makefile, the following line were added to specify the application CPU:

```
CORE=CM0P
```

If you intend to use the *design.modus* generated code in CM0+, then add `BSP_DESIGN_MODUS` to the `COMPONENTS=` in the CM0+ Makefile.

In the *cm4_app* Makefile, the following lines were added/modified to add a dependency to *cm0p_app*:

```
DISABLE_COMPONENTS=CM0P_SLEEP
DEPENDENT_APP_PATHS=../cm0p_app

getlibs : getlibs_cm0p
getlibs_cm0p:
	$(MAKE) -C ../cm0p_app/ $(MAKECMDGOALS)
```

The last lines are added to trigger the `make getlibs` command in the *cm0p_app* folder, when issuing the same command in the *cm4_app* folder. Each application should have its own *deps* folder. If the same library is used by both applications, it should be in the *deps* folder of both applications. If the library location is specified as the shared asset repo in the *mtb* file (which is by default), they will both automatically access it from the shared location. 

When the CM4 project is built, it builds both the CM0+ and CM4 applications and the resulting build artifacts contain code for both applications. When the CM0+ application is built, it will only build that application. When the application starts, the CM0+ starts first. It is responsible for starting the CM4.

By default, *cm0p_app* uses only 8192 bytes of SRAM and flash. If you wish to allocate more memory to *cm0p_app*, follow the instructions from the section "Customizing Linker Scripts" in [AN215656](https://www.cypress.com/AN215656) – *PSoC 6 MCU Dual-CPU System Design*.

The HAL library only supports the CM4, so *cyhal.h* is not included in the CM0+ application. The PDL library supports both the CM4 and CM0+.