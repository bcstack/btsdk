#include "system.h"
#include "stm32f0xx.h"
#include "stm32f0xx_hal.h"

/*
 * PA4, TIM14CH1, W
 * PA6, TIM3CH1,  R
 * PA7, TIM3CH2,  G
 * PB1, TIM3CH4,  B
 */

static TIM_HandleTypeDef    Tim3, Tim14;

void led_setup( void )
{
    GPIO_InitTypeDef GPIO_InitStruct;
    uint32_t uwPrescalerValue = 0;

    __GPIOA_CLK_ENABLE();
    __GPIOB_CLK_ENABLE();

    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;

    // PA6, TIM3CH1, R
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM3;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // PA7, TIM3CH2,  G
    GPIO_InitStruct.Pin = GPIO_PIN_7;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM3;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // PB1, TIM3CH4,  B
    GPIO_InitStruct.Pin = GPIO_PIN_1;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM3;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // PA4, TIM14CH1, W
    GPIO_InitStruct.Pin = GPIO_PIN_4;
    GPIO_InitStruct.Alternate = GPIO_AF4_TIM14;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    __TIM3_CLK_ENABLE();
    __TIM14_CLK_ENABLE();

    uwPrescalerValue = (uint32_t)(SystemCoreClock / 100000) - 1;

    Tim3.Instance = TIM3;
    Tim3.Init.Period            = 255;
    Tim3.Init.Prescaler         = uwPrescalerValue;
    Tim3.Init.ClockDivision     = 0;
    Tim3.Init.CounterMode       = TIM_COUNTERMODE_UP;
    Tim3.Init.RepetitionCounter = 0;
    HAL_TIM_PWM_Init(&Tim3);

    Tim14.Instance = TIM14;
    Tim14.Init.Period            = 255;
    Tim14.Init.Prescaler         = uwPrescalerValue;
    Tim14.Init.ClockDivision     = 0;
    Tim14.Init.CounterMode       = TIM_COUNTERMODE_UP;
    Tim14.Init.RepetitionCounter = 0;
    HAL_TIM_PWM_Init(&Tim14);
}

void led_set_color(u8 r, u8 g, u8 b, u8 w)
{
    TIM_OC_InitTypeDef sConfig;

    sConfig.OCMode       = TIM_OCMODE_PWM1;
    sConfig.OCPolarity   = TIM_OCPOLARITY_HIGH;
    sConfig.OCFastMode   = TIM_OCFAST_DISABLE;
    sConfig.OCNPolarity  = TIM_OCNPOLARITY_HIGH;
    sConfig.OCIdleState  = TIM_OCIDLESTATE_RESET;
    sConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;

    sConfig.Pulse = r;
    HAL_TIM_PWM_ConfigChannel(&Tim3, &sConfig, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&Tim3, TIM_CHANNEL_1);

    sConfig.Pulse = g;
    HAL_TIM_PWM_ConfigChannel(&Tim3, &sConfig, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&Tim3, TIM_CHANNEL_2);

    sConfig.Pulse = b;
    HAL_TIM_PWM_ConfigChannel(&Tim3, &sConfig, TIM_CHANNEL_4);
    HAL_TIM_PWM_Start(&Tim3, TIM_CHANNEL_4);

    sConfig.Pulse = w;
    HAL_TIM_PWM_ConfigChannel(&Tim14, &sConfig, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&Tim14, TIM_CHANNEL_1);
}
