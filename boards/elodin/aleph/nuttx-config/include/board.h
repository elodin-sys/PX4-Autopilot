/************************************************************************************
 * nuttx-configs/px4_fmu-v6x/include/board.h
 *
 *   Copyright (C) 2016-2019 Gregory Nutt. All rights reserved.
 *   Authors: David Sidrane <david.sidrane@nscdg.com>
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
 * 3. Neither the name NuttX nor the names of its contributors may be
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
 ************************************************************************************/
#ifndef __NUTTX_CONFIG_PX4_FMU_V6X_INCLUDE_BOARD_H
#define __NUTTX_CONFIG_PX4_FMU_V6X_INCLUDE_BOARD_H

/************************************************************************************
 * Included Files
 ************************************************************************************/

#include "board_dma_map.h"

#include <nuttx/config.h>

#ifndef __ASSEMBLY__
# include <stdint.h>
#endif

#include "stm32_rcc.h"
#include "stm32_sdmmc.h"

/************************************************************************************
 * Pre-processor Definitions
 ************************************************************************************/

/* Clocking *************************************************************************/
/* The px4_fmu-v6X  board provides the following clock sources:
 *
 *   X1: 16 MHz crystal for HSE
 *
 * So we have these clock source available within the STM32
 *
 *   HSI: 16 MHz RC factory-trimmed
 *   HSE: 16 MHz crystal for HSE
 */

#define STM32_BOARD_XTAL        24000000ul

#define STM32_HSI_FREQUENCY     16000000ul
#define STM32_LSI_FREQUENCY     32000
#define STM32_HSE_FREQUENCY     STM32_BOARD_XTAL
#define STM32_LSE_FREQUENCY     32768

/*

Main PLL Configuration comes from the working Rust code. I also
highly recommend the STM32CubeMx IDE to visualize and help configure
the clocks.

pwr.cr3.modify(|_, w| {
        // Disable bypass, SMPS
        w.bypass().clear_bit();
        w.sden().clear_bit();
        // Enable LDO
        w.ldoen().set_bit()
    });

    let clock_cfg = clocks::Clocks {
        pll_src: clocks::PllSrc::Hse(HSE_FREQ.to_Hz()), // 24 MHz
        pll1: clocks::PllCfg {
            enabled: true,
            pllp_en: true,
            pllq_en: true,
            pllr_en: false,
            divm: 3,   // pll_input_speed = 8 MHz
            divn: 100, // vco_speed = 800 MHz
            divp: 2,   // sysclk = 400 MHz
            divq: 8,   // 100 MHz
            ..Default::default()
        },
        pll2: clocks::PllCfg {
            enabled: true,
            pllp_en: true,
            pllq_en: false,
            pllr_en: false,
            divm: 3,  // pll_input_speed = 8 MHz (24 MHz / 3)
            divn: 25, // vco_speed = 200 MHz (8 MHz * 25)
            divp: 8,  // pll2_p_ck = 25 MHz (200 MHz / 8) - max for 16-bit ADC
            ..Default::default()
        },
        pll3: clocks::PllCfg {
            enabled: true,
            pllq_en: true,
            pllr_en: false,
            divm: 3,  // pll_input_speed = 8 MHz (24 MHz / 3)
            divn: 12, // vco_speed = 96 MHz (8 MHz * 12)
            divq: 2,  // pll3_q_ck = 48 MHz (96 MHz / 2) - for USB
            ..Default::default()
        },
        hsi48_on: false,
        input_src: clocks::InputSrc::Pll1, // 400 MHz (sysclk)
        d1_core_prescaler: clocks::HclkPrescaler::Div1, // 400 MHz (M7 core)
        hclk_prescaler: clocks::HclkPrescaler::Div2, // 200 MHz (hclk, M4 core)
        d2_prescaler1: clocks::ApbPrescaler::Div2, // 100 MHz
        d2_prescaler2: clocks::ApbPrescaler::Div2, // 100 MHz
        d3_prescaler: clocks::ApbPrescaler::Div2, // 100 MHz
        vos_range: clocks::VosRange::VOS1,
        can_src: clocks::CanSrc::Hse,
        usb_src: clocks::UsbSrc::Pll3Q,
        hse_bypass: true,
        ..Default::default()
    };
*/

#undef CONFIG_STM32H7_HAVE_SMPS
#define STM32_PWR_CR3_SDEN                  (1 << 2)  /* Bit 2: SMPS step-down converter enable */
#define PWR_D3CR_VOS_SCALE1                 (0x3 << 14)
#define PWR_CSR1_ACTVOSRDY                  (1 << 13)

#define STM32_BOARD_USEHSE

#define STM32_PLLCFG_PLLSRC      RCC_PLLCKSELR_PLLSRC_HSE

#define STM32_PLLCFG_PLL1CFG    (RCC_PLLCFGR_PLL1VCOSEL_WIDE | \
				 RCC_PLLCFGR_PLL1RGE_4_8_MHZ | \
				 RCC_PLLCFGR_DIVP1EN | \
				 RCC_PLLCFGR_DIVQ1EN )
#define STM32_PLLCFG_PLL1M       RCC_PLLCKSELR_DIVM1(3)
#define STM32_PLLCFG_PLL1N       RCC_PLL1DIVR_N1(100)
#define STM32_PLLCFG_PLL1P       RCC_PLL1DIVR_P1(2)
#define STM32_PLLCFG_PLL1Q       RCC_PLL1DIVR_Q1(8)
#define STM32_PLLCFG_PLL1R       RCC_PLL1DIVR_R1(8) // Not used, but must be defined

#define STM32_VCO1_FREQUENCY     ((STM32_HSE_FREQUENCY / 3) * 100) 	// 800 MHz
#define STM32_PLL1P_FREQUENCY    (STM32_VCO1_FREQUENCY / 2)	 	// 400 MHz
#define STM32_PLL1Q_FREQUENCY    (STM32_VCO1_FREQUENCY / 8)		// 100 MHz
#define STM32_PLL1R_FREQUENCY    (STM32_VCO1_FREQUENCY / 8)		// 100 MHz

/* PLL2 */

#define STM32_PLLCFG_PLL2CFG     (RCC_PLLCFGR_PLL2VCOSEL_WIDE | \
				  RCC_PLLCFGR_PLL2RGE_4_8_MHZ | \
				  RCC_PLLCFGR_DIVP2EN)
#define STM32_PLLCFG_PLL2M       RCC_PLLCKSELR_DIVM2(3)
#define STM32_PLLCFG_PLL2N       RCC_PLL2DIVR_N2(25)
#define STM32_PLLCFG_PLL2P       RCC_PLL2DIVR_P2(8)
#define STM32_PLLCFG_PLL2Q       RCC_PLL2DIVR_Q2(2) // Not used, but must be defined
#define STM32_PLLCFG_PLL2R       RCC_PLL2DIVR_R2(2) // Not used, but must be defined

#define STM32_VCO2_FREQUENCY     ((STM32_HSE_FREQUENCY / 3) * 25) // 200 MHz
#define STM32_PLL2P_FREQUENCY    (STM32_VCO2_FREQUENCY / 2)	  // 100 MHz
// #define STM32_PLL2Q_FREQUENCY    (STM32_VCO2_FREQUENCY / 2)
// #define STM32_PLL2R_FREQUENCY    (STM32_VCO2_FREQUENCY / 2)

/* PLL3 */

#define STM32_PLLCFG_PLL3CFG    (RCC_PLLCFGR_PLL3VCOSEL_WIDE | \
				 RCC_PLLCFGR_PLL3RGE_4_8_MHZ | \
				 RCC_PLLCFGR_DIVQ3EN)
#define STM32_PLLCFG_PLL3M      RCC_PLLCKSELR_DIVM3(3)
#define STM32_PLLCFG_PLL3N      RCC_PLL3DIVR_N3(12)
#define STM32_PLLCFG_PLL3P      RCC_PLL3DIVR_P3(2) // Not used, but must be defined
#define STM32_PLLCFG_PLL3Q      RCC_PLL3DIVR_Q3(2)
#define STM32_PLLCFG_PLL3R      RCC_PLL3DIVR_R3(2) // Not used, but must be defined

#define STM32_VCO3_FREQUENCY    ((STM32_HSE_FREQUENCY / 3) * 12) 	// 96 MHz
//#define STM32_PLL3P_FREQUENCY   (STM32_VCO3_FREQUENCY / 2)
#define STM32_PLL3Q_FREQUENCY   (STM32_VCO3_FREQUENCY / 4)		// 48 MHz
//#define STM32_PLL3R_FREQUENCY   (STM32_VCO3_FREQUENCY / 2)

/*

The D1 Clock Pre-scalar is set so that the M7 CPU clock is 400MHz and the
M4 CPU clock is set the 200MHz.
D2, D3 clocks are set to 100MHz
*/

#define STM32_RCC_D1CFGR_D1CPRE  (RCC_D1CFGR_D1CPRE_SYSCLK)
#define STM32_SYSCLK_FREQUENCY   (STM32_PLL1P_FREQUENCY)
#define STM32_CPUCLK_FREQUENCY   (STM32_SYSCLK_FREQUENCY / 1)		// 400 MHz

/* Configure Clock Assignments */

#define STM32_RCC_D1CFGR_HPRE   RCC_D1CFGR_HPRE_SYSCLKd2        /* HCLK  = SYSCLK / 2 */
#define STM32_ACLK_FREQUENCY    (STM32_CPUCLK_FREQUENCY / 2)    /* ACLK in D1, HCLK3 in D1 */
#define STM32_HCLK_FREQUENCY    (STM32_CPUCLK_FREQUENCY / 2)    /* HCLK in D2, HCLK4 in D3 */
#define STM32_BOARD_HCLK        STM32_HCLK_FREQUENCY            /* same as above, to satisfy compiler */

/* APB1 clock (PCLK1) is HCLK/2 (120 MHz) */

#define STM32_RCC_D2CFGR_D2PPRE1  RCC_D2CFGR_D2PPRE1_HCLKd2       /* PCLK1 = HCLK / 2 */
#define STM32_PCLK1_FREQUENCY     (STM32_HCLK_FREQUENCY/2)

/* APB2 clock (PCLK2) is HCLK/2 (120 MHz) */

#define STM32_RCC_D2CFGR_D2PPRE2  RCC_D2CFGR_D2PPRE2_HCLKd2       /* PCLK2 = HCLK / 2 */
#define STM32_PCLK2_FREQUENCY     (STM32_HCLK_FREQUENCY/2)

/* APB3 clock (PCLK3) is HCLK/2 (120 MHz) */

#define STM32_RCC_D1CFGR_D1PPRE   RCC_D1CFGR_D1PPRE_HCLKd2        /* PCLK3 = HCLK / 2 */
#define STM32_PCLK3_FREQUENCY     (STM32_HCLK_FREQUENCY/2)

/* APB4 clock (PCLK4) is HCLK/4 (120 MHz) */

#define STM32_RCC_D3CFGR_D3PPRE   RCC_D3CFGR_D3PPRE_HCLKd2       /* PCLK4 = HCLK / 2 */
#define STM32_PCLK4_FREQUENCY     (STM32_HCLK_FREQUENCY/2)

// Other Rust settings
#define STM32_HSEBYP_ENABLE

/* Timer clock frequencies */

/* Timers driven from APB1 will be twice PCLK1 */

#define STM32_APB1_TIM2_CLKIN   (2*STM32_PCLK1_FREQUENCY)
#define STM32_APB1_TIM3_CLKIN   (2*STM32_PCLK1_FREQUENCY)
#define STM32_APB1_TIM4_CLKIN   (2*STM32_PCLK1_FREQUENCY)
#define STM32_APB1_TIM5_CLKIN   (2*STM32_PCLK1_FREQUENCY)
#define STM32_APB1_TIM6_CLKIN   (2*STM32_PCLK1_FREQUENCY)
#define STM32_APB1_TIM7_CLKIN   (2*STM32_PCLK1_FREQUENCY)
#define STM32_APB1_TIM12_CLKIN  (2*STM32_PCLK1_FREQUENCY)
#define STM32_APB1_TIM13_CLKIN  (2*STM32_PCLK1_FREQUENCY)
#define STM32_APB1_TIM14_CLKIN  (2*STM32_PCLK1_FREQUENCY)

/* Timers driven from APB2 will be twice PCLK2 */

#define STM32_APB2_TIM1_CLKIN   (2*STM32_PCLK2_FREQUENCY)
#define STM32_APB2_TIM8_CLKIN   (2*STM32_PCLK2_FREQUENCY)
#define STM32_APB2_TIM15_CLKIN  (2*STM32_PCLK2_FREQUENCY)
#define STM32_APB2_TIM16_CLKIN  (2*STM32_PCLK2_FREQUENCY)
#define STM32_APB2_TIM17_CLKIN  (2*STM32_PCLK2_FREQUENCY)

/* Kernel Clock Configuration
 *
 * ABM - As far as I can tell, the clock domains in Rust default to the reset values.
 *
 * Note: look at Table 54 in ST Manual
 */

/* I2C123 clock source */

#define STM32_RCC_D2CCIP2R_I2C123SRC RCC_D2CCIP2R_I2C123SEL_HSI

/* I2C4 clock source */

#define STM32_RCC_D3CCIPR_I2C4SRC    RCC_D3CCIPR_I2C4SEL_HSI

/* SPI123 clock source */

#define STM32_RCC_D2CCIP1R_SPI123SRC RCC_D2CCIP1R_SPI123SEL_PLL1

/* SPI45 clock source */

#define STM32_RCC_D2CCIP1R_SPI45SRC  RCC_D2CCIP1R_SPI45SEL_APB

/* SPI6 clock source */

#define STM32_RCC_D3CCIPR_SPI6SRC    RCC_D3CCIPR_SPI6SEL_PLL2

/* USB 1 and 2 clock source */

#define STM32_RCC_D2CCIP2R_USBSRC    RCC_D2CCIP2R_USBSEL_PLL3

/* ADC 1 2 3 clock source */

#define STM32_RCC_D3CCIPR_ADCSRC     RCC_D3CCIPR_ADCSEL_PLL2

/* UART clock selection */
/* reset to default to overwrite any changes done by any bootloader */

#define STM32_RCC_D2CCIP2R_USART234578_SEL RCC_D2CCIP2R_USART234578SEL_RCC
#define STM32_RCC_D2CCIP2R_USART16_SEL     RCC_D2CCIP2R_USART16SEL_RCC

/* USB Clock Source*/
#define STM32_RCC_D2CCIP2R_USBSEL_PLL3 	RCC_D2CCIP2R_USBSEL_PLL3

/* FDCAN 1 2 clock source */

#define STM32_RCC_D2CCIP1R_FDCANSEL  RCC_D2CCIP1R_FDCANSEL_HSE   /* FDCAN 1 2 clock source */

#define STM32_FDCANCLK               STM32_HSE_FREQUENCY

/* FLASH wait states
 *
 *  ------------ ---------- -----------
 *  Vcore        MAX ACLK   WAIT STATES
 *  ------------ ---------- -----------
 *  1.15-1.26 V     70 MHz    0
 *  (VOS1 level)   140 MHz    1
 *                 210 MHz    2
 *  1.05-1.15 V     55 MHz    0
 *  (VOS2 level)   110 MHz    1
 *                 165 MHz    2
 *                 220 MHz    3
 *  0.95-1.05 V     45 MHz    0
 *  (VOS3 level)    90 MHz    1
 *                 135 MHz    2
 *                 180 MHz    3
 *                 225 MHz    4
 *  ------------ ---------- -----------
 */

#define BOARD_FLASH_WAITSTATES 2

/* SDMMC definitions ********************************************************/

/* Init 400kHz, freq = PLL1Q/(2*div)  div =  PLL1Q/(2*freq) */

#define STM32_SDMMC_INIT_CLKDIV     (125 << STM32_SDMMC_CLKCR_CLKDIV_SHIFT)

/* 25 MHz Max for now, 25 mHZ = PLL1Q/(2*div), div =  PLL1Q/(2*freq)
 * div = 4.8 = 240 / 50, So round up to 5 for default speed 24 MB/s
 */

/* 25 MHz Max for now, 25 mHZ = PLL1Q/(2*div), div = PLL1Q/(2*freq)
 * div = 100 / (2*25)
 */
#define STM32_SDMMC_MMCXFR_CLKDIV   (2 << STM32_SDMMC_CLKCR_CLKDIV_SHIFT)
#define STM32_SDMMC_SDXFR_CLKDIV    (2 << STM32_SDMMC_CLKCR_CLKDIV_SHIFT)

#define STM32_SDMMC_CLKCR_EDGE      STM32_SDMMC_CLKCR_NEGEDGE

// #if defined(CONFIG_STM32H7_SDMMC_XDMA) || defined(CONFIG_STM32H7_SDMMC_IDMA)
// #  define STM32_SDMMC_MMCXFR_CLKDIV   (5 << STM32_SDMMC_CLKCR_CLKDIV_SHIFT)
// #else
// #  define STM32_SDMMC_MMCXFR_CLKDIV   (100 << STM32_SDMMC_CLKCR_CLKDIV_SHIFT)
// #endif
// #if defined(CONFIG_STM32H7_SDMMC_XDMA) || defined(CONFIG_STM32H7_SDMMC_IDMA)
// #  define STM32_SDMMC_SDXFR_CLKDIV    (5 << STM32_SDMMC_CLKCR_CLKDIV_SHIFT)
// #else
// #  define STM32_SDMMC_SDXFR_CLKDIV    (100 << STM32_SDMMC_CLKCR_CLKDIV_SHIFT)
// #endif

// #define STM32_SDMMC_CLKCR_EDGE      STM32_SDMMC_CLKCR_NEGEDGE

/* LED definitions ******************************************************************/
/* The PX4 FMUV6X board has three, LED_GREEN a Green LED, LED_BLUE a Blue LED and
 * LED_RED a Red LED, that can be controlled by software.
 *
 * If CONFIG_ARCH_LEDS is not defined, then the user can control the LEDs in any way.
 * The following definitions are used to access individual LEDs.
 */

/* LED index values for use with board_userled() */

/* LED definitions ******************************************************************/
/* The px4_fmu-v6x board has three, LED_GREEN a Green LED, LED_BLUE a Blue LED and
 * LED_RED a Red LED, that can be controlled by software.
 *
 * If CONFIG_ARCH_LEDS is not defined, then the user can control the LEDs in any way.
 * The following definitions are used to access individual LEDs.
 */

/* LED index values for use with board_userled() */

#define BOARD_LED1        0
#define BOARD_LED2        1
#define BOARD_LED3        2
#define BOARD_NLEDS       3

#define BOARD_LED_RED     BOARD_LED1
#define BOARD_LED_GREEN   BOARD_LED2
#define BOARD_LED_BLUE    BOARD_LED3

/* LED bits for use with board_userled_all() */

#define BOARD_LED1_BIT    (1 << BOARD_LED1)
#define BOARD_LED2_BIT    (1 << BOARD_LED2)
#define BOARD_LED3_BIT    (1 << BOARD_LED3)

/* If CONFIG_ARCH_LEDS is defined, the usage by the board port is defined in
 * include/board.h and src/stm32_leds.c. The LEDs are used to encode OS-related
 * events as follows:
 *
 *
 *   SYMBOL                     Meaning                      LED state
 *                                                        Red   Green Blue
 *   ----------------------  --------------------------  ------ ------ ----*/

#define LED_STARTED        0 /* NuttX has been started   OFF    OFF   OFF  */
#define LED_HEAPALLOCATE   1 /* Heap has been allocated  OFF    OFF   ON   */
#define LED_IRQSENABLED    2 /* Interrupts enabled       OFF    ON    OFF  */
#define LED_STACKCREATED   3 /* Idle stack created       OFF    ON    ON   */
#define LED_INIRQ          4 /* In an interrupt          N/C    N/C   GLOW */
#define LED_SIGNAL         5 /* In a signal handler      N/C    GLOW  N/C  */
#define LED_ASSERTION      6 /* An assertion failed      GLOW   N/C   GLOW */
#define LED_PANIC          7 /* The system has crashed   Blink  OFF   N/C  */
#define LED_IDLE           8 /* MCU is is sleep mode     ON     OFF   OFF  */

/* Thus if the Green LED is statically on, NuttX has successfully booted and
 * is, apparently, running normally.  If the Red LED is flashing at
 * approximately 2Hz, then a fatal error has been detected and the system
 * has halted.
 */

/* Alternate function pin selections ************************************************/

// UART - Updated 7/31/2025 - See stm32h7x3xx_pinmap.h and schematic for details
// None of the UART on the schematic have RTS and CTS lines connected
#define GPIO_USART1_RX   GPIO_USART1_RX_2      /* PA10 */
#define GPIO_USART1_TX   GPIO_USART1_TX_2      /* PA9 */

#define GPIO_USART2_RX   GPIO_USART2_RX_2       /* PD6   */
#define GPIO_USART2_TX   GPIO_USART2_TX_2       /* PD5   */

#define GPIO_USART3_RX   GPIO_USART3_RX_3   /* PD9   */
#define GPIO_USART3_TX   GPIO_USART3_TX_3   /* PD8   */

#define GPIO_USART6_RX    GPIO_USART6_RX_1    /* PC7  */
#define GPIO_USART6_TX    GPIO_USART6_TX_1    /* PC6 */

#define GPIO_UART7_RX    GPIO_UART7_RX_4    /* PF6  */
#define GPIO_UART7_TX    GPIO_UART7_TX_4    /* PF7  */

#define GPIO_UART8_RX    GPIO_UART8_RX_1    /* PE0 */
#define GPIO_UART8_TX    GPIO_UART8_TX_1    /* PE1 */

/* CAN - Updated 7/31/2025 - See stm32h7x3xx_pinmap.h and schematic for details
 *
 * CAN1 is N/C?
 * CAN2 goes to PDB connector
 */
#define GPIO_CAN1_RX     GPIO_CAN1_RX_4     /* PH14  */
#define GPIO_CAN1_TX     GPIO_CAN1_TX_4     /* PH13  */
#define GPIO_CAN2_RX     GPIO_CAN2_RX_2     /* PB5 */
#define GPIO_CAN2_TX     GPIO_CAN2_TX_2     /* PB6  */

/* SPI - Updated 7/31/2025 - See stm32h7x3xx_pinmap.h and schematic for details
 * SPI1 goes to carrier board
 * SPI4 goes to FRAM
 * SPI5 goes to BMI270
 */
#define ADJ_SLEW_RATE(p) (((p) & ~GPIO_SPEED_MASK) | (GPIO_SPEED_2MHz))

#define GPIO_SPI1_MISO   GPIO_SPI1_MISO_3               /* PG9  */
#define GPIO_SPI1_MOSI   GPIO_SPI1_MOSI_3               /* PD7  */
#define GPIO_SPI1_SCK    ADJ_SLEW_RATE(GPIO_SPI1_SCK_3) /* PG11  */

#define GPIO_SPI4_MISO   GPIO_SPI4_MISO_2               /* PE5 */
#define GPIO_SPI4_MOSI   GPIO_SPI4_MOSI_2               /* PE6  */
#define GPIO_SPI4_SCK    ADJ_SLEW_RATE(GPIO_SPI4_SCK_2) /* PE2 */

#define GPIO_SPI5_MISO   GPIO_SPI5_MISO_3               /* PJ11  */
#define GPIO_SPI5_MOSI   GPIO_SPI5_MOSI_3               /* PJ10 */
#define GPIO_SPI5_SCK    ADJ_SLEW_RATE(GPIO_SPI5_SCK_3) /* PK0  */

/* I2C - Updated 7/31/2025 - See stm32h7x3xx_pinmap.h and schematic for details
 *
 *   The optional _GPIO configurations allow the I2C driver to manually
 *   reset the bus to clear stuck slaves.  They match the pin configuration,
 *   but are normally-high GPIOs.
 *
 */

#define GPIO_I2C1_SCL GPIO_I2C1_SCL_2       /* PB8  */
#define GPIO_I2C1_SDA GPIO_I2C1_SDA_2       /* PB9  */

#define GPIO_I2C1_SCL_GPIO                  (GPIO_OUTPUT | GPIO_OPENDRAIN |GPIO_SPEED_50MHz | GPIO_OUTPUT_SET | GPIO_PORTB | GPIO_PIN8)
#define GPIO_I2C1_SDA_GPIO                  (GPIO_OUTPUT | GPIO_OPENDRAIN |GPIO_SPEED_50MHz | GPIO_OUTPUT_SET | GPIO_PORTB | GPIO_PIN9)

#define GPIO_I2C2_SCL GPIO_I2C2_SCL_2       /* PF1 */
#define GPIO_I2C2_SDA GPIO_I2C2_SDA_2       /* PF0 */

#define GPIO_I2C2_SCL_GPIO                  (GPIO_OUTPUT | GPIO_OPENDRAIN |GPIO_SPEED_50MHz | GPIO_OUTPUT_SET | GPIO_PORTF | GPIO_PIN1)
#define GPIO_I2C2_SDA_GPIO                  (GPIO_OUTPUT | GPIO_OPENDRAIN |GPIO_SPEED_50MHz | GPIO_OUTPUT_SET | GPIO_PORTF | GPIO_PIN0)

#define GPIO_I2C3_SCL GPIO_I2C3_SCL_2       /* PH7 */
#define GPIO_I2C3_SDA GPIO_I2C3_SDA_2       /* PH8 */

#define GPIO_I2C3_SCL_GPIO                  (GPIO_OUTPUT | GPIO_OPENDRAIN |GPIO_SPEED_50MHz | GPIO_OUTPUT_SET | GPIO_PORTH | GPIO_PIN7)
#define GPIO_I2C3_SDA_GPIO                  (GPIO_OUTPUT | GPIO_OPENDRAIN |GPIO_SPEED_50MHz | GPIO_OUTPUT_SET | GPIO_PORTH | GPIO_PIN8)

#define GPIO_I2C4_SCL GPIO_I2C4_SCL_3       /* PH11 */
#define GPIO_I2C4_SDA GPIO_I2C4_SDA_3       /* PH12 */

#define GPIO_I2C4_SCL_GPIO                  (GPIO_OUTPUT | GPIO_OPENDRAIN | GPIO_SPEED_50MHz | GPIO_OUTPUT_SET | GPIO_PORTH | GPIO_PIN11)
#define GPIO_I2C4_SDA_GPIO                  (GPIO_OUTPUT | GPIO_OPENDRAIN | GPIO_SPEED_50MHz | GPIO_OUTPUT_SET | GPIO_PORTH | GPIO_PIN12)

// /* SDMMC1 - MMC1 is used, not MMC2
//  * Updated 7/31/2025 - See stm32h7x3xx_pinmap.h and schematic for details
//  *
//  *      VDD 3.3
//  *      GND
//  *      SDMMC1_CK                           PC12
//  *      SDMMC1_CMD                          PD2
//  *      SDMMC1_D0                           PC8
//  *      SDMMC1_D1                           PC9
//  *      SDMMC1_D2                           PC10
//  *      SDMMC1_D3                           PC11
//  */
// Note - These have pin mapping names like GPIO_SDMMC1_CK_0 but they don't seem to compile in
//      though the manually configured pin mappings do work. Same issue for ethernet
// #define GPIO_SDMMC1_CK 		(GPIO_ALT|GPIO_AF12|GPIO_PORTC|GPIO_PIN12)  /* PC12 */
// #define GPIO_SDMMC1_CMD 	(GPIO_ALT|GPIO_AF12|GPIO_PORTD|GPIO_PIN2)   /* PD2 */
// #define GPIO_SDMMC1_D0 		(GPIO_ALT|GPIO_AF12|GPIO_PORTC|GPIO_PIN8)   /* PC8 */
// #define GPIO_SDMMC1_D1 		(GPIO_ALT|GPIO_AF12|GPIO_PORTC|GPIO_PIN9)   /* PC9 */
// #define GPIO_SDMMC1_D2 		(GPIO_ALT|GPIO_AF12|GPIO_PORTC|GPIO_PIN10)  /* PC10 */
// #define GPIO_SDMMC1_D3 		(GPIO_ALT|GPIO_AF12|GPIO_PORTC|GPIO_PIN11)  /* PC11 */

// ABM - Disabled, sdmmc2 is not used
// /* SDMMC2
//  *
//  *      VDD 3.3
//  *      GND
//  *      SDMMC2_CK                           PD6
//  *      SDMMC2_CMD                          PD7
//  *      SDMMC2_D0                           PB14
//  *      SDMMC2_D1                           PB15
//  *      SDMMC2_D2                           PG11
//  *      SDMMC2_D3                           PB4
//  */

// #define GPIO_SDMMC1_CK   GPIO_SDMMC2_CK_1  /* PD6 */
// #define GPIO_SDMMC2_CMD  GPIO_SDMMC2_CMD_1 /* PD7 */
// //      GPIO_SDMMC2_D0   No Remap          /* PB14 */
// //      GPIO_SDMMC2_D1   No Remap          /* PB15 */
// #define GPIO_SDMMC2_D2   GPIO_SDMMC2_D2_1  /* PG11 */
// //      GPIO_SDMMC2_D3    No Remap         /* PB4 */




/* The STM32 H7 connects to a TI DP83848TSQ/NOPB
 * using RMII
 *
 *   STM32 H7 BOARD        DP83848TSQ/NOPB
 *   GPIO     SIGNAL       PIN NAME
 *   -------- ------------ -------------
 *   PA7     ETH_CRS_DV    CRS_DV
 *   PA1     ETH_REF_CL    X1
 *   PC4     ETH_RXD0      RX_D0
 *   PC5     ETH_RXD1      RX_D1
 *   PB11    ETH_TX_EN     TX_EN
 *   PG13    ETH_TXD0      TX_D0
 *   PG12    ETH_TXD1      TX_D1
 *
 * The PHY address is 1, since COL/PHYAD0 features a pull up.
 */

// Updated 7/31/2025 - See stm32h7x3xx_pinmap.h and schematic for details
#define GPIO_ETH_RMII_CRS_DV 				(GPIO_ALT|GPIO_AF11|GPIO_PUSHPULL|GPIO_PORTA|GPIO_PIN7)                 /* PA7 */
#define GPIO_ETH_RMII_REF_CLK 				(GPIO_ALT|GPIO_AF11|GPIO_PUSHPULL|GPIO_PORTA|GPIO_PIN1)                 /* PA1 */
#define GPIO_ETH_RMII_RXD0 				(GPIO_ALT|GPIO_AF11|GPIO_PUSHPULL|GPIO_PORTC|GPIO_PIN4)                 /* PC4 */
#define GPIO_ETH_RMII_RXD1 				(GPIO_ALT|GPIO_AF11|GPIO_PUSHPULL|GPIO_PORTC|GPIO_PIN5)                 /* PC5 */
#define GPIO_ETH_RMII_TX_EN 				(GPIO_ALT|GPIO_AF11|GPIO_PUSHPULL|GPIO_PORTB|GPIO_PIN11)                /* PB11 */
#define GPIO_ETH_RMII_TXD0 				(GPIO_ALT|GPIO_AF11|GPIO_PUSHPULL|GPIO_PORTB|GPIO_PIN12)                /* PB12 */
#define GPIO_ETH_RMII_TXD1 				(GPIO_ALT|GPIO_AF11|GPIO_PUSHPULL|GPIO_PORTB|GPIO_PIN13)                /* PB13 */

// TIMERS
// TODO ABM: No timers were present in the original code
// Updated 7/31/2025 - See stm32h7x3xx_pinmap.h and schematic for details
// #define GPIO_TIM1_CH1OUT   GPIO_TIM1_CH1OUT_2  /* PE9  */
// #define GPIO_TIM1_CH2OUT   GPIO_TIM1_CH2OUT_2  /* PE11 */
// #define GPIO_TIM1_CH3OUT   GPIO_TIM1_CH3OUT_2  /* PE13 */
// #define GPIO_TIM1_CH4OUT   GPIO_TIM1_CH4OUT_2  /* PE14 */

// #define GPIO_TIM2_CH1OUT   GPIO_TIM2_CH1OUT_3  /* PA5  */
// #define GPIO_TIM2_CH2OUT   GPIO_TIM2_CH2OUT_2  /* PB3  */
// #define GPIO_TIM2_CH3OUT   GPIO_TIM2_CH3OUT_2  /* PA2 */
// #define GPIO_TIM2_CH4OUT   GPIO_TIM2_CH4OUT_2  /* PA3 */

/* USB
 *
 *      OTG_FS_DM                           PA11
 *      OTG_FS_DP                           PA12
 *      VBUS                                PA9
 */


/* Board provides GPIO or other Hardware for signaling to timing analyzer */

#if defined(CONFIG_BOARD_USE_PROBES)
# include "stm32_gpio.h"
# define PROBE_N(n) (1<<((n)-1))
# define PROBE_1    (GPIO_OUTPUT|GPIO_PUSHPULL|GPIO_SPEED_2MHz|GPIO_OUTPUT_CLEAR|GPIO_PORTI|GPIO_PIN0)   /* PI0 AUX1 */
# define PROBE_2    (GPIO_OUTPUT|GPIO_PUSHPULL|GPIO_SPEED_2MHz|GPIO_OUTPUT_CLEAR|GPIO_PORTH|GPIO_PIN12)  /* PH12 AUX2 */
# define PROBE_3    (GPIO_OUTPUT|GPIO_PUSHPULL|GPIO_SPEED_2MHz|GPIO_OUTPUT_CLEAR|GPIO_PORTH|GPIO_PIN11)  /* PH11 AUX3 */
# define PROBE_4    (GPIO_OUTPUT|GPIO_PUSHPULL|GPIO_SPEED_2MHz|GPIO_OUTPUT_CLEAR|GPIO_PORTH|GPIO_PIN10)  /* PH10 AUX4 */
# define PROBE_5    (GPIO_OUTPUT|GPIO_PUSHPULL|GPIO_SPEED_2MHz|GPIO_OUTPUT_CLEAR|GPIO_PORTD|GPIO_PIN13)  /* PD13 AUX5 */
# define PROBE_6    (GPIO_OUTPUT|GPIO_PUSHPULL|GPIO_SPEED_2MHz|GPIO_OUTPUT_CLEAR|GPIO_PORTD|GPIO_PIN14)  /* PD14 AUX6 */
# define PROBE_7    (GPIO_OUTPUT|GPIO_PUSHPULL|GPIO_SPEED_2MHz|GPIO_OUTPUT_CLEAR|GPIO_PORTH|GPIO_PIN6)   /* PH6  AUX7 */
# define PROBE_8    (GPIO_OUTPUT|GPIO_PUSHPULL|GPIO_SPEED_2MHz|GPIO_OUTPUT_CLEAR|GPIO_PORTH|GPIO_PIN9)   /* PH9  AUX8 */
# define PROBE_9    (GPIO_OUTPUT|GPIO_PUSHPULL|GPIO_SPEED_2MHz|GPIO_OUTPUT_CLEAR|GPIO_PORTE|GPIO_PIN11)  /* PE11  CAP1 */

# define PROBE_INIT(mask) \
	do { \
		if ((mask)& PROBE_N(1)) { stm32_configgpio(PROBE_1); } \
		if ((mask)& PROBE_N(2)) { stm32_configgpio(PROBE_2); } \
		if ((mask)& PROBE_N(3)) { stm32_configgpio(PROBE_3); } \
		if ((mask)& PROBE_N(4)) { stm32_configgpio(PROBE_4); } \
		if ((mask)& PROBE_N(5)) { stm32_configgpio(PROBE_5); } \
		if ((mask)& PROBE_N(6)) { stm32_configgpio(PROBE_6); } \
		if ((mask)& PROBE_N(7)) { stm32_configgpio(PROBE_7); } \
		if ((mask)& PROBE_N(8)) { stm32_configgpio(PROBE_8); } \
		if ((mask)& PROBE_N(9)) { stm32_configgpio(PROBE_9); } \
	} while(0)

# define PROBE(n,s)  do {stm32_gpiowrite(PROBE_##n,(s));}while(0)
# define PROBE_MARK(n) PROBE(n,false);PROBE(n,true)
#else
# define PROBE_INIT(mask)
# define PROBE(n,s)
# define PROBE_MARK(n)
#endif

#endif  /*__NUTTX_CONFIG_PX4_FMU_V6X_INCLUDE_BOARD_H  */
