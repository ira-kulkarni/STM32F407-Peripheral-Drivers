/*
 * stm32f407xx_spi_driver.c
 *
 *  Created on: Jul 28, 2026
 *      Author: irakulkarni
 */
#include "stm32f407xx_spi_driver.h"
static void spi_txe_interrupt_handle(SPI_Handle_t *pHandle);
static void spi_rxne_interrupt_handle(SPI_Handle_t *pHandle);
static void spi_ovr_err_interrupt_handle(SPI_Handle_t *pHandle);

/*
 * Peripheral clock setup
 */

/******************************************************************************
 * @fn                  - SPI_PeriClockControl
 *
 * @brief               - Enables or disables the peripheral clock for the
 *                        specified SPI peripheral.
 *
 * @param[in]           - pSPIx : Pointer to the SPI peripheral (SPI1, SPI2, SPI3)
 *
 * @param[in]           - EnorDi : ENABLE to enable the peripheral clock,
 *                                 DISABLE to disable the peripheral clock.
 *
 * @return              - None
 *
 * @Note                - The SPI peripheral clock must be enabled before
 *                        configuring or using the SPI peripheral.
 *
 ******************************************************************************/

void SPI_PeriClockControl(SPI_RegDef_t *pSPIx, uint8_t EnorDi)
{
	{
		if (EnorDi == ENABLE)
		{
			if (pSPIx == SPI1)
			{
				SPI1_PCLK_EN();
			}else if (pSPIx == SPI2)
			{
				SPI2_PCLK_EN();
			}else if (pSPIx == SPI3)
			{
				SPI3_PCLK_EN();
			}
		}
		else
		{
			//code
		}
	}
}

/*
 * Init and de-init
 */

/******************************************************************************
 * @fn                  - SPI_Init
 *
 * @brief               - Initializes the SPI peripheral using the configuration
 *                        provided by the user.
 *
 * @param[in]           - pSPIHandle : Pointer to SPI handle structure
 *
 * @return              - None
 *
 * @Note                - Configures the SPI peripheral settings such as device
 *                        mode, bus configuration, clock speed, data frame format,
 *                        clock polarity, clock phase, and software slave
 *                        management.
 *
 ******************************************************************************/

void SPI_Init(SPI_Handle_t *pSPIHandle)
{
	// First configure SPI_CR1 register
	uint32_t tempreg = 0;

	//1. Configure device mode
	tempreg |= pSPIHandle->SPIConfig.SPI_DeviceMode << 2;

	// 2. Configure the bus config
	if(pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_FD) // full duplex
	{
		//bidi mode should be cleared
		tempreg &= ~(1 << 15);
	} else if (pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_HD)
	{
		//bidi mode should be set
		tempreg |= ~(1 << 15);
	}else if (pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_SIMPLEX_RXONLY)
	{
		// bidi mode should be cleared
		tempreg &= ~(1 << 15);
		// RXONLY bit must be set
		tempreg |= ~(1 << 10);
	}

	// 3. Configure the SPI serial clock speed (baud rate)
	tempreg |= pSPIHandle->SPIConfig.SPI_SclkSpeed << 3;

	// 4. Configure the DFF
	tempreg |= pSPIHandle->SPIConfig.SPI_DFF << SPI_CR1_DFF;

	// 5. Configure the CPOL
	tempreg |= pSPIHandle->SPIConfig.SPI_CPOL << SPI_CR1_CPOL;

	// 6. Configure the CPHA
	tempreg |= pSPIHandle->SPIConfig.SPI_CPHA << SPI_CR1_CPHA ;

	pSPIHandle->pSPIx->CR1 = tempreg;
}

/******************************************************************************
 * @fn                  - SPI_DeInit
 *
 * @brief               - Resets the specified SPI peripheral to its default
 *                        reset state.
 *
 * @param[in]           - pSPIx : Pointer to the SPI peripheral (SPI1, SPI2, SPI3)
 *
 * @return              - None
 *
 * @Note                - Clears all SPI configuration settings by resetting
 *                        the peripheral through the RCC reset registers.
 *
 ******************************************************************************/

void SPI_DeInit(SPI_RegDef_t *pGPIOx)
{
}

/******************************************************************************
 * @fn                  - SPI_GetFlagStatus
 *
 * @brief               - Checks whether the specified status flag is set.
 *
 * @param[in]           - pSPIx : Pointer to the SPI peripheral.
 *
 * @param[in]           - FlagName : SPI status flag to check.
 *
 * @return              - FLAG_SET if the flag is set,
 *                        FLAG_RESET if the flag is cleared.
 *
 * @Note                - Reads the SPI Status Register (SR) to determine the
 *                        current state of the specified flag.
 *
 ******************************************************************************/
uint8_t SPI_GetFlagStatus(SPI_RegDef_t *pSPIx, uint32_t FlagName)
{
	if (pSPIx->SR & FlagName)
	{
		return FLAG_SET;
	}

	return FLAG_RESET;
}

/*
 * Data read and write
 */

/******************************************************************************
 * @fn                  - SPI_SendData
 *
 * @brief               - Transmits data over the SPI peripheral.
 *
 * @param[in]           - pSPIx : Pointer to the SPI peripheral.
 *
 * @param[in]           - pTxBuffer : Pointer to the transmit data buffer.
 *
 * @param[in]           - Len : Number of bytes to transmit.
 *
 * @return              - None
 *
 * @Note                - Waits until the transmit buffer is empty (TXE flag)
 *                        before writing each data byte to the SPI Data Register
 *                        (DR). Continues until all bytes have been transmitted.
 *
 ******************************************************************************/

void SPI_SendData(SPI_RegDef_t *pSPIx, uint8_t *pTxBuffer, uint32_t Len)
{
    while (Len > 0)
    {
        // Wait until the transmit buffer is empty
        while (SPI_GetFlagStatus(pSPIx, SPI_TXE_FLAG) == FLAG_RESET);

        // Check whether SPI is in 16-bit mode
        if (pSPIx->CR1 & (1U << SPI_CR1_DFF))
        {
            // Load 16 bits into the data register
            pSPIx->DR = *((uint16_t *)pTxBuffer);

            // Two bytes were sent
            Len -= 2;

            // Move forward by two bytes
            pTxBuffer += 2;
        }
        else
        {
            // Load 8 bits into the data register
            pSPIx->DR = *pTxBuffer;

            // One byte was sent
            Len--;

            // Move forward by one byte
            pTxBuffer++;
        }
    }
}

/******************************************************************************
 * @fn                  - SPI_ReceiveData
 *
 * @brief               - Receives data from the SPI peripheral.
 *
 * @param[in]           - pSPIx : Pointer to the SPI peripheral.
 *
 * @param[in]           - pRxBuffer : Pointer to the receive data buffer.
 *
 * @param[in]           - Len : Number of bytes to receive.
 *
 * @return              - None
 *
 * @Note                - Waits until the receive buffer is not empty (RXNE flag)
 *                        before reading each data byte from the SPI Data Register
 *                        (DR). Continues until the requested number of bytes
 *                        have been received.
 *
 ******************************************************************************/

void SPI_ReceiveData(SPI_RegDef_t *pSPIx, uint8_t *pRxBuffer, uint32_t Len)
{
	while(Len > 0)
	{
		// 1. Wait until RXNE is set
		while(SPI_GetFlagStatus(pSPIx, SPI_RXNE_FLAG) == FLAG_RESET);

		// 2. Check the DFF bit
		if(pSPIx->CR1 & (1 << SPI_CR1_DFF))
		{
			// 16 bit DFF
			// 1. Load data into DR
			*((uint16_t*)pRxBuffer) = pSPIx->DR;
			Len--;
			Len--;
			(uint16_t*)pRxBuffer++;
		}else
		{
			// 8 bit DFF
			*((uint16_t*)pRxBuffer) = pSPIx->DR;
			Len--;
			pRxBuffer++;
		}
	}
}

/**
 * IRQ Configuration and ISR Handling
 */

/******************************************************************************
 * @fn                  - SPI_InterruptIRQConfig
 *
 * @brief               - Configures the SPI interrupt in the NVIC by enabling
 *                        or disabling it and setting its priority.
 *
 * @param[in]           - IRQNumber : IRQ number of the SPI peripheral.
 *
 * @param[in]           - IRQPriority : Priority level to assign to the interrupt.
 *
 * @param[in]           - EnorDi : ENABLE to enable the interrupt,
 *                                 DISABLE to disable the interrupt.
 *
 * @return              - None
 *
 * @Note                - Configures the Nested Vectored Interrupt Controller
 *                        (NVIC) for SPI interrupt handling.
 *
 ******************************************************************************/
void SPI_InterruptIRQConfig(uint8_t IRQNumber, uint8_t IRQPriority, uint8_t EnorDi)
{
}

/******************************************************************************
 * @fn                  - SPI_IRQPriorityConfig
 *
 * @brief               - Configures the priority of the specified SPI interrupt.
 *
 * @param[in]           - IRQNumber : IRQ number of the SPI peripheral.
 *
 * @param[in]           - IRQPriority : Priority level to assign to the interrupt.
 *
 * @return              - None
 *
 * @Note                - Sets the interrupt priority in the Nested Vectored
 *                        Interrupt Controller (NVIC). A lower priority value
 *                        corresponds to a higher interrupt priority.
 *
 ******************************************************************************/

void SPI_IRQPriorityConfig(uint8_t IRQNumber, uint8_t IRQPriority)
{

}

uint8_t SPI_SendDataIT(SPI_Handle_t *pSPIHandle, uint8_t *pTxBuffer,uint32_t Len)
{
    uint8_t state = pSPIHandle->TxState;

    if (state != SPI_BUSY_IN_TX)
    {
        // Save transmit buffer and length
        pSPIHandle->pTxBuffer = pTxBuffer;
        pSPIHandle->TxLen = Len;

        // Mark SPI as busy transmitting
        pSPIHandle->TxState = SPI_BUSY_IN_TX;

        // Enable TXE interrupt
        pSPIHandle->pSPIx->CR2 |= (1 << SPI_CR2_TXEIE);
    }

    return state;
}

uint8_t SPI_ReceiveDataIT(SPI_Handle_t *pSPIHandle, uint8_t *pRxBuffer,uint32_t Len)
{
    uint8_t state = pSPIHandle->RxState;

    // Proceed only if SPI is not already busy receiving
    if (state != SPI_BUSY_IN_RX)
    {
        // Save the receive buffer address
        pSPIHandle->pRxBuffer = pRxBuffer;

        // Save the number of bytes to receive
        pSPIHandle->RxLen = Len;

        // Mark SPI as busy receiving
        pSPIHandle->RxState = SPI_BUSY_IN_RX;

        // Enable the RXNE interrupt
        pSPIHandle->pSPIx->CR2 |= (1 << SPI_CR2_RXNEIE);
    }

    return state;
}

/******************************************************************************
 * @fn                  - SPI_IRQHandling
 *
 * @brief               - Handles SPI interrupt events and services the
 *                        corresponding interrupt source.
 *
 * @param[in]           - pHandle : Pointer to the SPI handle structure.
 *
 * @return              - None
 *
 * @Note                - Checks the SPI interrupt flags and performs the
 *                        appropriate action, such as handling transmit,
 *                        receive, or error interrupts.
 *
 ******************************************************************************/

void SPI_IRQHandling(SPI_Handle_t *pHandle)
{
    uint8_t temp1, temp2;

    // Check for TXE interrupt
    temp1 = pHandle->pSPIx->SR & (1 << SPI_SR_TXE);
    temp2 = pHandle->pSPIx->CR2 & (1 << SPI_CR2_TXEIE);

    if (temp1 && temp2)
    {
        spi_txe_interrupt_handle(pHandle);
    }

    // Check for RXNE interrupt
    temp1 = pHandle->pSPIx->SR & (1 << SPI_SR_RXNE);
    temp2 = pHandle->pSPIx->CR2 & (1 << SPI_CR2_RXNEIE);

    if (temp1 && temp2)
    {
        spi_rxne_interrupt_handle(pHandle);
    }

    // Check for overrun error interrupt
    temp1 = pHandle->pSPIx->SR & (1 << SPI_SR_OVR);
    temp2 = pHandle->pSPIx->CR2 & (1 << SPI_CR2_ERRIE);

    if (temp1 && temp2)
    {
        spi_ovr_err_interrupt_handle(pHandle);
    }
}

// Some helper function implementation
static void spi_txe_interrupt_handle(SPI_Handle_t *pSPIHandle)
{
	// 2. Check the DFF bit in CR1
	if((pSPIHandle->pSPIx->CR1 & (1 << SPI_CR1_DFF)))
	{
		// 16 bit DFF
		// 1. Load data into DR
		pSPIHandle->pSPIx->DR = *((uint16_t*)pSPIHandle->pTxBuffer);
		pSPIHandle->TxLen--;
		pSPIHandle->TxLen--;
		(uint16_t*)pSPIHandle->pTxBuffer;
	}else
	{
		// 8 bit DFF
		pSPIHandle->pSPIx->DR = *pSPIHandle->pTxBuffer;
		pSPIHandle->TxLen--;
		pSPIHandle->pTxBuffer;
	}
	if (! pSPIHandle->TxLen)
	{
		//TxLen is zero, so close the SPI transmission and inform the application
		// that TX is over
		// This prevents interrupts from setting up of TXE flag
		SPI_CloseTransmission(pSPIHandle);
		SPI_ApplicationEventCallback(pSPIHandle, SPI_EVENT_TX_COMPLT);

	}
}

static void spi_rxne_interrupt_handle(SPI_Handle_t *pSPIHandle)
{
    if (pSPIHandle->pSPIx->CR1 & (1 << SPI_CR1_DFF))
    {
        *((uint16_t *)pSPIHandle->pRxBuffer) =
                (uint16_t)pSPIHandle->pSPIx->DR;

        pSPIHandle->RxLen -= 2;
        pSPIHandle->pRxBuffer += 2;
    }
    else
    {
        *(pSPIHandle->pRxBuffer) =
                (uint8_t)pSPIHandle->pSPIx->DR;

        pSPIHandle->RxLen--;
        pSPIHandle->pRxBuffer++;
    }

    if (!pSPIHandle->RxLen)
    {
        SPI_CloseReception(pSPIHandle);
        SPI_ApplicationEventCallback(pSPIHandle, SPI_EVENT_RX_COMPLT);
    }
}

static void spi_ovr_err_interrupt_handle(SPI_Handle_t *pHandle)
{
    uint8_t temp;

    if (pHandle->TxState != SPI_BUSY_IN_TX)
    {
        temp = pHandle->pSPIx->DR;
        temp = pHandle->pSPIx->SR;
    }

    (void)temp;

    SPI_ApplicationEventCallback(pHandle, SPI_EVENT_OVR_ERR);
}

void SPI_CloseTransmission(SPI_Handle_t *pSPIHandle)
{
    pSPIHandle->pSPIx->CR2 &= ~(1 << SPI_CR2_TXEIE);
    pSPIHandle->pTxBuffer = NULL;
    pSPIHandle->TxLen = 0;
    pSPIHandle->TxState = SPI_READY;
}

void SPI_CloseReception(SPI_Handle_t *pSPIHandle)
{
    pSPIHandle->pSPIx->CR2 &= ~(1 << SPI_CR2_RXNEIE);
    pSPIHandle->pRxBuffer = NULL;
    pSPIHandle->RxLen = 0;
    pSPIHandle->RxState = SPI_READY;
}

void SPI_ClearOVRFlag(SPI_RegDef_t *pSPIx)
{
	uint8_t temp;
	temp = pSPIx->DR;
	temp = pSPIx->SR;
	(void)temp;
}

__weak void SPI_ApplicationEventCallback(SPI_Handle_t *pSPIHandle, uint8_t AppEv)
{
	// This is a weak implementation.
}
