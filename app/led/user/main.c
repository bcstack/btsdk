#include "system.h"
#include "board.h"
#include "bluetooth.h"

int main(void)
{
    board_setup();
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
