#include <hyprland/src/plugins/PluginAPI.hpp>
// #include <hyprland/src/includes.hpp>
#include <hyprland/src/Compositor.hpp>

inline HANDLE  PHANDLE = nullptr;
FILE*          o_file  = nullptr;

APICALL EXPORT std::string PLUGIN_API_VERSION() {
    return HYPRLAND_API_VERSION;
}

static void onMouseButton(void* self, std::any data) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    // data is guaranteed
    const auto mouseButtonEvent = std::any_cast<IPointer::SButtonEvent>(data);
    if (mouseButtonEvent.state != WL_POINTER_BUTTON_STATE_PRESSED) {
        return;
    }
    if (o_file) {
        std::string text("onMouseButton\n");
        fwrite(text.c_str(), 1, text.size(), o_file);
    }
}

static void onTouchDown(void* self, std::any data) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    // data is guaranteed
    const auto touchDownEvent = std::any_cast<Aquamarine::ITouch::SDownEvent>(data);

    if (o_file) {
        std::string text("onTouchDown\n");
        fwrite(text.c_str(), 1, text.size(), o_file);
    }
}

static void onKeyPress(void* self, std::any data) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    // data is guaranteed
    const auto keyPressEvent = std::any_cast<std::unordered_map<std::string, std::any>>(data);

    for (const std::pair<const std::string, std::any>& keyPress : keyPressEvent) {}
}

APICALL EXPORT PLUGIN_DESCRIPTION_INFO PLUGIN_INIT(HANDLE handle) {
    throw std::runtime_error("[MyPlugin] YOLO");

    o_file = fopen("/home/ross/programming/tmp/MyPlugin/log.txt", "w+");
    if (o_file == nullptr) {
        HyprlandAPI::addNotification(PHANDLE, "[MyPlugin] COULD NOT OPEN FILE.", CHyprColor{1.0, 0.2, 0.2, 1.0}, 5000);
        throw std::runtime_error("[MyPlugin] Internal state error");
    }
    fprintf(o_file, "%s", "PLUGIN INIT\n");

    PHANDLE = handle;

    const std::string HASH = __hyprland_api_get_hash();

    // ALWAYS add this to your plugins. It will prevent random crashes coming from
    // mismatched header versions.
    if (HASH != GIT_COMMIT_HASH) {
        HyprlandAPI::addNotification(PHANDLE, "[MyPlugin] Mismatched headers! Can't proceed.", CHyprColor{1.0, 0.2, 0.2, 1.0}, 5000);
        throw std::runtime_error("[MyPlugin] Version mismatch");
    }

    static auto p1 = HyprlandAPI::registerCallbackDynamic(PHANDLE, "mouseButton", [&](void* self, SCallbackInfo& info, std::any data) {
        onMouseButton(self, data);
        return;
    });

    static auto p2 = HyprlandAPI::registerCallbackDynamic(PHANDLE, "touchDown", [&](void* self, SCallbackInfo& info, std::any data) {
        onTouchDown(self, data);
        return;
    });

    HyprlandAPI::addNotification(PHANDLE, "[MyPlugin] ZZZZInitialized successfully!", CHyprColor{0.2, 1.0, 0.2, 1.0}, 5000);

    return {"MyPlugin", "Not that amazing plugin to test the world!", "Me", "0.1"};
}
