/*
  Copyright 2013-2014 bcstack.org

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include "stm32f0xx.h"
#include "stm32f0xx_hal.h"
#include "board.h"
#include "msp.h"

void Error_Handler( void );
void SystemClock_Config(void);

static UART_HandleTypeDef BtUart;

void board_setup(void)
{
    HAL_Init();

    SystemClock_Config();
}

void bt_reset( void )
{
    GPIO_InitTypeDef  GPIO_InitStruct;

    BT_RESET_GPIO_CLK_ENABLE();

    GPIO_InitStruct.Pin = BT_RESET_PIN;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(BT_RESET_GPIO_PORT, &GPIO_InitStruct);

    HAL_GPIO_WritePin(BT_RESET_GPIO_PORT, BT_RESET_PIN, GPIO_PIN_RESET);

    HAL_Delay(100);

    HAL_GPIO_WritePin(BT_RESET_GPIO_PORT, BT_RESET_PIN, GPIO_PIN_SET);
}

void bt_uart_setup(uint32_t baudrate)
{
    BtUart.Instance        = BTUART;

    BtUart.Init.BaudRate   = baudrate;
    BtUart.Init.WordLength = UART_WORDLENGTH_8B;
    BtUart.Init.StopBits   = UART_STOPBITS_1;
    BtUart.Init.Parity     = UART_PARITY_NONE;
  	BtUart.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
    //BtUart.Init.HwFlowCtl  = UART_HWCONTROL_RTS_CTS;
    BtUart.Init.Mode       = UART_MODE_TX_RX;
    BtUart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    if(HAL_UART_DeInit(&BtUart) != HAL_OK)
    {
        Error_Handler();
    }
    if(HAL_UART_Init(&BtUart) != HAL_OK)
    {
        Error_Handler();
    }
}

void bt_uart_tx(const uint8_t* buffer, uint16_t len)
{
    HAL_UART_Transmit_DMA(&BtUart, (uint8_t*)buffer, len);
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    bt_uart_tx_done();
}

void bt_uart_rx(uint8_t* buffer, uint16_t len)
{
    HAL_UART_Receive_DMA(&BtUart, (uint8_t*)buffer, len);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    bt_uart_rx_done();
}

void Error_Handler( void )
{
    while (1);
}

#ifdef  USE_FULL_ASSERT

void assert_failed(uint8_t* file, uint32_t line)
{ 
    while (1)
    {
    }
}
#endif

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
  static DMA_HandleTypeDef hdma_tx;
  static DMA_HandleTypeDef hdma_rx;
  
  GPIO_InitTypeDef  GPIO_InitStruct;
  
  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* Enable GPIO TX/RX clock */
  BTUART_TX_GPIO_CLK_ENABLE();
  BTUART_RX_GPIO_CLK_ENABLE();

  /* Enable BTUART clock */
  BTUART_CLK_ENABLE(); 

  /* Enable DMA clock */
  DMAx_CLK_ENABLE();   
  
  /*##-2- Configure peripheral GPIO ##########################################*/  
  /* UART TX GPIO pin configuration  */
  GPIO_InitStruct.Pin       = BTUART_TX_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Alternate = BTUART_TX_AF;
  
  HAL_GPIO_Init(BTUART_TX_GPIO_PORT, &GPIO_InitStruct);
    
  /* UART RX GPIO pin configuration  */
  GPIO_InitStruct.Pin = BTUART_RX_PIN;
  GPIO_InitStruct.Alternate = BTUART_RX_AF;
    
  HAL_GPIO_Init(BTUART_RX_GPIO_PORT, &GPIO_InitStruct);

  /* UART CTS GPIO pin configuration  */
  GPIO_InitStruct.Pin = BTUART_CTS_PIN;
  GPIO_InitStruct.Alternate = BTUART_CTS_AF;
    
  HAL_GPIO_Init(BTUART_CTS_GPIO_PORT, &GPIO_InitStruct);

  /* UART RTS GPIO pin configuration  */
  GPIO_InitStruct.Pin = BTUART_RTS_PIN;
  GPIO_InitStruct.Alternate = BTUART_RTS_AF;
    
  HAL_GPIO_Init(BTUART_RTS_GPIO_PORT, &GPIO_InitStruct);

  /*##-3- Configure the DMA ##################################################*/
  /* Configure the DMA handler for Transmission process */
  hdma_tx.Instance                 = BTUART_TX_DMA_STREAM;

  hdma_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
  hdma_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
  hdma_tx.Init.MemInc              = DMA_MINC_ENABLE;
  hdma_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdma_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
  hdma_tx.Init.Mode                = DMA_NORMAL;
  hdma_tx.Init.Priority            = DMA_PRIORITY_LOW;

  HAL_DMA_Init(&hdma_tx);

  /* Associate the initialized DMA handle to the UART handle */
  __HAL_LINKDMA(huart, hdmatx, hdma_tx);
    
  /* Configure the DMA handler for reception process */
  hdma_rx.Instance                 = BTUART_RX_DMA_STREAM;
  
  hdma_rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
  hdma_rx.Init.PeriphInc           = DMA_PINC_DISABLE;
  hdma_rx.Init.MemInc              = DMA_MINC_ENABLE;
  hdma_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdma_rx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
  hdma_rx.Init.Mode                = DMA_NORMAL;
  hdma_rx.Init.Priority            = DMA_PRIORITY_HIGH;

  HAL_DMA_Init(&hdma_rx);
    
  /* Associate the initialized DMA handle to the the UART handle */
  __HAL_LINKDMA(huart, hdmarx, hdma_rx);
    
  /*##-4- Configure the NVIC for DMA #########################################*/
  /* NVIC configuration for DMA transfer complete interrupt (BTUART_TX) */
  HAL_NVIC_SetPriority(BTUART_DMA_TX_IRQn, 0, 1);
  HAL_NVIC_EnableIRQ(BTUART_DMA_TX_IRQn);
    
  /* NVIC configuration for DMA transfer complete interrupt (BTUART_RX) */
  HAL_NVIC_SetPriority(BTUART_DMA_RX_IRQn, 0, 0);   
  HAL_NVIC_EnableIRQ(BTUART_DMA_RX_IRQn);
}

/**
  * @brief UART MSP De-Initialization 
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  *          - Revert GPIO, DMA and NVIC configuration to their default state
  * @param huart: UART handle pointer
  * @retval None
  */
void HAL_UART_MspDeInit(UART_HandleTypeDef *huart)
{

  /*##-1- Reset peripherals ##################################################*/
  BTUART_FORCE_RESET();
  BTUART_RELEASE_RESET();

  /*##-2- Disable peripherals and GPIO Clocks #################################*/
  /* Configure UART Tx as alternate function  */
  HAL_GPIO_DeInit(BTUART_TX_GPIO_PORT, BTUART_TX_PIN);
  /* Configure UART Rx as alternate function  */
  HAL_GPIO_DeInit(BTUART_RX_GPIO_PORT, BTUART_RX_PIN);
   
  /*##-3- Disable the DMA ####################################################*/
  /* De-Initialize the DMA channel associated to reception process */
  if(huart->hdmarx != 0)
  {
    HAL_DMA_DeInit(huart->hdmarx);
  }
  /* De-Initialize the DMA channel associated to transmission process */
  if(huart->hdmatx != 0)
  {
    HAL_DMA_DeInit(huart->hdmatx);
  }  
  
  /*##-4- Disable the NVIC for DMA ###########################################*/
  HAL_NVIC_DisableIRQ(BTUART_DMA_TX_IRQn);
  HAL_NVIC_DisableIRQ(BTUART_DMA_RX_IRQn);
}

void BTUART_DMA_RX_IRQHandler(void)
{
    HAL_DMA_IRQHandler(BtUart.hdmatx);
    HAL_DMA_IRQHandler(BtUart.hdmarx);
}

void delayms(uint16_t time)
{
    HAL_Delay(time);
}

void NMI_Handler(void)
{
}

void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

void SVC_Handler(void)
{
}

void PendSV_Handler(void)
{
}

void SysTick_Handler(void)
{
    HAL_IncTick();
}

void EXTI0_1_IRQHandler(void)
{
}

void RTC_IRQHandler(void)
{ 
}

void TIM14_IRQHandler(void)
{
}
