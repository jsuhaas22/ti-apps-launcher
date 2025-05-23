/* Configuration file for AM62x SIP EVM */

#include "backend/includes/common.h"

struct device_info device_info_am62sip = {
    .dtMatchString = "AM62xSIP",
    .platform = "am62xxsip-evm",
    .wallpaper = "/images/am62sip_wallpaper.png",
    .include_apps = {
        app_industrial_control_minimal,
        app_live_camera,
        app_settings,
    },
    .include_powerbuttons = {
        action_shutdown,
        action_reboot,
    },
};
