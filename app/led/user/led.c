#include "stm32f0xx.h"
#include "stm32f0xx_hal.h"
#include "bluetooth.h"

/*
 * PC8, TIM3, CH3
 */

#define TIMx                           TIM3
#define TIMx_CLK_ENABLE()              __TIM3_CLK_ENABLE()

#define BLUE_PIN                GPIO_PIN_8
#define BLUE_GPIO_PORT          GPIOC
#define BLUE_GPIO_CLK_ENABLE()  __GPIOC_CLK_ENABLE()
#define BLUE_GPIO_CLK_DISABLE() __GPIOC_CLK_DISABLE()

#define GREEN_PIN                GPIO_PIN_9
#define GREEN_GPIO_PORT          GPIOC
#define GREEN_GPIO_CLK_ENABLE()  __GPIOC_CLK_ENABLE()
#define GREEN_GPIO_CLK_DISABLE() __GPIOC_CLK_DISABLE()

static TIM_HandleTypeDef    TimHandle;

void led_setup( void )
{
    GPIO_InitTypeDef GPIO_InitStruct;
    uint32_t uwPrescalerValue = 0;

    BLUE_GPIO_CLK_ENABLE();

    GPIO_InitStruct.Pin = BLUE_PIN;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;

    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(BLUE_GPIO_PORT, &GPIO_InitStruct);
    HAL_GPIO_WritePin(BLUE_GPIO_PORT, BLUE_PIN, GPIO_PIN_SET);
    HAL_Delay(100);

    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM3;
    HAL_GPIO_Init(BLUE_GPIO_PORT, &GPIO_InitStruct);

    GREEN_GPIO_CLK_ENABLE();

    GPIO_InitStruct.Pin = GREEN_PIN;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;

    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(GREEN_GPIO_PORT, &GPIO_InitStruct);
    HAL_GPIO_WritePin(GREEN_GPIO_PORT, GREEN_PIN, GPIO_PIN_SET);
    HAL_Delay(100);

    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM3;
    HAL_GPIO_Init(GREEN_GPIO_PORT, &GPIO_InitStruct);

    TIMx_CLK_ENABLE();

    uwPrescalerValue = (uint32_t)(SystemCoreClock / 100000) - 1;

    TimHandle.Instance = TIMx;
    TimHandle.Init.Period            = 127;
    TimHandle.Init.Prescaler         = uwPrescalerValue;
    TimHandle.Init.ClockDivision     = 0;
    TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
    TimHandle.Init.RepetitionCounter = 0;
    if (HAL_TIM_PWM_Init(&TimHandle) != HAL_OK)
    {
        /* Initialization Error */
        Error_Handler();
    }

}

static s8 dim = 0;
void led_loop( void )
{
    u16 pulse;

    TIM_OC_InitTypeDef sConfig;

    dim += 2;

    if (dim >= 0) pulse = dim;
    else pulse = -dim;

    sConfig.OCMode       = TIM_OCMODE_PWM1;
    sConfig.OCPolarity   = TIM_OCPOLARITY_HIGH;
    sConfig.OCFastMode   = TIM_OCFAST_DISABLE;
    sConfig.OCNPolarity  = TIM_OCNPOLARITY_HIGH;
    sConfig.OCIdleState  = TIM_OCIDLESTATE_RESET;
    sConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;

    sConfig.Pulse = pulse;
    if (HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_3) != HAL_OK)
    {
        /* Configuration Error */
        Error_Handler();
    }

    /* Start channel 3 */
    if (HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_3) != HAL_OK)
    {
        /* PWM generation Error */
        Error_Handler();
    }

    sConfig.Pulse = 128 - pulse;
    if (HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_4) != HAL_OK)
    {
        /* Configuration Error */
        Error_Handler();
    }

    /* Start channel 4 */
    if (HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_4) != HAL_OK)
    {
        /* PWM generation Error */
        Error_Handler();
    }

}
