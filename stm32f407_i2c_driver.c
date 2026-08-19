/*
 * stm32f407xx_i2c_driver.c
 *
 *  Created on: Jul 30, 2026
 *      Author: irakulkarni
 */
#include "stm32f407xx_i2c_driver.h"

uint16_t AHB_PreScalar[8] = { 2, 4, 8, 16, 64, 128, 256, 512 };
uint16_t APB1_PreScalar[4] = { 2, 4, 8, 16 };

static void I2C_GenerateStartCondition(I2C_RegDef_t *pI2Cx);
static void I2C_ExecuteAddressPhaseWrite(I2C_RegDef_t *pI2Cx, uint8_t SlaveAddr);
static void I2C_ExecuteAddressPhaseRead(I2C_RegDef_t *pI2Cx, uint8_t SlaveAddr);
static void I2C_ClearADDRFlag(I2C_Handle_t *pI2CHandle);
static void I2C_GenerateStopCondition(I2C_RegDef_t *pI2Cx);
		uint8_t Len, uint8_t SlaveAddr);
uint8_t I2C_MasterReceiveDataIT(I2C_Handle_t **pI2CHandle, uint8_t *pRxBuffer,
		uint8_t Len, uint8_t SlaveAddr);

void I2C_PeriClockControl(I2C_RegDef_t *pI2Cx, uint8_t EnorDi) {
	if (EnorDi == ENABLE) {
		if (pI2Cx == I2C1) {
			I2C1_PCLK_EN();
		} else if (pI2Cx == I2C2) {
			I2C2_PCLK_EN();
		} else if (pI2Cx == I2C3) {
			I2C3_PCLK_EN();
		}
	} else {
		if (pI2Cx == I2C1) {
			I2C1_PCLK_DI();
		} else if (pI2Cx == I2C2) {
			I2C2_PCLK_DI();
		} else if (pI2Cx == I2C3) {
			I2C3_PCLK_DI();
		}
	}
}

uint32_t RCC_GetPCLK1Value(void) {
	uint32_t pclk1;
	uint32_t SystemClk;
	uint8_t clksrc;
	uint8_t temp;
	uint16_t ahbp;
	uint8_t apb1;

	clksrc = ((RCC->CFGR >> 2) & 0x3);

	if (clksrc == 0) {
		SystemClk = 16000000;
	} else if (clksrc == 1) {
		SystemClk = 8000000;
	} else if (clksrc == 2) {
		SystemClk = 16000000;
	}

	// Find the AHB prescaler
	temp = ((RCC->CFGR >> 4) & 0xF);

	if (temp < 8) {
		ahbp = 1;
	} else {
		ahbp = AHB_PreScalar[temp - 8];
	}

	// Find the APB1 prescaler
	temp = ((RCC->CFGR >> 10) & 0x7);

	if (temp < 4) {
		apb1 = 1;
	} else {
		apb1 = APB1_PreScalar[temp - 4];
	}

	pclk1 = (SystemClk / ahbp) / apb1;

	return pclk1;
}

void I2C_DeInit(I2C_RegDef_t *pI2Cx);

uint8_t I2C_GetFlagStatus(I2C_RegDef_t *pI2Cx, uint32_t FlagName) {
	if (pI2Cx->SR1 & FlagName) {
		return FLAG_SET;
	}

	return FLAG_RESET;
}

void I2C_MasterSendData(I2C_Handle_t *pI2CHandle, uint8_t *pTxBuffer,
		uint8_t Len, uint8_t SlaveAddr) {
	// 1. Generate START
	I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

	// 2. Wait until START is generated
	while (!(I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_SB)))
		;

	// 3. Send slave address with write bit
	I2C_ExecuteAddressPhaseWrite(pI2CHandle->pI2Cx, SlaveAddr);

	// 4. Wait until address phase is complete
	while (!(I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_ADDR)))
		;

	// 5. Clear ADDR flag
	I2C_ClearADDRFlag(pI2CHandle->pI2Cx);

	// 6. Send all data
	while (Len > 0) {
		while (!(I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_TXE)))
			;

		pI2CHandle->pI2Cx->DR = *pTxBuffer;
		pTxBuffer++;
		Len--;
	}

	// 7. Wait until transmission is complete
	while (!(I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_TXE)))
		;
	while (!(I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_BTF)))
		;

	// 8. Generate STOP
	I2C_GenerateStopCondition(pI2CHandle->pI2Cx);
}

void I2C_MasterReceiveData(I2C_Handle_t **pI2CHandle, uint8_t *pRxBuffer,
		uint8_t Len, uint8_t SlaveAddr) {
	// 1. Generate START condition
	I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

	// 2. Confirm that start generation is completed by checking the SB flag in SR1
	// Note: Until SB is cleared SCL will be stretched (pulled to LOW)
	while (!(I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_SB)))
		;

	// 3. Send the address of the slave with r/nw bit set to R(1) (total 8 bits)
	I2C_ExecuteAddressPhaseRead(pI2CHandle->pI2Cx, SlaveAddr);

	// 4. Wait until address phase is completed by checking the ADDR flag in the SR1
	while (!(I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_ADDR)))
		;

	// procedure to read only 1 byte from slave
	if (Len == 1) {
		// Disable acking
		I2C_ManageAcking(pI2CHandle->pI2Cx, I2C_ACK_DISABLE);

		// Clear the ADDR flag
		I2C_ClearADDRFlag(pI2CHandle);

		// Wait until RXNE becomes 1
		while (!I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_RXNE))
			;

		// Generate STOP condition
		I2C_GenerateStopCondition(pI2CHandle->pI2Cx);

		// Read data in to buffer
		*pRxBuffer = pI2CHandle->pI2Cx->DR;
	}
	// procedure to read data from slave when Len > 1
	if (Len > 1) {
		// clear the ADDR flag
		I2C_ClearADDRFlag(pI2CHandle);

		// read the data until Len becomes 0
		for (uint32_t i = Len; i > 0; i--) {
			// wait until RXNE becomes 1
			while (!I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_RXNE))
				;

			if (i == 2) // if last 2 bytes are remaining
					{
				// Disable acking
				I2C_ManageAcking(pI2CHandle->pI2Cx, I2C_ACK_DISABLE);

				// generate STOP condition
				I2C_GenerateStopCondition(pI2CHandle->pI2Cx);
			}

			// read the data from data register into buffer
			*pRxBuffer = pI2CHandle->pI2Cx->DR;

			// increment the buffer address
			*pRxBuffer++;
		}
	}
	// re-enable ACKing
	if (pI2CHandle->I2C_Config.I2C_AckControl == I2C_ACK_ENABLE) {
		I2C_ManageAcking(pI2CHandle->pI2Cx, I2C_ACK_ENABLE);
	}
}

void I2C_GenerateStartCondition(I2C_RegDef_t *pI2Cx) {
	pI2Cx->CR1 |= (1 << I2C_CR1_START);
}

static void I2C_ExecuteAddressPhaseWrite(I2C_RegDef_t *pI2Cx, uint8_t SlaveAddr) {
	SlaveAddr = SlaveAddr << 1;
	SlaveAddr &= ~(1); // SlaveAddr is Slave address + r/nw bit = 0
	pI2Cx->DR = SlaveAddr;
}

static void I2C_ClearADDRFlag(I2C_Handle_t *pI2CHandle) {
	uint32_t dummy_read;

	// Check for device mode
	if (pI2CHandle->pI2Cx->SR2 & (1 << I2C_SR2_MSL)) {
		if (pI2CHandle->TxRxState == I2C_BUSY_IN_RX) {
			if (pI2CHandle->RxSize == 1) {
				// First disable the ACK
				I2C_ManageAcking(pI2CHandle->pI2Cx, DISABLE);

				// Clear the ADDR flag (read SR1, read SR2)
				dummy_read = pI2CHandle->pI2Cx->Sr1;
				dummy_read = pI2CHandle->pI2Cx->Sr2;
				(void) dummy_read;
			}
		}
	} else {
		// Device is slave mode
		// Clear the ADDR flag (read SR1, read SR2)
		dummy_read = pI2CHandle->pI2Cx->Sr1;
		dummy_read = pI2CHandle->pI2Cx->Sr2;
		(void) dummy_read;

	}
}

static void I2C_GenerateStopCondition(I2C_RegDef_t *pI2Cx) {
	pI2Cx->CR1 |= (1 << I2C_CR1_STOP);
}

static void I2C_ExecuteAddressPhaseRead(I2C_RegDef_t *pI2Cx, uint8_t SlaveAddr) {
	SlaveAddr = SlaveAddr << 1;
	SlaveAddr |= 1; // SlaveAddr is Slave address + r/nw bit = 1
	pI2Cx->DR = SlaveAddr;
}

void I2C_ManageAcking(I2C_RegDef_t *pI2Cx, uint8_t EnorDi) {
	if (EnorDi == I2C_ACK_ENABLE) {
		// enable the ack
		pI2Cx->CR1 |= (1 << I2C_CR1_ACK)
	} else {
		// disable the ack
		pI2Cx->CR1 &= ~(1 << I2C_CR1_ACK);
	}
}

uint8_t I2C_MasterSendDataIT(I2C_Handle_t *pI2CHandle, uint8_t *pTxBuffer,
		uint32_t Len, uint8_t SlaveAddr, uint8_t Sr) {
	uint8_t busystate = pI2CHandle->TxRxState;

	if ( (busystate != I2C_BUSY_IN_TX) && (busystate != I2C_BUSY_IN_RX)) {
		pI2CHandle->pTxBuffer = pTxBuffer;
		pI2CHandle->TxLen = Len;
		pI2CHandle->TxRxState = I2C_BUSY_IN_TX;
		pI2CHandle->DevAddr = SlaveAddr;
		pI2CHandle->Sr = Sr;

		//Implement code to Generate START Condition
		I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

		//Implement the code to enable ITBUFEN Control Bit
		pI2CHandle->pI2Cx->CR2 |= (1 << I2C_CR2_ITBUFEN);

		//Implement the code to enable ITEVFEN Control Bit
		pI2CHandle->pI2Cx->CR2 |= (1 << I2C_CR2_ITEVTEN);

		//Implement the code to enable ITERREN Control Bit
		pI2CHandle->pI2Cx->CR2 |= (1 << I2C_CR2_ITERREN);

	}

	return busystate;

}

uint8_t I2C_MasterReceiveDataIT(I2C_Handle_t *pI2CHandle, uint8_t *pRxBuffer,
		uint32_t Len, uint8_t SlaveAddr, uint8_t Sr) {

	uint8_t busystate = pI2CHandle->TxRxState;

	if ( (busystate != I2C_BUSY_IN_TX) && (busystate != I2C_BUSY_IN_RX)) {
		pI2CHandle->pRxBuffer = pRxBuffer;
		pI2CHandle->RxLen = Len;
		pI2CHandle->TxRxState = I2C_BUSY_IN_RX;
		pI2CHandle->RxSize = Len; //Rxsize is used in the ISR code to manage the data reception
		pI2CHandle->DevAddr = SlaveAddr;
		pI2CHandle->Sr = Sr;

		//Implement code to Generate START Condition
		I2C_GenerateStartCondition(pI2CHandle->pI2Cx)

		//Implement the code to enable ITBUFEN Control Bit
		pI2CHandle->pI2Cx->CR2 |= (1 << I2C_CR2_ITBUFEN);

		//Implement the code to enable ITEVFEN Control Bit
		pI2CHandle->pI2Cx->CR2 |= (1 << I2C_CR2_ITEVTEN);

		//Implement the code to enable ITERREN Control Bit
		pI2CHandle->pI2Cx->CR2 |= (1 << I2C_CR2_ITERREN);
	}

	return busystate;
}

void I2C_SlaveSendData(I2C_RegDef_t *pI2C, uint8_t data)
{
	pI2C->DR = data;
}

uint8_t I2C_SlaveReceiveData(I2C_RegDef_t *pI2C)
{
	return (uint8_t)pI2C->DR;
}


void I2C_EV_IRQHandling(I2C_Handle_t **pI2CHandle) {
	// Interrupt handling for both master and slave mode of a device
	uint32_t temp1, temp2, temp3;
	temp1 = pI2CHandle->pI2Cx->CR2 & (1 << I2C_CR2_ITEVTEN);
	temp2 = I2CHandle->pI2Cx->CR2 & (1 << I2C_CR2_ITBUFEN);
	temp3 = pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_SB);

	// 1. Handle for interrupt generated by SB event
	// Note: SB flag is only applicable in Master mode
	if (temp1 && temp2) {
		// The interrupt is generated because of SB flag
		// This block will not be executed in slave mode becauyse for slave SB is always zero
		// In this block lets execute the address phase
		if (pI2CHandle->TxRxState == I2C_BUSY_IN_TX) {
			I2C_ExecuteAddressPhaseWrite(pI2CHandle->pI2Cx,
					pI2CHandle->DevAddr);
		} else if (pI2CHandle->TxRxState == I2C_BUSY_IN_RX) {
			I2C_ExecuteAddressPhaseRead(I2CHandle->pI2CHandle->DevAddr);
		}
	}
	temp3 = pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_ADDR);

	// 2. Handle for interrupt generated by ADDR event
	// Note: when master mode: address is sent
	// When slave mode: address matched with own address
	if (temp1 && temp3) {
		// interrupt is generated because of ADDR event
		I2C_ClearADDRFlag(pI2CHandle);
	}
	temp3 = pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_BTF);

	// 3. Handle for interrupt generated by BTF (Byte Transfer Finished) event
	if (temp1 && temp3) {
		// BTF flag is set
		if (pI2CHandle->TxRxState == I2C_BUSY_IN_TX) {
			// make sure that TXE is also set
			if (pI2CHandle->I2Cx->SR1 & (1 << I2C_SR1_TXE)) {
				// BTF, TXE = 1
				if (pI2CHandle->TxLen == 0) {
					// 1. Generate STOP condition
					if (pI2CHandle->Sr == I2C_DISABLE_SR) {
						I2C_GenerateStopCondition(pI2CHandle->I2Cx);
					}

					// 2. Reset all the member elements of the handle structure
					I2C_CloseSendData();

					// 3. Notify the application about transmission complete
					I2C_ApplicationEventCallback(pI2CHandle, I2C_EV_TX_CMPLT);
				}
			} else if (pI2CHandle->TxRxState == I2C_BUSY_IN_RX) {

			}
		}
	}
	temp3 = pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_STOPF);

	// 4. Handle for interrupt generated by STOP event
	// Note: stop detection flag is applicable only slave mode.
	if (temp1 && temp2 && temp3) {
		// STOPF flag is set
		// Clear the STOPF (read SR1, write to CR1)
		pI2CHandle->pI2Cx->CR1 |= 0x0000;

		// Notify the application that STOP is detected
		I2C_ApplicationEventCallback(pI2CHandle, I2C_EV_STOP);

	}
	temp3 = pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_TXE);

	// 5. Handle for interrupt generated by TXE event
	if (temp1 && temp2 && temp3) {
		// TXE flag is set
		// We have to do the data transmission
		if (pI2CHandle->TxRxState == I2C_BUSY_IN_TX) {
			if (pI2CHanlde->Tx > 0) {
				// 1. Load data into DR
				pI2CHanlde->pI2Cx->DR = *(pI2CHandle->pTxBuffer);

				// 2. Decrement the TxLen
				pI2CHandle->TxLen--;

				// 2. Increment the buffer address
				pI2CHandle->pTxBuffer++;
			}
		}
	}
	temp3 = pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_RXNE);

	// 6. Handle for interrupt generated by RXNE event
	if (temp1 && temp2 && temp3) {
		if (pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR2_RXNE)) {
			// RXNE flag is set
			if (pI2CHandle->TxRxState == I2C_BUSY_IN_RX) {
				// We have to do data reception
				if (pI2CHandle->RxSize == 1) {
					*pI2CHandle->pRxBuffer = pI2CHandle->pI2Cx->DR;
				}
				if (pI2CHandle->RxSize == 1) {
					*pI2CHandle->pRxBuffer = pI2CHandle->pI2Cx->DR;
					pI2CHanlde->RxLen--;
				}

				if (pI2CHandle->RxSize > 1) {
					if (pI2CHandle->RxLen == 2) {
						// clear the ack bit
						I2C_ManageAcking(pI2CHandle->pI2Cx, DISABLE);

						// read DR
						*pI2CHandle->pRxBuffer = pI2CHandle->pI2C->DR;
						pI2CHandle->pRxBuffer++;
						pI2CHandle->RxLen--;
					}
					if (pI2CHandle->RxLen == 0) {
						// close the I2C data reception and notify the application

						// 1. generate the STOP condition
						if (pI2CHandle->Sr == I2C_DISABLE_SR) {
							I2C_GenerateStopCondition(pI2CHandle->pI2Cx);
						}

						// 2. close the I2C RX
						I2C_CloseReceiveData();

						// 3. Notify the application
						I2C_ApplicationEventCallback(pI2CHandle,
								I2C_EV_RX_COMPLT);
					}
				}
			}
		}
	}
}
void I2C_ER_IRQHandling(I2C_Handle_t* *pI2CHandle)

