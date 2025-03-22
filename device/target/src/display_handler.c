/** 
 * @file display_handler.c
 * @author Jack Duignan (JackpDuignan@gmail.com)
 * @date 2025-03-22
 * @brief Handle the display of frequency values
 */


#include <stdint.h>
#include <stdbool.h>

#include "pin.h"

#include "TM1638.h"
#include "freq_handler.h"
#include "device_select.h"

#include "display_handler.h"

// The left display
struct TM1638Device disp2 = {
    .name = "Display 2",
    ._dots = 0
};

// The right display
struct TM1638Device disp1 = {
    .name = "Display 1",
    ._dots = 0
};

int display_handler_init(void) {
    disp2.clockPin = DISP_CLK;
    disp2.dataPin = DISP_DATA;
    disp2.stbPin = DISP_2_SELECT;

    disp1.clockPin = DISP_CLK;
    disp1.dataPin = DISP_DATA;
    disp1.stbPin = DISP_1_SELECT;

    if (tm1638_init(&disp2)!= 0) {
        return 1;
    }

    if (tm1638_init(&disp1)!= 0) {
        return 2;
    }

    tm1638_dot(&disp2, 2, true);
    tm1638_dot(&disp1, 2, true);

    return 0;
}

int display_handler_update(void) {
    uint8_t selected_device = device_select_get();

    freq_t active = freq_info_get(selected_device, ACTIVE_FREQ);
    freq_t standby = freq_info_get(selected_device, STANDBY_FREQ);

    tm1638_write(&disp2, active, "%d");
    tm1638_write(&disp1, standby, "%d");

    return 0;
}