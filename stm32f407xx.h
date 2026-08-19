#include "stm32f407xx_spi_driver.h"/*
 * stm32f407xx.h
 *
 *  Created on: Jul 17, 2026
 *      Author: irakulkarni
 *
 */

#ifndef INC_STM32F407XX_H_
#define INC_STM32F407XX_H_

#include <stddef.h>
#include <stdint.h>

#define __vo volatile
#define __weak __attribute__((weak))

// START Processor Specific Details
// ARM Cortex Mx Processor NVIC ISERx register addresses
#define NVIC_ISER0    ((__vo uint32_t *)0xE000E100U)
#define NVIC_ISER1    ((__vo uint32_t *)0xE000E104U)
#define NVIC_ISER2    ((__vo uint32_t *)0xE000E108U)
#define NVIC_ISER3    ((__vo uint32_t *)0xE000E10CU)

#define NVIC_ICER0    ((__vo uint32_t *)0xE000E180U)
#define NVIC_ICER1    ((__vo uint32_t *)0xE000E184U)
#define NVIC_ICER2    ((__vo uint32_t *)0xE000E188U)
#define NVIC_ICER3    ((__vo uint32_t *)0xE000E18CU)

// ARM Cortex Mx Processor Priority Register Address Calculation
#define NVIC_PR_BASE_ADDR				((__vo uint32_t*)0xE000E400)

// ARM Cortex Mx Processor number of priority bits implemented in Priority Register
#define NO_PR_BITS_IMPLEMENTED					4


// Base addresses of Flash and SRAM memories

#define FLASH_BASEADDR 					0x080000000U // starting address of Flash memory (stores program)
#define SRAM1_BASEADDR					0x200000000U // starting address of SRAM1 (main RAM used for variables)
#define SRAM2_BASEADDR					0x200001C00U // starting address of SRAM2 (additional RAM)
#define ROM_BASEADDR				  	0x1FFF00000U // starting address of System ROM (bootloader - helps load firmware onto STM32)
#define SRAM 							      SRAM1_BASEADDR

// AHBx and APBx Bus Peripheral base addresses

#define PERIPH_BASEADDR					  0x400000000U // base address of all peripheral registers
#define APB1PERIPH_BASEADDR				PERIPH_BASEADDR // APBI bus starts at the peripheral base address
#define APB2PERIPH_BASEADDR				0x400100000U // APB2 bus starting address
#define AHB1PERIPH_BASEADDR				0x400200000U // AHB1 bus starting address
#define AHB2PERIPH_BASEADDR				0x500000000U // AHB2 bus starting address

// Base addresses of peripherals which are hanging on AHB1 bus
#define GPIOA_BASEADDR					(AHB1PERIPH_BASEADDR + 0x0000)
#define GPIOB_BASEADDR					(AHB1PERIPH_BASEADDR + 0x0400)
#define GPIOC_BASEADDR					(AHB1PERIPH_BASEADDR + 0x0800)
#define GPIOD_BASEADDR					(AHB1PERIPH_BASEADDR + 0x0C00)
#define GPIOE_BASEADDR					(AHB1PERIPH_BASEADDR + 0x1000)
#define GPIOF_BASEADDR					(AHB1PERIPH_BASEADDR + 0x1400)
#define GPIOG_BASEADDR					(AHB1PERIPH_BASEADDR + 0x1800)
#define GPIOH_BASEADDR					(AHB1PERIPH_BASEADDR + 0x1C00)
#define GPIOI_BASEADDR					(AHB1PERIPH_BASEADDR + 0x2000)
#define RCC_BASEADDR				  	(AHB1PERIPH_BASEADDR + 0x3800)

// Base addresses of peripherals which are hanging on APB1 bus
#define I2C1_BASEADDR					(APB1PERIPH_BASEADDR + 0x5400)
#define I2C2_BASEADDR					(APB1PERIPH_BASEADDR + 0x5800)
#define I2C3_BASEADDR					(APB1PERIPH_BASEADDR + 0x5C00)

// Base addresses of peripherals which are hanging on APB1 bus
#define I2C1_BASEADDR					(APB1PERIPH_BASEADDR + 0x5400)
#define I2C2_BASEADDR					(APB1PERIPH_BASEADDR + 0x5800)
#define I2C3_BASEADDR					(APB1PERIPH_BASEADDR + 0x5C00)

#define SPI2_BASEADDR					(APB1PERIPH_BASEADDR + 0x3800)
#define SPI3_BASEADDR					(APB1PERIPH_BASEADDR + 0x3C00)

#define USART2_BASEADDR					(APB1PERIPH_BASEADDR + 0x4400)
#define USART3_BASEADDR					(APB1PERIPH_BASEADDR + 0x4800)
#define UART4_BASEADDR					(APB1PERIPH_BASEADDR + 0x4C00)
#define UART5_BASEADDR					(APB1PERIPH_BASEADDR + 0x5000)

// Base addresses of peripherals which are hanging on APB2 bus
#define EXTI_BASEADDR					(APB2PERIPH_BASEADDR + 0x3C00)
#define SPI1_BASEADDR					(APB2PERIPH_BASEADDR + 0x3000)
#define SYSCFG_BASEADDR					(APB2PERIPH_BASEADDR + 0x3800)
#define USART1_BASEADDR					(APB2PERIPH_BASE + 0x1000)
#define USART6_BASEADDR					(APB2PERIPH_BASE + 0x1400)

/***********************Peripheral Register Definition Structures***********************/

// This structure groups all the registers for a GPIO peripheral into one user-defined data
// data type so we can easily access registers instead of using memory addresses manually

typedef struct
{
	__vo uint32_t MODER; // GPIO port mode register					Address offset: 0x00
	__vo uint32_t OTYPER; // GPIO type register						Address offset: 0x04
	__vo uint32_t OSPEEDR; // GPIO output speed register			Address offset: 0x08
	__vo uint32_t PUPDR; // GPIO Pull-up/pull-down register			Address offset: 0x0C
	__vo uint32_t IDR; // GPIO input data register					Address offset: 0x10
	__vo uint32_t ODR; // GPIO output data register					Address offset: 0x14
	__vo uint32_t BSRRL; // Bit set register low					Address offset: 0x18
	__vo uint32_t BSRRH; // Bit set register high					Address offset: 0x1A
	__vo uint32_t LCKR; // Configuration lock register				Address offset: 0x1C
	__vo uint32_t AFR[2]; // AFR[0]: GPIO alternate function low register AFR[1]: GPIO alternate function high register Address offset: 0x20-0x24
}GPIO_RegDef_t;


// Peripheral register definition structure for RCC

typedef struct
{
	__vo uint32_t CR;             /*!< Clock control register                    Address offset: 0x00 */
	__vo uint32_t PLLCFGR;        /*!< PLL configuration register                Address offset: 0x04 */
	__vo uint32_t CFGR;           /*!< Clock configuration register              Address offset: 0x08 */
	__vo uint32_t CIR;            /*!< Clock interrupt register                  Address offset: 0x0C */
	__vo uint32_t AHB1RSTR;       /*!< AHB1 peripheral reset register            Address offset: 0x10 */
	__vo uint32_t AHB2RSTR;       /*!< AHB2 peripheral reset register            Address offset: 0x14 */
	__vo uint32_t AHB3RSTR;       /*!< AHB3 peripheral reset register            Address offset: 0x18 */
	uint32_t RESERVED0;           /*!< Reserved, 0x1C */
	__vo uint32_t APB1RSTR;       /*!< APB1 peripheral reset register            Address offset: 0x20 */
	__vo uint32_t APB2RSTR;       /*!< APB2 peripheral reset register            Address offset: 0x24 */
	uint32_t RESERVED1[2];        /*!< Reserved, 0x28-0x2C */
	__vo uint32_t AHB1ENR;        /*!< AHB1 peripheral clock enable register     Address offset: 0x30 */
	__vo uint32_t AHB2ENR;        /*!< AHB2 peripheral clock enable register     Address offset: 0x34 */
	__vo uint32_t AHB3ENR;        /*!< AHB3 peripheral clock enable register     Address offset: 0x38 */
	uint32_t RESERVED2;           /*!< Reserved, 0x3C */
	__vo uint32_t APB1ENR;        /*!< APB1 peripheral clock enable register     Address offset: 0x40 */
	__vo uint32_t APB2ENR;        /*!< APB2 peripheral clock enable register     Address offset: 0x44 */
	uint32_t RESERVED3[2];        /*!< Reserved, 0x48-0x4C */
	__vo uint32_t AHB1LPENR;      /*!< AHB1 low-power enable register            Address offset: 0x50 */
	__vo uint32_t AHB2LPENR;      /*!< AHB2 low-power enable register            Address offset: 0x54 */
	__vo uint32_t AHB3LPENR;      /*!< AHB3 low-power enable register            Address offset: 0x58 */
	uint32_t RESERVED4;           /*!< Reserved, 0x5C */
	__vo uint32_t APB1LPENR;      /*!< APB1 low-power enable register            Address offset: 0x60 */
	__vo uint32_t APB2LPENR;      /*!< APB2 low-power enable register            Address offset: 0x64 */
	uint32_t RESERVED5[2];        /*!< Reserved, 0x68-0x6C */
	__vo uint32_t BDCR;           /*!< Backup domain control register            Address offset: 0x70 */
	__vo uint32_t CSR;            /*!< Clock control/status register             Address offset: 0x74 */
	uint32_t RESERVED6[2];        /*!< Reserved, 0x78-0x7C */
	__vo uint32_t SSCGR;          /*!< Spread spectrum clock register            Address offset: 0x80 */
	__vo uint32_t PLLI2SCFGR;     /*!< PLLI2S configuration register             Address offset: 0x84 */
	__vo uint32_t PLLSAICFGR;     /*!< PLLSAI configuration register             Address offset: 0x88 */
	__vo uint32_t DCKCFGR;        /*!< Dedicated clocks configuration register   Address offset: 0x8C */
	__vo uint32_t CKGATENR;       /*!< Clocks gated enable register              Address offset: 0x90 */
	__vo uint32_t DCKCFGR2;       /*!< Dedicated clocks configuration register 2 Address offset: 0x94 */

} RCC_RegDef_t;

// Peripheral register definition structure for EXTI

typedef struct
{
    __vo uint32_t IMR;    // Interrupt Mask Register             Address offset: 0x00
    __vo uint32_t EMR;    // Event Mask Register                 Address offset: 0x04
    __vo uint32_t RTSR;   // Rising Trigger Selection Register   Address offset: 0x08
    __vo uint32_t FTSR;   // Falling Trigger Selection Register  Address offset: 0x0C
    __vo uint32_t SWIER;  // Software Interrupt Event Register   Address offset: 0x10
    __vo uint32_t PR;     // Pending Register                    Address offset: 0x14
}EXTI_RegDef_t;

// Peripheral register definition structure for SPI

typedef struct
{
	__vo uint32_t CR1;      // SPI Control Register 1: Configures SPI mode, clock, data size, master/slave, etc.
	__vo uint32_t CR2;      // SPI Control Register 2: Configures interrupts, DMA, and slave select features.
	__vo uint32_t SR;       // SPI Status Register: Indicates transmit/receive status and error flags.
	__vo uint32_t DR;       // SPI Data Register: Holds data to be transmitted or received.
	__vo uint32_t CRCPR;    // SPI CRC Polynomial Register: Stores the polynomial used for CRC calculation.
	__vo uint32_t RXCRCR;   // SPI RX CRC Register: Contains the CRC value calculated for received data.
	__vo uint32_t TXCRCR;   // SPI TX CRC Register: Contains the CRC value calculated for transmitted data.
	__vo uint32_t I2SCFGR;  // I2S Configuration Register: Configures the peripheral when operating in I2S mode.
	__vo uint32_t I2SPR;    // I2S Prescaler Register: Sets the clock prescaler for I2S mode.

} SPI_RegDef_t;

// Peripheral register definition structure for SYSCFG

typedef struct
{
    __vo uint32_t MEMRMP;        // Memory Remap Register                     Address offset: 0x00
    __vo uint32_t PMC;           // Peripheral Mode Configuration Register    Address offset: 0x04
    __vo uint32_t EXTICR[4];     // External Interrupt Configuration Registers Address offset: 0x08 - 0x14
    uint32_t RESERVED1[2];       // Reserved                                  Address offset: 0x18 - 0x1C
    __vo uint32_t CMPCR;         // Compensation Cell Control Register        Address offset: 0x20
    uint32_t RESERVED2[2];       // Reserved                                  Address offset: 0x24 - 0x28
    __vo uint32_t CFGR;          // Configuration Register                    Address offset: 0x2C

} SYSCFG_RegDef_t;

// Peripheral register definition structure for I2C
typedef struct
{
	__vo uint32_t CR1;    // Control Register 1 - Enables/disables I2C and controls basic operation
	__vo uint32_t CR2;    // Control Register 2 - Configures interrupts, DMA, and peripheral clock frequency
	__vo uint32_t OAR1;   // Own Address Register 1 - Stores the primary I2C device address
	__vo uint32_t OAR2;   // Own Address Register 2 - Stores the secondary I2C address (if dual addressing is used)
	__vo uint32_t DR;     // Data Register - Holds data to be transmitted or received
	__vo uint32_t SR1;    // Status Register 1 - Indicates communication events and errors
	__vo uint32_t SR2;    // Status Register 2 - Provides additional status information (busy, master/slave, etc.)
	__vo uint32_t CCR;    // Clock Control Register - Sets the I2C clock speed (Standard/Fast mode)
	__vo uint32_t TRISE;  // TRISE Register - Configures the maximum SCL rise time
	__vo uint32_t FLTR;   // Filter Register - Configures analog and digital noise filters
} I2C_RegDef_t;


// Peripheral definitions (peripheral base addresses typecasted to xxx_RegDef_t
// Tells compiler where each GPIO peripheral is located in memory and lets us access its registers

#define GPIOA 							((GPIO_RegDef_t*)GPIOA_BASEADDR)
#define GPIOB 							((GPIO_RegDef_t*)GPIOB_BASEADDR)
#define GPIOC 							((GPIO_RegDef_t*)GPIOC_BASEADDR)
#define GPIOD 							((GPIO_RegDef_t*)GPIOD_BASEADDR)
#define GPIOE 							((GPIO_RegDef_t*)GPIOE_BASEADDR)
#define GPIOF 							((GPIO_RegDef_t*)GPIOF_BASEADDR)
#define GPIOG 							((GPIO_RegDef_t*)GPIOG_BASEADDR)
#define GPIOH 							((GPIO_RegDef_t*)GPIOH_BASEADDR)
#define GPIOI 							((GPIO_RegDef_t*)GPIOI_BASEADDR)

#define RCC								((RCC_RegDef_t*)RCC_BASEADDR)
#define EXTI							((EXTI_RegDef_t*)EXTI_BASEADDR)
#define SYSCFG							((SYSCFG_RegDef_t*)SYSCFG_BASEADDR)

#define SPI1							((SPI_RegDef_t*)SPI1_BASEADDR)
#define SPI2 							((SPI_RegDef_t*)SPI2_BASEADDR)
#define SPI3							((SPI_RegDef_t*)SPI3_BASEADDR)

#define I2C1							((I2C_RegDef_t*)I2C1_BASEADDR)
#define I2C2							((I2C_RegDef_t*)I2C2_BASEADDR)
#define I2C3							((I2C_RegDef_t*)I2C3_BASEADDR)

// Clock enable macros for GPIOx peripherals

#define GPIOA_PCLK_EN()					(RCC ->AHB1ENR |= (1 << 0))
#define GPIOB_PCLK_EN()    			    (RCC ->AHB1ENR |= (1 << 1))
#define GPIOC_PCLK_EN()    			    (RCC ->AHB1ENR |= (1 << 2))
#define GPIOD_PCLK_EN()   			    (RCC ->AHB1ENR |= (1 << 3))
#define GPIOE_PCLK_EN()  			    (RCC ->AHB1ENR |= (1 << 4))
#define GPIOF_PCLK_EN()   			    (RCC ->AHB1ENR |= (1 << 5))
#define GPIOG_PCLK_EN()   			    (RCC ->AHB1ENR |= (1 << 6))
#define GPIOH_PCLK_EN()                 (RCC ->AHB1ENR |= (1 << 7))

// Clock enable macros for I2Cx peripherals
#define I2C1_PCLK_EN()					(RCC ->APB1ENR |= (1 << 21))
#define I2C2_PCLK_EN()					(RCC ->APB1ENR |= (1 << 22))
#define I2C3_PCLK_EN()					(RCC ->APB1ENR |= (1 << 23))

// Clock enable macros for SPIx peripherals
#define SPI1_PCLK_EN()					(RCC ->APB2ENR |= (1 << 12))
#define SPI2_PCLK_EN()					(RCC ->APB1ENR |= (1 << 14))
#define SPI3_PCLK_EN()					(RCC ->APB1ENR |= (1 << 15))
#define SPI4_PCLK_EN()					(RCC ->APB2ENR |= (1 << 13))

// Clock enable macros for USARTx peripherals
#define USART1_PCCK_EN()				(RCC ->APB2ENR |= (1 << 4))
#define USART2_PCCK_EN()				(RCC ->APB1ENR |= (1 << 17))
#define USART3_PCCK_EN()				(RCC ->APB1ENR |= (1 << 18))
#define UART4_PCCK_EN()					(RCC ->APB1ENR |= (1 << 19))
#define UART5_PCCK_EN()					(RCC ->APB1ENR |= (1 << 20))
#define USART6_PCCK_EN()				(RCC ->APB1ENR |= (1 << 5))

// Clock enable macros for SYSCFG peripherals
#define SYSCFG_PCLK_EN()				(RCC ->APB2ENR |= (1 << 14))

// Clock disable macros for GPIOx peripherals
#define GPIOA_PCLK_DI()					(RCC ->AHB1ENR &= ~(1 << 0))
#define GPIOB_PCLK_DI() 				(RCC->AHB1ENR &= ~(1 << 1))
#define GPIOC_PCLK_DI() 				(RCC->AHB1ENR &= ~(1 << 2))
#define GPIOD_PCLK_DI() 				(RCC->AHB1ENR &= ~(1 << 3))
#define GPIOE_PCLK_DI() 				(RCC->AHB1ENR &= ~(1 << 4))
#define GPIOF_PCLK_DI() 				(RCC->AHB1ENR &= ~(1 << 5))
#define GPIOG_PCLK_DI() 				(RCC->AHB1ENR &= ~(1 << 6))

// Clock disable macros for I2Cx peripherals
#define I2C1_PCLK_DI()					(RCC ->APB1ENR |= (1 << 21))
#define I2C2_PCLK_DI()					(RCC ->APB1ENR |= (1 << 22))
#define I2C3_PCLK_DI()					(RCC ->APB1ENR |= (1 << 23))

// Clock disable macros for SPIx peripherals

// Clock disable macros for USARTx peripherals

// Clock disable macros for SYSCFG peripherals

// Macros to reset GPIOx peripherals
// Macros to reset GPIOx peripherals

#define GPIOA_REG_RESET()    do { (RCC->AHB1RSTR |=  (1 << 0)); \
                                  (RCC->AHB1RSTR &= ~(1 << 0)); } while (0)

#define GPIOB_REG_RESET()    do { (RCC->AHB1RSTR |=  (1 << 1)); \
                                  (RCC->AHB1RSTR &= ~(1 << 1)); } while (0)

#define GPIOC_REG_RESET()    do { (RCC->AHB1RSTR |=  (1 << 2)); \
                                  (RCC->AHB1RSTR &= ~(1 << 2)); } while (0)

#define GPIOD_REG_RESET()    do { (RCC->AHB1RSTR |=  (1 << 3)); \
                                  (RCC->AHB1RSTR &= ~(1 << 3)); } while (0)

#define GPIOE_REG_RESET()    do { (RCC->AHB1RSTR |=  (1 << 4)); \
                                  (RCC->AHB1RSTR &= ~(1 << 4)); } while (0)

#define GPIOF_REG_RESET()    do { (RCC->AHB1RSTR |=  (1 << 5)); \
                                  (RCC->AHB1RSTR &= ~(1 << 5)); } while (0)

#define GPIOG_REG_RESET()    do { (RCC->AHB1RSTR |=  (1 << 6)); \
                                  (RCC->AHB1RSTR &= ~(1 << 6)); } while (0)

#define GPIOH_REG_RESET()    do { (RCC->AHB1RSTR |=  (1 << 7)); \
                                  (RCC->AHB1RSTR &= ~(1 << 7)); } while (0)

#define GPIOI_REG_RESET()    do { (RCC->AHB1RSTR |=  (1 << 8)); \
                                  (RCC->AHB1RSTR &= ~(1 << 8)); } while (0)}

// Returns port code for giving GPIOx base address

#define GPIO_BASEADDR_TO_CODE(x)		((x == GPIOA) ? 0 :\
										(x == GPIOB) ? 1 :\
										(x == GPIOC) ? 2 :\
										(x == GPIOD) ? 3 :\
										(x == GPIOE) ? 4 :\
										(x == GPIOF) ? 5 :\
										(x == GPIOG) ? 6 :\
										(x == GPIOH) ? 7 :\
										(x == GPIOI) ? 8 : 0)

// IRQ (Interrupt Request) number of STM32F407x MCU
#define IRQ_NO_EXTI0		  6
#define IRQ_NO_EXTI1		  7
#define IRQ_NO_EXTI2		  8
#define IRQ_NO_EXTI3		  9
#define IRQ_NO_EXTI4		  10
#define IRQ_NO_EXTI9_5		23
#define IRQ_NO_EXTI5_10		40
#define IRQ_NO_SPI1		  	35
#define IRQ_NO_SPI2		  	36
#define IRQ_NO_SPI3		  	51
#define IRQ_NO_SPI4
#define IRQ_NO_I2C1_EV		31
#define IRQ_NO_I2C1_ER		32


// Some generic macros
#define ENABLE 			    	1
#define DISABLE 	    		0
#define SET 			      	ENABLE
#define RESET 			    	DISABLE
#define GPIO_PIN_SET	   	SET
#define GPIO_PIN_RESET		RESET
#define FLAG_RESET			  RESET
#define FLAG_SET		    	SET

/**********************************************************************************
 * Bit position definitions of SPI peripherals
 **********************************************************************************/

/*
 * Bit position definitions SPI_CR1
 */
#define SPI_CR1_CPHA	  	0
#define SPI_CR1_CPOL	  	1
#define SPI_CR1_MSTR	  	2
#define SPI_CR1_BR		  	3
#define SPI_CR1_SPE			  6
#define SPI_CR1_LSBFIRST	7
#define SPI_CR1_SSI			  8
#define SPI_CR1_SSM			  9
#define SPI_CR1_RXONLY		10
#define SPI_CR1_DFF			  11
#define SPI_CR1_CRCNEXT		12
#define SPI_CR1_CRCEN		  13
#define SPI_CR1_BIDIOE		14
#define SPI_CR1_BIDIMODE	15

/*
 * Bit position definitions SPI_CR2
 */
#define SPI_CR2_RXDMAEN		0
#define SPI_CR2_TXDMAEN		1
#define SPI_CR2_SSOE		  2
#define SPI_CR2_FRF			  4
#define SPI_CR2_ERRIE		  5
#define SPI_CR2_RXNEIE		6
#define SPI_CR2_TXEIE		  7

/*
 * Bit position definitions SPI_SR
 */
#define SPI_SR_RXNE			0
#define SPI_SR_TXE			1
#define SPI_SR_CHSIDE		2
#define SPI_SR_UDR			3
#define SPI_SR_CRCERR		4
#define SPI_SR_MODF			5
#define SPI_SR_OVR			6
#define SPI_SR_BSY			7
#define SPI_SR_FRE			8

/**********************************************************************************
 * Bit position definitions of I2C peripherals
 **********************************************************************************/
/*
 * Bit position definitions I2C_CR1
 */
#define I2C_CR1_PE				0
#define I2C_CR1_NOSTRETCH	7
#define I2C_CR1_START			8
#define I2C_CR1_STOP			9
#define I2C_CR1_ACK				10
#define I2C_CR1_SWRST			15

/*
 * Bit position definitions I2C_CR2
 */
#define I2C_CR2_FREQ				0
#define I2C_CR2_ITERREN			8
#define I2C_CR2_ITEVTEN			9
#define I2C_CR2_ITBUFEN			10

/*
 * Bit position definitions I2C_OAR1
 */
#define I2C_OAR1_ADD0				0
#define I2C_OAR1_ADD71			1
#define I2C_OAR1_ADD98			8
#define I2C_OAR1_ADDMODE		15

/*
 * Bit position definitions I2C_SR1
 */
#define I2C_SR1_SB				0
#define I2C_SR1_ADDR			1
#define I2C_SR1_BTF				2
#define I2C_SR1_ADD10			3
#define I2C_SR1_STOPF			4
#define I2C_SR1_RXNE			6
#define I2C_SR1_TXE				7
#define I2C_SR1_BERR			8
#define I2C_SR1_ARLO			9
#define I2C_SR1_AF				10
#define I2C_SR1_OVR				11
#define I2C_SR1_TIMEOUT		14

/*
 * Bit position definitions I2C_SR2
 */
#define I2C_SR2_MSL				0
#define I2C_SR2_BUSY			1
#define I2C_SR2_TRA				2
#define I2C_SR2_GENCALL		4
#define I2C_SR2_DUALF			7

/*
 * Bit position definitions I2C_CCR
 */
#define I2C_CCR_CCR				0
#define I2C_CCR_DUTY			14
#define I2C_CCR_FS				15

#include "stm32f407xx_gpio_driver.h"
#include "stm32f407xx_spi_driver.h"
#include "stm32f407xx_i2c_driver.h"

#endif /* INC_STM32407XX_H_ */
