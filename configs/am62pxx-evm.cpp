/* Configuration file for AM62Px */

#include <iostream>
#include "backend/includes/common.h"
#include "backend/includes/live_camera.h"
#include "backend/includes/camera.h"
#include "backend/includes/arm_analytics.h"
#include "backend/includes/run_cmd.h"
#include "backend/includes/settings.h"
#include "backend/includes/gpu_performance.h"
#include "backend/includes/benchmarks.h"
#include "backend/includes/wifi.h"

#include "config_common.h"

using namespace std;
QString platform = "am62pxx-evm";
QString wallpaper = "file:///opt/ti-apps-launcher/assets/am6x_oob_demo_home_image.png";

power_actions include_powerbuttons[] = {
    action_shutdown,
    action_reboot,
#if RT_BUILD == 0
    action_suspend,
#endif
};
int include_powerbuttons_count = ARRAY_SIZE(include_powerbuttons);

app_info include_apps[] = {
    app_industrial_control_sitara,
    app_camera,
    app_arm_analytics,
    app_benchmarks,
    app_gpu_performance,
    app_seva_store,
    app_chromium_browser,
    app_3d_demo,
    app_settings,
    app_terminal,
    app_wifi,
};
int include_apps_count = ARRAY_SIZE(include_apps);

Settings settings;
LiveCamera live_camera;
Camera camera;
ArmAnalytics arm_analytics;
Benchmarks benchmarks;
Gpu_performance gpuperformance;
Wifi wifi;

RunCmd *seva_store = new RunCmd(QStringLiteral("su weston -c \"chromium --no-first-run http://localhost:8007/#/\""));
RunCmd *demo_3d = new RunCmd(QStringLiteral("/usr/bin/SGX/demos/Wayland/OpenGLESSkinning"));
RunCmd *poweraction = new RunCmd(QStringLiteral(""));
RunCmd *chromium_browser = new RunCmd(QStringLiteral("su weston -c \"chromium --no-first-run https://webglsamples.org/aquarium/aquarium.html\""));

void platform_setup(QQmlApplicationEngine *engine) {
    std::cout << "Running Platform Setup of AM62P!" << endl;
    engine->rootContext()->setContextProperty("live_camera", &live_camera);
    engine->rootContext()->setContextProperty("camera", &camera);
    engine->rootContext()->setContextProperty("cameralist", &camera.Camera_list);
    engine->rootContext()->setContextProperty("arm_analytics", &arm_analytics);
    engine->rootContext()->setContextProperty("chromium_browser", chromium_browser);
    engine->rootContext()->setContextProperty("seva_store", seva_store);
    engine->rootContext()->setContextProperty("demo_3d", demo_3d);
    engine->rootContext()->setContextProperty("settings", &settings);
    engine->rootContext()->setContextProperty("wifi", &wifi);
    engine->rootContext()->setContextProperty("benchmarks", &benchmarks);
    engine->rootContext()->setContextProperty("gpuperformance", &gpuperformance);

    engine->rootContext()->setContextProperty("poweraction", poweraction);
}

