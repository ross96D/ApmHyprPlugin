#include <hyprland/src/plugins/PluginAPI.hpp>
#include <hyprland/src/devices/IKeyboard.hpp>
#include <hyprland/src/Compositor.hpp>

inline HANDLE        PHANDLE = nullptr;
FILE*                o_file  = nullptr;
SP<HOOK_CALLBACK_FN> mouseButtonCb;
SP<HOOK_CALLBACK_FN> touchDownCb;
SP<HOOK_CALLBACK_FN> keyPressCb;

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
    std::string text("onMouseButton\n");
    fwrite(text.c_str(), 1, text.size(), o_file);
    fflush(o_file);
}

static void onTouchDown(void* self, std::any data) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    // data is guaranteed
    const auto  touchDownEvent = std::any_cast<Aquamarine::ITouch::SDownEvent>(data);

    std::string text("onTouchDown\n");
    fwrite(text.c_str(), 1, text.size(), o_file);
    fflush(o_file);
}

static void onKeyPress(void* self, std::any data) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    // data is guaranteed
    auto dataMap = std::any_cast<std::unordered_map<std::string, std::any>>(data);
    auto event   = std::any_cast<IKeyboard::SKeyEvent>(dataMap["event"]);
    if (event.state != WL_KEYBOARD_KEY_STATE_PRESSED) {
        return;
    }
    std::string text = "onKeyPress " + std::to_string(event.keycode) + "\n";
    fwrite(text.c_str(), 1, text.size(), o_file);
    fflush(o_file);
}

APICALL EXPORT PLUGIN_DESCRIPTION_INFO PLUGIN_INIT(HANDLE handle) {
    o_file = fopen("/home/ross/programming/tmp/MyPlugin/log.txt", "w+");
    if (o_file == nullptr) {
        HyprlandAPI::addNotification(PHANDLE, "[MyPlugin] COULD NOT OPEN FILE.", CHyprColor{1.0, 0.2, 0.2, 1.0}, 5000);
        throw std::runtime_error("[MyPlugin] Internal state error");
    }
    fprintf(o_file, "%s", "PLUGIN INIT\n");
    fflush(o_file);

    PHANDLE = handle;

    const std::string HASH = __hyprland_api_get_hash();

    // ALWAYS add this to your plugins. It will prevent random crashes coming from
    // mismatched header versions.
    if (HASH != GIT_COMMIT_HASH) {
        HyprlandAPI::addNotification(PHANDLE, "[MyPlugin] Mismatched headers! Can't proceed.", CHyprColor{1.0, 0.2, 0.2, 1.0}, 5000);
        throw std::runtime_error("[MyPlugin] Version mismatch");
    }

    mouseButtonCb = HyprlandAPI::registerCallbackDynamic(PHANDLE, "mouseButton", [&](void* self, SCallbackInfo& info, std::any data) {
        onMouseButton(self, data);
        return;
    });

    touchDownCb = HyprlandAPI::registerCallbackDynamic(PHANDLE, "touchDown", [&](void* self, SCallbackInfo& info, std::any data) {
        onTouchDown(self, data);
        return;
    });

    keyPressCb = HyprlandAPI::registerCallbackDynamic(PHANDLE, "keyPress", [&](void* self, SCallbackInfo& info, std::any data) {
        onKeyPress(self, data);
        return;
    });

    HyprlandAPI::addNotification(PHANDLE, "[MyPlugin] Initialized successfully!", CHyprColor{0.2, 1.0, 0.2, 1.0}, 5000);

    return {"MyPlugin", "Not that amazing plugin to test the world!", "Me", "0.1"};
}

APICALL EXPORT void PLUGIN_EXIT() {
    mouseButtonCb.reset();
    touchDownCb.reset();
    keyPressCb.reset();
}