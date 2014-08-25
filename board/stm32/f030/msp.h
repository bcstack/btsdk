#ifndef _MSP_STM32F030_H_
#define _MSP_STM32F030_H_

#define BTUART                           USART1
#define BTUART_CLK_ENABLE()              __USART1_CLK_ENABLE()
#define DMAx_CLK_ENABLE()                __DMA1_CLK_ENABLE()
#define BTUART_RX_GPIO_CLK_ENABLE()      __GPIOA_CLK_ENABLE()
#define BTUART_TX_GPIO_CLK_ENABLE()      __GPIOA_CLK_ENABLE()

#define BTUART_FORCE_RESET()             __USART1_FORCE_RESET()
#define BTUART_RELEASE_RESET()           __USART1_RELEASE_RESET()

#define BTUART_CTS_PIN                   GPIO_PIN_0
#define BTUART_CTS_GPIO_PORT             GPIOA
#define BTUART_CTS_AF                    GPIO_AF1_USART1
#define BTUART_RTS_PIN                   GPIO_PIN_1
#define BTUART_RTS_GPIO_PORT             GPIOA
#define BTUART_RTS_AF                    GPIO_AF1_USART1
#define BTUART_TX_PIN                    GPIO_PIN_2
#define BTUART_TX_GPIO_PORT              GPIOA
#define BTUART_TX_AF                     GPIO_AF1_USART1
#define BTUART_RX_PIN                    GPIO_PIN_3
#define BTUART_RX_GPIO_PORT              GPIOA
#define BTUART_RX_AF                     GPIO_AF1_USART1

#define BT_RESET_PIN                    GPIO_PIN_5
#define BT_RESET_GPIO_PORT              GPIOA
#define BT_RESET_GPIO_CLK_ENABLE()  __GPIOA_CLK_ENABLE()
#define BT_RESET_GPIO_CLK_DISABLE() __GPIOA_CLK_DISABLE()

/* Definition for BTUART's DMA */
#define BTUART_TX_DMA_STREAM              DMA1_Channel2
#define BTUART_RX_DMA_STREAM              DMA1_Channel3

/* Definition for BTUART's NVIC */
#define BTUART_DMA_TX_IRQn                DMA1_Channel2_3_IRQn
#define BTUART_DMA_RX_IRQn                DMA1_Channel2_3_IRQn
#define BTUART_DMA_TX_IRQHandler          DMA1_Channel2_3_IRQHandler
#define BTUART_DMA_RX_IRQHandler          DMA1_Channel2_3_IRQHandler

#endif // _MSP_STM32F030_H_
