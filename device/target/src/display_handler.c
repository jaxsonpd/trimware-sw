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

#define enable_all_digits() tm1638_enable_digit(&disp2, 0, true); \
                            tm1638_enable_digit(&disp2, 1, true); \
                            tm1638_enable_digit(&disp2, 2, true); \
                            tm1638_enable_digit(&disp2, 3, true); \
                            tm1638_enable_digit(&disp2, 4, true); \
                            tm1638_enable_digit(&disp2, 5, true); \
                            tm1638_enable_digit(&disp1, 0, true); \
                            tm1638_enable_digit(&disp1, 1, true); \
                            tm1638_enable_digit(&disp1, 2, true); \
                            tm1638_enable_digit(&disp1, 3, true); \
                            tm1638_enable_digit(&disp1, 4, true); \
                            tm1638_enable_digit(&disp1, 5, true);


// The left display
struct TM1638Device disp2 = {
    .name = "Display 2",
};

// The right display
struct TM1638Device disp1 = {
    .name = "Display 1",
};

int display_handler_init(void) {

    if (tm1638_init(&disp1, DISP_1_SELECT, DISP_DATA, DISP_CLK) != 0) {
        return 1;
    }

    if (tm1638_init(&disp2, DISP_2_SELECT, DISP_DATA, DISP_CLK) != 0) {
        return 2;
    }

    return 0;
}

int display_handler_update(void) {
    uint8_t selectedDevice = device_select_get();
    freqType_t deviceType = freq_handler_convert_to_type(selectedDevice);

    freq_t active = freq_info_get(deviceType, ACTIVE_FREQ);
    freq_t standby = freq_info_get(deviceType, STANDBY_FREQ);

    switch (deviceType) {
    case COM1:
    case COM2:
        enable_all_digits();
        tm1638_enable_dot(&disp2, 2, true);
        tm1638_enable_dot(&disp1, 2, true);
        tm1638_write(&disp2, active, "%d");
        tm1638_write(&disp1, standby, "%d");
        break;
    case NAV1:
    case NAV2:
        tm1638_enable_dot(&disp2, 2, true);
        tm1638_enable_dot(&disp1, 2, true);
        tm1638_enable_digit(&disp2, 5, false);
        tm1638_enable_digit(&disp1, 5, false);
        tm1638_write(&disp2, active, "%d");
        tm1638_write(&disp1, standby, "%d");
        break;

    case XPDR:
    
    default:
        break;
    }
    

    return 0;
}