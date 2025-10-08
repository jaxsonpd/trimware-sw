/**
 * @file display_handler.c
 * @author Jack Duignan (JackpDuignan@gmail.com)
 * @date 2025-03-22
 * @brief Handle the display of frequency values
 */


#include <stdint.h>
#include <stdbool.h>

#include "pin.h"

#include "../common/TM1638.h"
#include "freq_handler.h"
#include "device_select.h"

#include "display_handler.h"


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
    static bool firstRun = true;
    
    static freqType_t prevDeviceType = COM1;

    static freq_t prevActive = 0;
    static freq_t prevStandby = 0;

    freqType_t deviceType = freq_handler_convert_to_type(device_select_get());

    freq_t active = freq_info_get(deviceType, ACTIVE_FREQ);
    freq_t standby = freq_info_get(deviceType, STANDBY_FREQ);

    if (firstRun) {
        tm1638_reset(&disp1);
        tm1638_reset(&disp2);
        firstRun = false;
    }

    if (prevDeviceType != deviceType) {
        tm1638_reset(&disp1);
        tm1638_reset(&disp2);
        switch (deviceType) {
            case COM1:
            case COM2:
                tm1638_enable_dot(&disp2, 2, true);
                tm1638_enable_dot(&disp1, 2, true);
                break;
            case NAV1:
            case NAV2:
                tm1638_enable_dot(&disp2, 2, true);
                tm1638_enable_dot(&disp1, 2, true);
                tm1638_enable_digit(&disp2, 5, false);
                tm1638_enable_digit(&disp1, 5, false);
                break;
        
            case XPDR:
                tm1638_enable_digit(&disp2, 0, false);
                tm1638_enable_digit(&disp2, 1, false);
                tm1638_set_display_state(&disp1, false);
                break;
            default:
                break;
            }

        prevDeviceType = deviceType;
    }

    if (prevActive != active || prevStandby != standby) {
        switch (deviceType) {
        case COM1:
        case COM2:
        case NAV1:
        case NAV2:
            tm1638_write(&disp2, active, "%d");
            tm1638_write(&disp1, standby, "%d");
            break;

        case XPDR:
            tm1638_write(&disp2, active, "%d");
        default:
            break;
        }
        prevActive = active;
        prevStandby = standby;
    }
    

    return 0;
}