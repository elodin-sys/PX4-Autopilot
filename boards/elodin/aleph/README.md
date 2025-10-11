# Notes for getting the Elodin Aleph Running

## Overview

The .px4board files contain the various configuration options for the firmware, similar to a Kconfig file in Zephyr. We need the latest Aleph hardware to contain drivers for:
- BMI270
- BMM350
- BMP581

These drivers are provided in the stock PX4 firmware main branch which is currently at v1.16.0rc3. This [repo](https://github.com/Pictorus-Labs/PX4-Autopilot) branch v1.16.0-rc3-aleph contains the latest v1.16.0rc3 with the Aleph board.

## Configuring the firmware

Running `make elodin_aleph_default boardconfig` brings up a TUI that enables configuration of the `default` settings that wind up in the `.px4board` files, in this case the `default.px4board` file. The original settings generated a firmware binary larger than the flash size and also lacked the new sensors.

 ## Bootloader (and issues using a Nucleo H753 board)
 PX4 (and Aelph) uses a bootloader model, a small firmware program that runs at boot with limited connectivity and a small subset of functionality. The `CMakeLists.txt` file shows the bootloader is compiled with:
 ```
 if("${PX4_BOARD_LABEL}" STREQUAL  "bootloader")
	add_compile_definitions(BOOTLOADER)
	add_library(drivers_board
		bootloader_main.c
		init.c
		usb.c
		timer_config.cpp
	)
	target_link_libraries(drivers_board
		PRIVATE
			nuttx_arch # sdio
			nuttx_drivers # sdio
			px4_layer #gpio
			arch_io_pins # iotimer
			bootloader
	)
	target_include_directories(drivers_board PRIVATE ${PX4_SOURCE_DIR}/platforms/nuttx/src/bootloader/common)
```

We should have USB working when the bootloader is successfully flashed, which we can then use with `QGroundControl` to flash the main firmware. `make elodin_aleph_bootloader` creates the bootloader and puts several binary flavors in `./build/elodin_aleph_bootloader`.

You can use STM32CubeProgrammer to write the bootloader binary to address 0x08000000 or flash the ELF. Once this is flashed, the STM32 USB should appear in `dmesg` alongside the RP2040.


## List of configuration files
There are a lot of configuration files:
- `.px4board` - Lists the PX4 specific configuration parameters and drivers to include
- `./nuttx-config/include/board.h` - Pin mappings for NuttX
- `./src/board_config.h` - Another set of pin mappings, used for PX4
- `./nuttx-config/nsh/defconfig` - Board level configuration for NuttX, has things like I2C enable, SPI enable, etc. [NuttX SHell is a command line like tool that that runs at NuttX boot](https://nuttx.apache.org/docs/10.0.0/components/nsh/nsh.html)


## New BMI270 I2C Driver
This driver takes the SPI variant of the BMI270 with some help from the BMI088 I2C driver to make a new BMI270 I2C driver. The new driver replaces the SPI instantiation with I2C, changes the parameters a bit, and can be configured using the TUI. The SPI `transfer` function is different from the I2C `transfer` function due to the nature of the busses, and had to be changed. If there are any issues with the I2C IMUs, check the `transfer` function implementation.

__NOTE__: There is an issue with the I2C transfer implementation in the `STATE::MICROCODE_LOAD`. The SPI version sets several registers in multi-byte update that doesn't translate 1:1 to I2C. Needs to be fixed, but low on the priority list. It is not currently loaded at boot right now.

To debug this issue:
- Once the primary firmware is loaded, you can attach the debugger.
- Set a breakpoint in the bmi270_i2c.cpp file in the `STATE::MICROCODE_LOAD`
- Login to `NuttX` shell using picocom or another serial terminal:
picocom -b 115200 -l -n --flow n /dev/ttyACM1
- Run `bmi270_i2c start -I -b 3` to launch the program.
- The breakpoint should hit

Next steps:
- Try to figure out the limits of a multi-byte I2C write


### Current Peripheral implementation
With the new sensors, we can disable or delete the old ones and add in the new ones. Some notes have been made in `rc.board_sensors` regarding the disabled sensors, with the addition of:

```
mount -t vfat /dev/mmcsd0 /mnt/sdcard

# Internal magnetometer on I2c3
bmm350 start -I -b 3

# Internal barometer on I2C3
bmp581 start -I -b 3

# BMI270 on SPI5
bmi270 start -s -b 5

# MS4520do Differential airspeed indicator
ms4525do start -I -b 4
```

## Building the firmware
Run `make elodin_aleph` to generate several flavors of binaries located at `./build/elodin_aleph_default`.

# Configure PX4 project on Apple Silicon
1. If you are on Apple Silicon, you will need to configure your terminal to run in Rosetta mode.

	The instructions are from the PX4 documentation for setting up a Rosetta / x86_64 terminal are out of date. I had to do the following.
	1. Open a new `terminal` window.
	1. Go to settings and create a new profile called `x86_64`.
	1. Select the new profile and switch to the `Shell` tab.
	1. Check the `Run command` checkbox and enter `arch -x86_64 zsh` in the command field.
	1. Check the `Run inside shell` checkbox.
	1. Open your new `x86_64` terminal.

2. Install homebrew for x86_64 architecture. Open the x86 terminal and run the install script:
`/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"`

	This should install homebrew in `/usr/local/homebrew` instead of `/opt/homebrew` where the Apple Silicon Homebrew lives. I also made an alias for the x86 homebrew in my `~/.zshrc` file: `alias ibrew="/usr/local/homebrew/bin/brew"`. For any commands that use `brew` + `x86_64`, use `ibrew` instead.

3. Install PX4 dev tools, though it probably won't work:
	```
	ibrew tap PX4/px4
	ibrew install px4-dev
	```

	NOTE: This step was problematic. The PX4/px4 repo has a SHA256 hash mismatch for the `px4-dev` formula and refused to install. I ended up forking the repo and patching the formula, [here](https://github.com/Pictorus-Labs/homebrew-px4/tree/amcelroy/sha256-px4-dev-update).

	These steps can be used to install the patched formula, make sure to use both the `x86_64` terminal and `ibrew` alias:
	1. Uninstall the official PX4/px4 tap:
		```
		ibrew untap PX4/px4
		```
	1. Run `ibrew tap Pictorus-Labs/px4 https://github.com/Pictorus-Labs/homebrew-px4.git --custom-remote && cd "$(ibrew --repo Pictorus-Labs/px4)"`. This should put you in `/usr/local/Homebrew/Library/Taps/Pictorus-Labs/homebrew-px4`.
	1. Fetch the latest changes:
		```
		git fetch origin
		git checkout amcelroy/sha256-px4-dev-update
		```
	1. Install the patched formula:
		```
		ibrew install Pictorus-Labs/px4/px4-dev
		```

4. Create a new `.venv`:
	```
	python3 -m venv .venv
	source .venv/bin/activate
	```

5. Install PX4 Python tools into the new `.venv`:
	```
	pip install pyserial empty toml numpy pandas jinja2 pyyaml pyros-genmsg packaging kconfiglib future jsonschema
	```

6. Try to build the firmware:
	```
	make elodin_aleph_default boardconfig
	make elodin_aleph
	```
If you run into issues, see the troubleshooting section below, there were several issues that popped up that were blockers. If everything works, you should see a terminal output that looks like:
```
Memory region         Used Size  Region Size  %age Used
        ITCM_RAM:          0 GB        64 KB      0.00%
           FLASH:     1867332 B      1920 KB     94.98%
       DTCM1_RAM:          0 GB        64 KB      0.00%
       DTCM2_RAM:          0 GB        64 KB      0.00%
        AXI_SRAM:      102088 B       512 KB     19.47%
           SRAM1:          0 GB       128 KB      0.00%
           SRAM2:          0 GB       128 KB      0.00%
           SRAM3:          0 GB        32 KB      0.00%
           SRAM4:          2 KB        64 KB      3.12%
          BKPRAM:          0 GB         4 KB      0.00%
[1163/1164] Creating /Users/<username>/Code/elodin/build/elodin_aleph_default/elodin_aleph_default.px4
```

## Troubleshooting macOS Issues
- CMake insists on using system Python instead of the `.venv` Python. Activate the correct `.venv` and run:
	```
	make distclean
	make PX4_CMAKE_ARGS="-DPYTHON_EXECUTABLE=$(which python3)" elodin_aleph
	```
	If this was done correctly, you should see the something like `-- Found PythonInterp: /Users/<username>/Code/elodin/.venv/bin/python3` in the CMake output that shows the correct Python and virtual environment is used. Once this was set, CMake seemed content to continue using the `.venv` Python and didn't need the PX4_CMAKE_ARGS anymore.

- `AttributeError: module 'em' has no attribute 'RAW_OPT'` - Use an older version of empy, for example `pip install "empy==3.3.4"`. Apparently the latest version of `empy` has a breaking change that PX4 doesn't support yet.
- `ModuleNotFoundError: No module named 'pkg_resources'` - Install `pip install setuptools`.
- Everything compiles, but there is a linker error. Ensure the `x86_64` toolchains are installed and the PATH is set correctly. Most likely a linker error is compiling with the `arm64` toolchain instead of the `x86_64` toolchain.
	```
	ibrew install gcc@11
	export PATH="/usr/local/bin:$PATH"
	which arm-none-eabi-gcc
	file $(which arm-none-eabi-gcc)
	# Should say: Mach-O 64-bit executable x86_64
	```
	Add this to your `~/.zshrc` file to point the x86_64 terminal to the correct toolchain. Also adds in a bump to the number of files that can be opened in the terminal (see next bullet point).
	```
	if [[ "$(uname -m)" == "x86_64" ]]; then
		# You're in a Rosetta (x86_64 translated) terminal
		export PATH="/usr/local/bin:$PATH"
		echo "[Rosetta] Using x86_64 Homebrew toolchain"
		ulimit -n 4096
	fi
	```
	Close and re-open the terminal, go to the PX4 project directory, activate the .venv, and try the `make` command again.
- A linker error along the lines of: `too many files open`. Ensure `ulimit -n 4096` is set in your terminal, it was added in the bullet point above this one in the `~/.zshrc` file.
- When running `ibrew install Pictorus-Labs/px4/px4-dev`, any compile time issues might be related to the location of the x86 Homebrew install. Check the location of the x86 brew installation and update the alias line in `~/.zshrc` (for example, `alias ibrew="/usr/local/bin/brew"`).

# Using the Firmware
Once the firmware has been built, launch QGroundControl click on the `Q` menu icon in the upper left:
- Go to Vehicle configuration
- Firmware
- You may need to unplug and re-plug in the USB-C cable
- A pop-up should appear that has some options. Select `load custom firmware` and navigate to the .px4 binary file that was build in the previous step and load that.
- Once that is loaded, Mavlink should be working on UART7 and the parameters should be automatically fetched from the board. I used a USB -> Serial TTL cable since the UART telemetry link was dropping a lot of packets.


# More notes

## Configuring the MCU NuttX
The NuttX configuration file for the board and the boardloader are located at:
- `./nuttx-config/nsh/defconfig` - NuttX configuration file for the board
- `./nuttx-config/bootloader/defconfig` - NuttX configuration file for the bootloader

Run `make elodin_aleph_default menuconfig` to bring up the NuttX configuration menu.

## UART Mappings
- UART1 (NuttX: /dev/ttyS0) -> Carrier Board
- UART2 (NuttX: /dev/ttyS1) -> GPS (J7)
- UART3 (NuttX: /dev/ttyS2) -> RP2040 UART0
- UART6 (NuttX: /dev/ttyS3) -> RP2040 UART1
- UART7 (NuttX: /dev/ttyS4) -> Mavlink (J4)
- UART8 (NuttX: /dev/ttyS5) -> RC Control (J5)

## I2C Mappings
- I2C1 -> EEPROM (CAT24C512)
- I2C2 -> BMI270
- I2C3 -> EEPROM, BMM350, BMP581
- I2C4 -> Currently goes to an I2C splitter and ms4525do

## SPI Mappings
- SPI1 -> Carrier board
- SPI4 -> FRAM
- SPI5 -> BMI270

## CAN Mappings
- FDCAN1 -> J10 (not enabled in firmware)
- FDCAN2 -> J2 (not enabled in firmware)

## USB Mappings
- USB0 -> Goes to USB hub and mapped to bootloader, or in the primary firmware the NuttX Shell
- USB1 -> Carrier Board

## ADC Mappings
- PA0 - VMON-VBAT
- PC2 - VMON-VIN
- PC3 - IMON-5V-AUX

## GPIO Mappings
- PG3 - GPIO0
- PG2 - GPIO1
- PK2 - GPIO2
- PG4 - GPIO3
- PG5 - GPIO4
- PG6 - GPIO5
- PG8 - GPIO6
- PG7 - GPIO7
- PH15 - 5V-AUX-EN

## Configuring the MCU PX4
Run `make elodin_aleph_default boardconfig` to access the PX4 configuration menu.

## TODO / Open Items:
- [X] Do SPI pins need to be added to `PX4_GPIO_INIT_LIST`? Do the I2C pins need to be removed? Added the pins to the `PX4_GPIO_INIT_LIST` in `./src/board_config.h`, possibly remove? NuttX seems to handle GPIO setup in the modules, `PX4_GPIO_INIT_LIST` is more for extra PX4 functionality (heaters, arming, that sort of thing).1
- [X] SPI1 CS (CS mapping this is on in spi.cpp, pin mapping is done in board.h).
- [X] Check UARTX, I2CX, SPIX pin mappings in `./boards/elodin/aleph/nuttx-config/board.h` vs. the Elodin latest docs to make sure they match.
- [X] Timer configuration in timer_config.cpp is probably not correct since it includes timer 12 which isn't on the schematic.
- [X] ADC configuration for PA0-C, PC2-C, and PC3-C
- [X] GPIO to control the 5V boost IC
- [X] GPIO for MCU_GPIO0-7 pins
- [X] Remap UART tty names and function using `make elodin_aleph_default boardconfig` to access PX4 menu
- [X] Validate the bootloader works as expected on Aleph or Pixhawk hardware
- [X] Validate that the firmware can be loaded
- [X] Flash (debugprobe)[https://github.com/raspberrypi/debugprobe/releases] to RP2040
- [ ] Test sensors:
   - [ ] PWM
   - [X] BMI270 SPI
   - [X] SD Card
   - [X] BMM350
   - [X] BMP581
   - [ ] BMI270 I2C (need to fix an I2C transfer error)

# Issues
- [ ] Not sure what to do with board_config.h options
	- //PX4_I2C_BUS_MTD (update 8/20/25 - The SPI4 FRAM is routed to be used as MTD, though we are using the /mnt/sdcard/param to hold parameters)
	- //UAVCAN_NUM_IFACES_RUNTIME (update 8/20/25 - CAN is disabled)
	- //BOARD_MTD_NUM_EEPROM (update 8/20/25 - not using)
	- //PX4_I2C_OBDEV_SE050
	- //The first 2 banks of #define settings (ttyS5, LTC44) (update 8/20/25 - these are for companion STM chips, we are not using them.)

# stm32h7x7xx_rcc.c file

NuttX doesn't seem to set the correct PWR peripheral CR3 register correctly. As per Rust:
```
pwr.cr3.modify(|_, w| {
	// Disable bypass, SMPS
	w.bypass().clear_bit();
	w.sden().clear_bit();
	// Enable LDO
	w.ldoen().set_bit()
});
```
we need these settings. Replace the NuttX one (platforms/nuttx/NuttX/nuttx/arch/arm/stm32h7) with the on in the Elodin repo.


# Make clean can delete some settings:

launch.json - Debugs the Bootloader using a Segger J-Link, change the executable path and gdbPath to you local settings:
```
{
	"name": "J-Link (STM32H747 M7) — Bootloader",
	"type": "cortex-debug",
	"request": "launch",
	"servertype": "jlink",
	"executable": "${workspaceFolder}/build/elodin_aleph_bootloader/elodin_aleph_bootloader.elf",
	"gdbPath": "/home/austin/gcc-arm-none-eabi-10-2020-q4-major/bin/arm-none-eabi-gdb",

	"device": "STM32H747II",              // J-Link device name
	"interface": "swd",
	"serialNumber": "",                   // (optional) lock to a specific J-Link by S/N
	"runToEntryPoint": "Reset_Handler",   // or "main" after bring-up
	"svdFile": "",                        // (optional) path to H747 SVD, if you have it
	//"jlinkTargetInterfaceSpeed": 1000,    // start slow (kHz); raise once stable (e.g., 4000)

	// If J-Link is not on PATH, set serverpath to JLinkGDBServerCLExe:
	// "serverpath": "/opt/SEGGER/JLink/JLinkGDBServerCLExe",

	"cwd": "${workspaceFolder}",
	"preLaunchCommands": [
	"set mem inaccessible-by-default off",
	"set print pretty on",
	"set confirm off"
	],
	"postLaunchCommands": [
	// make sure we actually halt at reset
	"monitor reset",
	"monitor halt",
	"monitor clrbp",
	"monitor regs"
	],
	"svdLoadAddress": "symbol",           // use ELF symbols for peripheral base (safe default)
	"printCalls": true,
	"showDevDebugOutput": "raw",          // useful while stabilizing
	"liveWatch": true
},
{
	"name": "J-Link (STM32H747 M7) — Attach",
	"type": "cortex-debug",
	"request": "attach",
	"servertype": "jlink",
	"executable": "${workspaceFolder}/build/elodin_aleph_bootloader/elodin_aleph_bootloader.elf",
	"gdbPath": "/home/austin/gcc-arm-none-eabi-10-2020-q4-major/bin/arm-none-eabi-gdb",

	"device": "STM32H747II",              // J-Link device name
	"interface": "swd",
	"serialNumber": "",                   // (optional) lock to a specific J-Link by S/N
	"runToEntryPoint": "Reset_Handler",   // or "main" after bring-up
	"svdFile": "",                        // (optional) path to H747 SVD, if you have it
	//"jlinkTargetInterfaceSpeed": 1000,    // start slow (kHz); raise once stable (e.g., 4000)

	// If J-Link is not on PATH, set serverpath to JLinkGDBServerCLExe:
	// "serverpath": "/opt/SEGGER/JLink/JLinkGDBServerCLExe",

	"cwd": "${workspaceFolder}",
	"showDevDebugOutput": "raw",          // useful while stabilizing
	"liveWatch": true
},
{
	"name": "J-Link (STM32H747 M7) — Attach Under Reset",
	"type": "cortex-debug",
	"request": "attach",
	"servertype": "jlink",
	"executable": "${workspaceFolder}/build/elodin_aleph_bootloader/elodin_aleph_bootloader.elf",
	"gdbPath": "/home/austin/gcc-arm-none-eabi-10-2020-q4-major/bin/arm-none-eabi-gdb",

	"device": "STM32H747II",              // J-Link device name
	"interface": "swd",
	"serialNumber": "",                   // (optional) lock to a specific J-Link by S/N
	"runToEntryPoint": "Reset_Handler",   // or "main" after bring-up
	"svdFile": "",                        // (optional) path to H747 SVD, if you have it
	//"jlinkTargetInterfaceSpeed": 1000,    // start slow (kHz); raise once stable (e.g., 4000)

	// If J-Link is not on PATH, set serverpath to JLinkGDBServerCLExe:
	// "serverpath": "/opt/SEGGER/JLink/JLinkGDBServerCLExe",

	"postAttachCommands": [
	"monitor reset",         // reset the core
	"monitor halt"           // and halt immediately
	],

	"cwd": "${workspaceFolder}",
	"showDevDebugOutput": "raw",          // useful while stabilizing
	"liveWatch": true
},
```
