/*
 * stm32f407xx_USART_driver.h
 *
 *  Created on: Aug 3, 2026
 *      Author: irakulkarni
 */

#ifndef INC_STM32F407XX_USART_DRIVER_H_
#define INC_STM32F407XX_USART_DRIVER_H_

/*
 * Configuration structure for USARTx peripheral
 */
typedef struct
{
	uint8_t USART_Mode;
	uint8_t USART_Baud;
	uint8_t USART_NoOfStopBits;
	uint8_t USART_WordLength;
	uint8_t USART_ParityControl;
	uint8_t USART_HWFlowControl;
}USART_Config_t;

/*
 * Handle structure for USARTx peripheral
 */
typedef struct
{
	USART_RegDef_T *pUSARTx;
	USART_Config_t USART_Config;
}USART_Config_t;

/*
 * APIs
 */
void USART_PeriClockControl(USART_RegDef_t *pUSARTx, uint8_t EnOrDi);
void USART_PeripheralControl(USART_RegDef_t *pUSARTx, uint8_t EnOrDi);
uint8_t USART_GetFlagStatus(USART_RegDef_t *pUSARTx, uint8_t StatusFlagName);
void USART_ClearFlag(USART_RegDef_t *pUSARTx, uint16_t StatusFlagName);
void USART_IRQInterruptConfig(uint8_t IRQNumber, uint8_t, EnorDi);
void USART_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority);

#endif /* INC_STM32F407XX_USART_DRIVER_H_ */
