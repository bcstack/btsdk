#include "system.h"
#include "board.h"
#include "bluetooth.h"
#include "stm32f0xx.h"
#include "stm32f0xx_hal.h"

void led_setup(void);
void led_set_color(u8 r, u8 g, u8 b, u8 w);
void spp_input(u8* input, u16 isize);

int main(void)
{
    board_setup();
    led_setup();
    bt_reset();

    bt_uart_setup(115200);
    bt_setup();

    gap_reset();
    gap_set_visible(1);

    while (1)
    {
        while (sys_schedule());
        bt_loop();
    }
}

void spp_input(u8* input, u16 isize)
{
    led_set_color(input[0], input[1], input[2], input[3]);
}

