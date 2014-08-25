#include "stm32f0xx.h"
#include "stm32f0xx_hal.h"

/*
 * PC8, TIM3, CH3
 */

#define LED_TIM                           TIM3
#define LED_TIM_CLK_ENABLE()              __TIM3_CLK_ENABLE()

#define RED_PIN                GPIO_PIN_6
#define RED_GPIO_PORT          GPIOC
#define RED_GPIO_CLK_ENABLE()  __GPIOC_CLK_ENABLE()
#define RED_GPIO_CLK_DISABLE() __GPIOC_CLK_DISABLE()

#define GREEN_PIN                GPIO_PIN_7
#define GREEN_GPIO_PORT          GPIOC
#define GREEN_GPIO_CLK_ENABLE()  __GPIOC_CLK_ENABLE()
#define GREEN_GPIO_CLK_DISABLE() __GPIOC_CLK_DISABLE()

#define BLUE_PIN                GPIO_PIN_8
#define BLUE_GPIO_PORT          GPIOC
#define BLUE_GPIO_CLK_ENABLE()  __GPIOC_CLK_ENABLE()
#define BLUE_GPIO_CLK_DISABLE() __GPIOC_CLK_DISABLE()

#define WHITE_PIN                GPIO_PIN_9
#define WHITE_GPIO_PORT          GPIOC
#define WHITE_GPIO_CLK_ENABLE()  __GPIOC_CLK_ENABLE()
#define WHITE_GPIO_CLK_DISABLE() __GPIOC_CLK_DISABLE()

static TIM_HandleTypeDef    TimHandle;

void led_setup( void )
{
    GPIO_InitTypeDef GPIO_InitStruct;
    uint32_t uwPrescalerValue = 0;

    RED_GPIO_CLK_ENABLE();
    GREEN_GPIO_CLK_ENABLE();
    BLUE_GPIO_CLK_ENABLE();
    WHITE_GPIO_CLK_ENABLE();

    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;

    GPIO_InitStruct.Pin = RED_PIN;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM3;
    HAL_GPIO_Init(RED_GPIO_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GREEN_PIN;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM3;
    HAL_GPIO_Init(GREEN_GPIO_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = BLUE_PIN;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM3;
    HAL_GPIO_Init(BLUE_GPIO_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = WHITE_PIN;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM3;
    HAL_GPIO_Init(WHITE_GPIO_PORT, &GPIO_InitStruct);

    LED_TIM_CLK_ENABLE();

    uwPrescalerValue = (uint32_t)(SystemCoreClock / 100000) - 1;

    TimHandle.Instance = LED_TIM;
    TimHandle.Init.Period            = 255;
    TimHandle.Init.Prescaler         = uwPrescalerValue;
    TimHandle.Init.ClockDivision     = 0;
    TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
    TimHandle.Init.RepetitionCounter = 0;
    HAL_TIM_PWM_Init(&TimHandle);
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
    HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_1);

    sConfig.Pulse = g;
    HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_2);

    sConfig.Pulse = b;
    HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_3);

    sConfig.Pulse = w;
    HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_4);
    HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_4);
}
