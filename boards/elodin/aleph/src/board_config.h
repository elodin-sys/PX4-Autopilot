/****************************************************************************
 *
 *   Copyright (c) 2016-2022 PX4 Development Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/**
 * @file board_config.h
 *
 * PX4FMU-v6x internal definitions
 */

#pragma once

/****************************************************************************************************
 * Included Files
 ****************************************************************************************************/

#include <px4_platform_common/px4_config.h>
#include <nuttx/compiler.h>
#include <stdint.h>


#include <stm32_gpio.h>

/****************************************************************************************************
 * Definitions
 ****************************************************************************************************/

#undef TRACE_PINS

/* PX4IO connection configuration */

// #define BOARD_USES_PX4IO_VERSION       2
// #define PX4IO_SERIAL_DEVICE            "/dev/ttyS5"
// #define PX4IO_SERIAL_TX_GPIO           GPIO_USART6_TX
// #define PX4IO_SERIAL_RX_GPIO           GPIO_USART6_RX
// #define PX4IO_SERIAL_BASE              STM32_USART6_BASE
// #define PX4IO_SERIAL_VECTOR            STM32_IRQ_USART6
// #define PX4IO_SERIAL_TX_DMAMAP         DMAMAP_USART6_TX
// #define PX4IO_SERIAL_RX_DMAMAP         DMAMAP_USART6_RX
// #define PX4IO_SERIAL_RCC_REG           STM32_RCC_APB2ENR
// #define PX4IO_SERIAL_RCC_EN            RCC_APB2ENR_USART6EN
// #define PX4IO_SERIAL_CLOCK             STM32_PCLK2_FREQUENCY
// #define PX4IO_SERIAL_BITRATE           1500000               /* 1.5Mbps -> max rate for IO */

/* Configuration ************************************************************************************/

//#define BOARD_HAS_LTC44XX_VALIDS      2 //  N Bricks
//#define BOARD_HAS_USB_VALID           1 // LTC Has USB valid
//#define BOARD_HAS_NBAT_V              2d // 2 Digital Voltage
//#define BOARD_HAS_NBAT_I              2d // 2 Digital Current

/* PX4FMU GPIOs ***********************************************************************************/

/* Trace Clock and D0-D3 are available on the trace connector
 *
 * TRACECLK PE2  - Dedicated       - Trace Connector Pin 1
 * TRACED0  PE3  - nLED_RED        - Trace Connector Pin 3
 * TRACED1  PE4  - nLED_GREEN      - Trace Connector Pin 5
 * TRACED2  PE5  - nLED_BLUE       - Trace Connector Pin 7
 * TRACED3  PE6  - nARMED          - Trace Connector Pin 8

 */

/* LEDs are driven with push open drain to support Anode to 5V or 3.3V or used as TRACE0-2 */

#if !defined(TRACE_PINS)
#  define GPIO_nLED_RED        /* PE3 */  (GPIO_OUTPUT|GPIO_OPENDRAIN|GPIO_SPEED_50MHz|GPIO_OUTPUT_SET|GPIO_PORTE|GPIO_PIN3)
#  define GPIO_nLED_GREEN      /* PA15 */  (GPIO_OUTPUT|GPIO_OPENDRAIN|GPIO_SPEED_50MHz|GPIO_OUTPUT_SET|GPIO_PORTA|GPIO_PIN15)
#  define GPIO_nLED_BLUE       /* PI10 */  (GPIO_OUTPUT|GPIO_OPENDRAIN|GPIO_SPEED_50MHz|GPIO_OUTPUT_SET|GPIO_PORTI|GPIO_PIN0)

#  define BOARD_HAS_CONTROL_STATUS_LEDS      1
#  define BOARD_OVERLOAD_LED     LED_RED
#  define BOARD_ARMED_STATE_LED  LED_BLUE

#else

#  define GPIO_TRACECLK1 (GPIO_TRACECLK |GPIO_PULLUP|GPIO_SPEED_100MHz|GPIO_PUSHPULL)  //(GPIO_ALT|GPIO_AF0|GPIO_PORTE|GPIO_PIN2)
#  define GPIO_TRACED0   (GPIO_TRACED0_2|GPIO_PULLUP|GPIO_SPEED_100MHz|GPIO_PUSHPULL)  //(GPIO_ALT|GPIO_AF0|GPIO_PORTE|GPIO_PIN3)
#  define GPIO_TRACED1   (GPIO_TRACED1_2|GPIO_PULLUP|GPIO_SPEED_100MHz|GPIO_PUSHPULL)  //(GPIO_ALT|GPIO_AF0|GPIO_PORTE|GPIO_PIN4)
#  define GPIO_TRACED2   (GPIO_TRACED2_2|GPIO_PULLUP|GPIO_SPEED_100MHz|GPIO_PUSHPULL)  //(GPIO_ALT|GPIO_AF0|GPIO_PORTE|GPIO_PIN5)
#  define GPIO_TRACED3   (GPIO_TRACED3_2|GPIO_PULLUP|GPIO_SPEED_100MHz|GPIO_PUSHPULL)  //(GPIO_ALT|GPIO_AF0|GPIO_PORTE|GPIO_PIN6)
//#define GPIO_TRACESWO                        //(GPIO_ALT|GPIO_AF0|GPIO_PORTB|GPIO_PIN3)

#  undef  BOARD_HAS_CONTROL_STATUS_LEDS
#  undef  BOARD_OVERLOAD_LED
#  undef  BOARD_ARMED_STATE_LED

#  define GPIO_nLED_RED    GPIO_TRACED0
#  define GPIO_nLED_GREEN  GPIO_TRACED1
#  define GPIO_nLED_BLUE   GPIO_TRACED2
#  define GPIO_nARMED      GPIO_TRACED3
#  define GPIO_nARMED_INIT GPIO_TRACED3
#endif


/* Devices on the onboard buses.
 *
 * Note that these are unshifted addresses.
 */
//#define BOARD_MTD_NUM_EEPROM        2 /* MTD: base_eeprom, imu_eeprom*/

//#define PX4_I2C_OBDEV_SE050         0x48

/*
 * ADC channels
 *
 * These are the channel numbers of the ADCs of the microcontroller that
 * can be used by the Px4 Firmware in the adc driver
 */

/* ADC defines to be used in sensors.cpp to read from a particular channel */

#define ADC1_CH(n)                  (n)

/* N.B. there is no offset mapping needed for ADC3 because */
#define ADC3_CH(n)                  (n)

#define PC4_ADC_GPIO \
	/* PA0 - C */  GPIO_ADC12_INP0, 	\
	/* PC2 - C */  GPIO_ADC3_INP0, 		\
	/* PC3 - C */  GPIO_ADC3_INP1, 		\
	/* PH3 */      GPIO_ADC3_INP14, 	\
	/* PH4 */      GPIO_ADC3_INP15

#define ADC_VMON_BAT 			ADC1_CH(0)
#define ADC_VMON_VIN 			ADC3_CH(0)
#define ADC_IMON_5V_AUX 		ADC3_CH(1)
// See board_hw_rev_ver.c for details, these seem to be tied to HW versioning via hardware
// I just allocated some unused pins as ADC inputs to satisfy the build
#define ADC_HW_VER_SENSE_CHANNEL 	ADC3_CH(14)
#define ADC_HW_REV_SENSE_CHANNEL 	ADC3_CH(15)


#define ADC_CHANNELS \
	((1 << ADC_VMON_BAT) | \
	 (1 << ADC_VMON_VIN)) | \
	 (1 << ADC_IMON_5V_AUX)

/* HW has to large of R termination on ADC todo:change when HW value is chosen */

#define HW_REV_VER_ADC_BASE STM32_ADC3_BASE

#define SYSTEM_ADC_BASE STM32_ADC1_BASE

/* HW has to large of R termination on ADC todo:change when HW value is chosen */
#define BOARD_ADC_OPEN_CIRCUIT_V     (5.6f)

/* HW Version and Revision drive signals Default to 1 to detect */
#define BOARD_HAS_HW_SPLIT_VERSIONING

// ABM - Doesn't seem relevant to the Aleph board, G0 is not hooked up.
#define GPIO_HW_VER_REV_DRIVE  /* PG0 */ (GPIO_OUTPUT|GPIO_PUSHPULL|GPIO_SPEED_2MHz|GPIO_OUTPUT_SET|GPIO_PORTG|GPIO_PIN0)
#define GPIO_HW_REV_SENSE      /* PH4 */  GPIO_ADC3_INP15
#define GPIO_HW_VER_SENSE      /* PH3 */  GPIO_ADC3_INP14
#define HW_INFO_INIT_PREFIX    "V6X"

//#define UAVCAN_NUM_IFACES_RUNTIME  1

/* PWM
 */
#define DIRECT_PWM_OUTPUT_CHANNELS   16

// This pin enables 5V aux power to the peripheral, like the jumper 5V supplies
#define GPIO_VDD_5V_PERIPH_EN          	/* PH15  */ (GPIO_OUTPUT|GPIO_PUSHPULL|GPIO_SPEED_2MHz|GPIO_OUTPUT_SET|GPIO_PORTH|GPIO_PIN15)

// Added for Aleph board to enable 5V boost
#define VDD_5V_PERIPH_EN(on_true)          	px4_arch_gpiowrite(GPIO_VDD_5V_PERIPH_EN, on_true)

// Updated this to match schematic
/* Spare GPIO */
#define MCU_GPIO_0    			/* PG3  */  (GPIO_INPUT|GPIO_PULLUP|GPIO_PORTG|GPIO_PIN3)
#define MCU_GPIO_1    			/* PG1  */  (GPIO_INPUT|GPIO_PULLUP|GPIO_PORTG|GPIO_PIN1)
#define MCU_GPIO_2    			/* PK2  */  (GPIO_INPUT|GPIO_PULLUP|GPIO_PORTK|GPIO_PIN2)
#define MCU_GPIO_3    			/* PG4  */  (GPIO_INPUT|GPIO_PULLUP|GPIO_PORTG|GPIO_PIN4)
#define MCU_GPIO_4    			/* PG5  */  (GPIO_INPUT|GPIO_PULLUP|GPIO_PORTG|GPIO_PIN5)
#define MCU_GPIO_5    			/* PG6  */  (GPIO_INPUT|GPIO_PULLUP|GPIO_PORTG|GPIO_PIN6)
#define MCU_GPIO_6    			/* PG8  */  (GPIO_INPUT|GPIO_PULLUP|GPIO_PORTG|GPIO_PIN8)
#define MCU_GPIO_7    			/* PG7  */  (GPIO_INPUT|GPIO_PULLUP|GPIO_PORTG|GPIO_PIN7)

/* NFC GPIO */
// ABM - This pin is an interrupt input pin on the schematic, but not denoted as NFC
#define GPIO_NFC_GPIO                   /* PC0 */ (GPIO_INPUT|GPIO_FLOAT|GPIO_EXTI|GPIO_PORTC|GPIO_PIN0)


/* USB OTG FS
 *
 * PA9  OTG_FS_VBUS VBUS sensing
 */

// We don't have a VBUS sense pin, so just using an unmapped pin since it needs to be defined
#define GPIO_OTGFS_VBUS         /* PB0 */ (GPIO_INPUT|GPIO_PULLUP|GPIO_SPEED_100MHz|GPIO_PORTB|GPIO_PIN0)

/* High-resolution timer */
#define HRT_TIMER               3  /* use timer3 for the HRT */
#define HRT_TIMER_CHANNEL       3  /* use capture/compare channel 3 */

#define HRT_PPM_CHANNEL         /* T8C1 */  1  /* use capture/compare channel 1 */
#define GPIO_PPM_IN             /* PI5 T8C1 */ GPIO_TIM3_CH1IN_2

/* RC Serial port */

#define RC_SERIAL_PORT                     "/dev/ttyS5"
#define RC_SERIAL_SINGLEWIRE

/* Input Capture Channels. */
#define INPUT_CAP1_TIMER                  1
#define INPUT_CAP1_CHANNEL     /* T1C2 */ 2
#define GPIO_INPUT_CAP1        		GPIO_TIM1_CH2IN

/* PWM input driver. Use FMU AUX5 pins attached to timer4 channel 2 */
#define PWMIN_TIMER                       4
#define PWMIN_TIMER_CHANNEL    /* T4C2 */ 2
#define GPIO_PWM_IN            		GPIO_TIM4_CH2IN_2


// /* Safety Switch is HW version dependent on having an PX4IO
//  * So we init to a benign state with the _INIT definition
//  * and provide the the non _INIT one for the driver to make a run time
//  * decision to use it.
//  */
// // Updated 7/31/2025 - Was mapped to PD10
// #define GPIO_nSAFETY_SWITCH_LED_OUT_INIT   /* PI9 */ (GPIO_INPUT|GPIO_FLOAT|GPIO_PORTI|GPIO_PIN9)
// #define GPIO_nSAFETY_SWITCH_LED_OUT        /* PI9 */ (GPIO_OUTPUT|GPIO_PUSHPULL|GPIO_SPEED_2MHz|GPIO_OUTPUT_SET|GPIO_PORTI|GPIO_PIN9)

// /* Enable the FMU to control it if there is no px4io fixme:This should be BOARD_SAFETY_LED(__ontrue) */
// #define GPIO_LED_SAFETY GPIO_nSAFETY_SWITCH_LED_OUT

// #define GPIO_SAFETY_SWITCH_IN              /* PF5 */ (GPIO_INPUT|GPIO_PULLUP|GPIO_PORTF|GPIO_PIN5)
// /* Enable the FMU to use the switch it if there is no px4io fixme:This should be BOARD_SAFTY_BUTTON() */
// #define GPIO_BTN_SAFETY GPIO_SAFETY_SWITCH_IN /* Enable the FMU to control it if there is no px4io */

// /* Power switch controls ******************************************************/

// #define SPEKTRUM_POWER(_on_true)           VDD_3V3_SPEKTRUM_POWER_EN(_on_true)

/*
 * FMUv6X has a separate RC_IN
 *
 * GPIO PPM_IN on PI5 T8CH1
 * SPEKTRUM_RX (it's TX or RX in Bind) on UART6 PC7
 *   Inversion is possible in the UART and can drive  GPIO PPM_IN as an output
 */

// #define GPIO_PPM_IN_AS_OUT             (GPIO_OUTPUT|GPIO_PUSHPULL|GPIO_SPEED_2MHz|GPIO_OUTPUT_SET|GPIO_PORTI|GPIO_PIN5)
// #define SPEKTRUM_RX_AS_GPIO_OUTPUT()   px4_arch_configgpio(GPIO_PPM_IN_AS_OUT)
// #define SPEKTRUM_RX_AS_UART()          /* Can be left as uart */
// #define SPEKTRUM_OUT(_one_true)        px4_arch_gpiowrite(GPIO_PPM_IN_AS_OUT, (_one_true))

#define SDIO_SLOTNO                    0  /* Only one slot */
#define SDIO_MINOR                     0

/* SD card bringup does not work if performed on the IDLE thread because it
 * will cause waiting.  Use either:
 *
 *  CONFIG_BOARDCTL=y, OR
 *  CONFIG_BOARD_INITIALIZE=y && CONFIG_BOARD_INITTHREAD=y
 */

#if defined(CONFIG_BOARD_INITIALIZE) && !defined(CONFIG_BOARDCTL) && \
   !defined(CONFIG_BOARD_INITTHREAD)
#  warning SDIO initialization cannot be perfomed on the IDLE thread
#endif

/* By Providing BOARD_ADC_USB_CONNECTED (using the px4_arch abstraction)
 * this board support the ADC system_power interface, and therefore
 * provides the true logic GPIO BOARD_ADC_xxxx macros.
 */
//#define BOARD_ADC_USB_CONNECTED (px4_arch_gpioread(GPIO_OTGFS_VBUS))
//#define BOARD_ADC_USB_VALID     (!px4_arch_gpioread(GPIO_OTGFS_VBUS))
#define BOARD_ADC_USB_CONNECTED 	(px4_arch_gpioread(GPIO_OTGFS_VBUS))
#define BOARD_ADC_USB_VALID     	(!px4_arch_gpioread(GPIO_OTGFS_VBUS))
// #define BOARD_ADC_USB_VALID     (!px4_arch_gpioread(GPIO_nVDD_USB_VALID))

/* FMUv6X never powers off the Servo rail */

#define BOARD_ADC_SERVO_VALID     (1)

#if !defined(BOARD_HAS_LTC44XX_VALIDS) || BOARD_HAS_LTC44XX_VALIDS == 0
#  define BOARD_ADC_BRICK1_VALID  (1)
#  define BOARD_ADC_BRICK2_VALID  (0)
#elif BOARD_HAS_LTC44XX_VALIDS == 1
#  define BOARD_ADC_BRICK1_VALID  (!px4_arch_gpioread(GPIO_nVDD_BRICK1_VALID))
#  define BOARD_ADC_BRICK2_VALID  (0)
#elif BOARD_HAS_LTC44XX_VALIDS == 2
#  define BOARD_ADC_BRICK1_VALID  (!px4_arch_gpioread(GPIO_nVDD_BRICK1_VALID))
#  define BOARD_ADC_BRICK2_VALID  (!px4_arch_gpioread(GPIO_nVDD_BRICK2_VALID))
#elif BOARD_HAS_LTC44XX_VALIDS == 3
#  define BOARD_ADC_BRICK1_VALID  (!px4_arch_gpioread(GPIO_nVDD_BRICK1_VALID))
#  define BOARD_ADC_BRICK2_VALID  (!px4_arch_gpioread(GPIO_nVDD_BRICK2_VALID))
#  define BOARD_ADC_BRICK3_VALID  (!px4_arch_gpioread(GPIO_nVDD_BRICK3_VALID))
#elif BOARD_HAS_LTC44XX_VALIDS == 4
#  define BOARD_ADC_BRICK1_VALID  (!px4_arch_gpioread(GPIO_nVDD_BRICK1_VALID))
#  define BOARD_ADC_BRICK2_VALID  (!px4_arch_gpioread(GPIO_nVDD_BRICK2_VALID))
#  define BOARD_ADC_BRICK3_VALID  (!px4_arch_gpioread(GPIO_nVDD_BRICK3_VALID))
#  define BOARD_ADC_BRICK4_VALID  (!px4_arch_gpioread(GPIO_nVDD_BRICK4_VALID))
#else
#  error Unsupported BOARD_HAS_LTC44XX_VALIDS value
#endif


/* This board provides a DMA pool and APIs */
#define BOARD_DMA_ALLOC_POOL_SIZE 5120

/* This board provides the board_on_reset interface */

#define BOARD_HAS_ON_RESET 1

// ABM - Added unused pin for GPIO TRACE
#define GPIO_TRACE (GPIO_OUTPUT|GPIO_OUTPUT_SET|GPIO_PORTE|GPIO_PIN7)

// I'm not sure we need to add CAN pins here, the old code did
#define PX4_GPIO_INIT_LIST { \
	GPIO_VDD_5V_PERIPH_EN, \
	MCU_GPIO_0, \
	MCU_GPIO_1, \
	MCU_GPIO_2, \
	MCU_GPIO_3, \
	MCU_GPIO_4, \
	MCU_GPIO_5, \
	MCU_GPIO_6, \
	MCU_GPIO_7, \
}

#define BOARD_ENABLE_CONSOLE_BUFFER

#define PX4_SPI_BUS_RAMTRON 4

#define BOARD_NUM_IO_TIMERS 4

__BEGIN_DECLS

/****************************************************************************************************
 * Public Types
 ****************************************************************************************************/

/****************************************************************************************************
 * Public data
 ****************************************************************************************************/

#ifndef __ASSEMBLY__

/****************************************************************************************************
 * Public Functions
 ****************************************************************************************************/

/****************************************************************************
 * Name: stm32_sdio_initialize
 *
 * Description:
 *   Initialize SDIO-based MMC/SD card support
 *
 ****************************************************************************/

int stm32_sdio_initialize(void);

/****************************************************************************************************
 * Name: stm32_spiinitialize
 *
 * Description:
 *   Called to configure SPI chip select GPIO pins for the PX4FMU board.
 *
 ****************************************************************************************************/

extern void stm32_spiinitialize(void);

extern void stm32_usbinitialize(void);

extern void board_peripheral_reset(int ms);

#include <px4_platform_common/board_common.h>

#endif /* __ASSEMBLY__ */

__END_DECLS
