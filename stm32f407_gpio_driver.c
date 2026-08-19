/*
 * stm32f407xx_gpio_driver.c
 *
 *  Created on: Jul 21, 2026
 *      Author: irakulkarni
 */

#include "stm32f407xx_gpio_driver.h"

/**
 * Peripheral clock setup
 */

/******************************************************************************
 * @fn                  - GPIO_PeriClockControl
 *
 * @brief               - This function enables or disables peripheral clock for the given GPIO port
 *
 * @param[in]           - Base address of GPIO peripheral
 * @param[in]           - Enable or disable macros
 * @param[in]           -
 *
 * @return              - none
 *
 * @Note                - none
 *
 *
 ******************************************************************************/

void GPIO_PeriClockControl(GPIO_RegDef_t *pGPIOx, uint8_t EnorDi) {
	if (EnorDi == ENABLE) {
		if (pGPIOx == GPIOA) {
			GPIOA_PCLK_EN();
		} else if (pGPIOx == GPIOB) {
			GPIOB_PCLK_EN();
		} else if (pGPIOx == GPIOC) {
			GPIOC_PCLK_EN();
		} else if (pGPIOx == GPIOD) {
			GPIOD_PCLK_EN();
		} else if (pGPIOx == GPIOE) {
			GPIOE_PCLK_EN();
		} else if (pGPIOx == GPIOF) {
			GPIOF_PCLK_EN();
		} else if (pGPIOx == GPIOG) {
			GPIOG_PCLK_EN();
		}
	} else {
		if (pGPIOx == GPIOA) {
			GPIOA_PCLK_DI();
		} else if (pGPIOx == GPIOB) {
			GPIOB_PCLK_DI();
		} else if (pGPIOx == GPIOC) {
			GPIOC_PCLK_DI();
		} else if (pGPIOx == GPIOD) {
			GPIOD_PCLK_DI();
		} else if (pGPIOx == GPIOE) {
			GPIOE_PCLK_DI();
		} else if (pGPIOx == GPIOF) {
			GPIOF_PCLK_DI();
		} else if (pGPIOx == GPIOG) {
			GPIOG_PCLK_DI();
		}
	}
}

/**
 * Init and de-init
 */
/******************************************************************************
 * @fn                  - GPIO_Init
 *
 * @brief               - Initializes and configures a GPIO pin
 *
 *
 * @param[in]           - pGPIOHandle : Pointer to GPIO handle structure containing the pin configuration
 *
 * @return              - None
 *
 * @Note                - Configures the selected GPIO pin's mode, speed, pull-up/pull-down resistor,
 * 						output type, and alternate function if required.
 *
 *
 *
 * ******************************************************************************/

void GPIO_Init(GPIO_Handle_t *pGPIOHandle) {
	uint32_t temp = 0; // temp register

	// 1. Configure the mode of the GPIO pin

	if (pGPIOHandle->GPIO_PinConfig.GPIO_PinMode <= GPIO_MODE_ANALOG) {
		// the non interrupt mode
		temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinMode
				<< (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
		pGPIOHandle->pGPIOx->MODER &= ~(0x3
				<< pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber); // clearing
		pGPIOHandle->pGPIOx->MODER |= temp; // setting
	} else {
		if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IT_FT)
		{
			// 1. Configure the FTSR
			EXTI->FTSR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
			// Clear the corresponding RTSR bit
			EXTI->RTSR &= ~(1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);

		}else if (pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IT_RFT)
		{
			// 1. Configure the RTSR
			EXTI->RTSR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
			// Clear the corresponding RTSR bit
			EXTI->FTSR &= ~(1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);

		}else if (pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IT_RFT)
		{
			// 1. Configure both FTSR and RTSR
			EXTI->RTSR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
			// Clear the corresponding RTSR bit
			EXTI->FTSR |= ~(1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
		}

		// 2. Configure the GPIO port selection in SYSCFG_EXTICR
		uint8_t temp1 = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber / 4;
		uint8_t temp2 = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber % 4;
		uint8_t portcode = GPIO_BASEADDR_TO_CODE(pGPIOHandle->pGPIOx);
		SYSCFG_PCLK_EN();
		SYSCFG->EXTICR[temp1] = portcode << (temp2 * 4);

		// 3. Enable the EXTI interrupt delivery using IMR
		EXTI->IMR |= 1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber;
	}

	temp = 0;

	// 2. Configure the speed
	temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinSpeed
			<< (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
	pGPIOHandle->pGPIOx->OSPEEDR &= ~(0x3
			<< pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber); // clearing
	pGPIOHandle->pGPIOx->OSPEEDR |= temp;

	temp = 0;

	// 3. Configure the pupd settings
	temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinPuPdControl
			<< (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
	pGPIOHandle->pGPIOx->PUPDR &= ~(0x3
			<< pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber); // clearing
	pGPIOHandle->pGPIOx->PUPDR |= temp;

	temp = 0;

	// 4. Configure the optype
	temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinOPType
			<< (pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
	pGPIOHandle->pGPIOx->OTYPER &= ~(0x1
			<< pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber); // clearing
	pGPIOHandle->pGPIOx->OTYPER |= temp;

	// 5. Configure the alternate functionality
	// 5. Configure the alternate functionality
	if (pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_ALTFN) {
		uint8_t temp1, temp2;

		temp1 = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber / 8;
		temp2 = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber % 8;

		pGPIOHandle->pGPIOx->AFR[temp1] &= ~(0xF << (4 * temp2));
		pGPIOHandle->pGPIOx->AFR[temp1] |=
				(pGPIOHandle->GPIO_PinConfig.GPIO_PinAltFunMode << (4 * temp2));
	}
}

/******************************************************************************
 * @fn                  - GPIO_DeInit
 *
 * @brief               - Resets the selected GPIO peripheral to its default reset state
 *
 *
 * @param[in]           - pGPIOx: base address of GPIO port
 *
 * @return              - None
 *
 * @Note                - Clears all GPIO register configurations for selected port
 *
 *
 *
 * ******************************************************************************/

void GPIO_DeInit(GPIO_RegDef_t *pGPIOx)
{
			if (pGPIOx == GPIOA) {
				GPIOA_REG_RESET();
			} else if (pGPIOx == GPIOB) {
				GPIOB_REG_RESET();
			} else if (pGPIOx == GPIOC) {
				GPIOC_REG_RESET();
			} else if (pGPIOx == GPIOD) {
				GPIOD_REG_RESET();
			} else if (pGPIOx == GPIOE) {
				GPIOE_REG_RESET();
			} else if (pGPIOx == GPIOF) {
				GPIOF_REG_RESET();
			} else if (pGPIOx == GPIOG) {
				GPIOG_REG_RESET();
			} else if (pGPIOx == GPIOH) {
				GPIOH_REG_RESET();
			} else if (pGPIOx == GPIOI){
				GPIOA_REG_RESET();
			}
}

/**
 * Data read and write
 */
/******************************************************************************
* @fn                  - GPIO_ReadFromInputPin
*
* @brief               - Reads the logic level of the specified GPIO input pin
*
* @param[in]           - pGPIOx : Base address of the GPIO peripheral
* @param[in]           - PinNumber : GPIO pin number to be read
*
* @return              - uint8_t : Returns the current logic level of the pin
*                        (0 = LOW, 1 = HIGH)
*
* @Note                - Reads the corresponding bit from the GPIO input data
*                        register (IDR) and returns its value.
*
******************************************************************************/

uint8_t GPIO_ReadFromInputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber)
{
	uint8_t value;
	value = (uint8_t) ((pGPIOx->IDR >> PinNumber) && 0x00000001);

	return value;
}

/******************************************************************************
 * @fn                  - GPIO_ReadFromInputPort
 *
 * @brief               - Reads the current logic levels of all GPIO pins in the
 *                        specified GPIO port
 *
 * @param[in]           - pGPIOx : Base address of the GPIO peripheral
 *
 * @return              - uint16_t : Returns the 16-bit value of the GPIO input
 *                        data register (IDR)
 *
 * @Note                - Each bit in the returned value corresponds to the
 *                        logic level of the matching GPIO pin (Pin 0–Pin 15).
 *
 ******************************************************************************/

uint16_t GPIO_ReadFromInputPort(GPIO_RegDef_t *pGPIOx)
{
    uint16_t value;

    value = (uint16_t)pGPIOx->IDR;

    return value;
}

/******************************************************************************
 * @fn                  - GPIO_WriteToOutputPin
 *
 * @brief               - Writes a logic level to the specified GPIO output pin
 *
 * @param[in]           - pGPIOx : Base address of the GPIO peripheral
 * @param[in]           - PinNumber : GPIO pin number to be written
 * @param[in]           - Value : Logic level to write to the pin
 *                        (0 = LOW, 1 = HIGH)
 *
 * @return              - None
 *
 * @Note                - Updates the corresponding bit in the GPIO output data
 *                        register (ODR) with the specified value.
 *
 ******************************************************************************/

void GPIO_WriteToOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber,
uint8_t Value)
{
	if(Value == GPIO_PIN_SET)
	{
		// Write 1 to output data register at bit field corresponding pin number
		pGPIOx->ODR |= (1<<PinNumber);
	}else
	{
		// Write 0
		pGPIOx->ODR &= ~(1 << PinNumber);
	}
}

/******************************************************************************
 * @fn                  - GPIO_WriteToOutputPort
 *
 * @brief               - Writes a 16-bit value to the specified GPIO output port
 *
 * @param[in]           - pGPIOx : Base address of the GPIO peripheral
 * @param[in]           - Value : 16-bit value to be written to the GPIO output
 *                        data register (ODR)
 *
 * @return              - None
 *
 * @Note                - Writes the specified value to all 16 GPIO output pins
 *                        of the selected port simultaneously.
 *
 ******************************************************************************/

void GPIO_WriteToOutputPort(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber,
uint8_t Value)
{
	pGPIOx->ODR = Value;
}

/******************************************************************************
 * @fn                  - GPIO_ToggleOutputPin
 *
 * @brief               - Toggles the logic level of the specified GPIO output pin
 *
 * @param[in]           - pGPIOx : Base address of the GPIO peripheral
 * @param[in]           - PinNumber : GPIO pin number to be toggled
 *
 * @return              - None
 *
 * @Note                - Inverts the current state of the selected GPIO output
 *                        pin by toggling the corresponding bit in the GPIO
 *                        output data register (ODR).
 *
 ******************************************************************************/

void GPIO_ToggleOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber)
{
	pGPIOx->ODR ^= (1 << PinNumber);
}

/**
 * IRQ Configuration and ISR Handling
 */
/******************************************************************************
 * @fn                  - GPIO_IRQConfig
 *
 * @brief               - Configures the NVIC settings for a GPIO interrupt.
 *
 * @param[in]           - IRQNumber : IRQ number to configure
 * @param[in]           - IRQPriority : Priority level for the IRQ
 * @param[in]           - EnorDi : ENABLE or DISABLE the IRQ
 *
 * @return              - None
 *
 * @Note                - Enables or disables the specified IRQ and sets
 *                        its priority in the NVIC.
 *
 *
 *
 ******************************************************************************/
void GPIO_IRQInterruptConfig(uint8_t IRQNumber, uint8_t IRQPriority, uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	{
		if(IRQNumber <= 31)
		{
			//program ISER0 register
			*NVIC_ISER0 |= (1 << IRQNumber);
		}else if (IRQNumber > 31 && IRQNumber < 64) // 32 to 63
		{
			//program ISER1 register
			*NVIC_ISER1 |= (1 << IRQNumber % 32);
		}else if (IRQNumber >= 64 && IRQNumber < 96) // 64 to 95
		{
			//program ISER2 register
			*NVIC_ISER2 |= (1 << IRQNumber % 64);
		}
	}
	else
	{
		if(IRQNumber <= 31)
		{
			//program ICER0 register
			*NVIC_ICER0 |= (1 << IRQNumber);
		}else if (IRQNumber > 31 && IRQNumber < 64)
		{
			//program ICER1 register
			*NVIC_ICER1 |= (1 << (IRQNumber % 32));
		}else if(IRQNumber >= 6 && IRQNumber < 96)
		{
			//program ICER2 register
			*NVIC_ISER2 |= (1 << (IRQNumber % 64));
		}
	}

}

/******************************************************************************
 * @fn                  - GPIO_IRQPriorityConfig
 *
 * @brief               - Configures the priority of a GPIO interrupt in the NVIC.
 *
 * @param[in]           - IRQNumber : IRQ number whose priority is to be configured
 * @param[in]           - IRQPriority : Priority level to assign to the IRQ
 *
 * @return              - None
 *
 * @Note                - Sets the priority of the specified IRQ in the NVIC
 *                        priority register.
 *
 *
 *
 ******************************************************************************/

void GPIO_IRQPriorityConfig(uint8_t IRQNumber, uint8_t IRQPriority)
{
	// 1. First find out IPR register
	uint8_t iprx = IRQNumber / 4;
	uint8_t iprx_selection = IRQNumber % 4;

	uint8_t shift_amount = (8 * iprx_selection) + (8 - NO_PR_BITS_IMPLEMENTED);
	*(NVIC_PR_BASE_ADDR + iprx * 4) |= IRQPriority << (8 * shift_amount);
}

/******************************************************************************
 * @fn                  - GPIO_IRQHandling
 *
 * @brief               - Handles a GPIO interrupt by clearing the corresponding
 *                        EXTI pending bit.
 *
 * @param[in]           - PinNumber : GPIO pin number that generated the interrupt
 *
 * @return              - None
 *
 * @Note                - Clears the pending bit in the EXTI Pending Register (PR)
 *                        to acknowledge the interrupt and allow future interrupts.
 *
 *
 *
 ******************************************************************************/

void GPIO_IRQHandling(uint8_t PinNumber)
{
	// clear the exti pr register corresponding to the pin number
	if(EXTI -> PR & (1 << PinNumber))
	{
		// clear
		EXTI-> PR |= (1 << PinNumber);
	}
}
