#include <hyprland/src/plugins/PluginAPI.hpp>
#include <hyprland/src/devices/IKeyboard.hpp>
#include <hyprland/src/Compositor.hpp>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>

const char*          fifo_file_path = "/tmp/__testMyPlugin2";

inline HANDLE        PHANDLE = nullptr;
SP<HOOK_CALLBACK_FN> mouseButtonCb;
SP<HOOK_CALLBACK_FN> touchDownCb;
SP<HOOK_CALLBACK_FN> keyPressCb;

APICALL EXPORT std::string PLUGIN_API_VERSION() {
    return HYPRLAND_API_VERSION;
}

static void write_event() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    int fd = open(fifo_file_path, O_WRONLY | O_NONBLOCK);
    if (fd == -1) {
        std::string msgerr = strerror(errno);
        std::string msg    = "[MyPlugin] open " + msgerr;
        Debug::log(LOG, msg);
        return;
    }
    // output: "<sec> <nsec>"
    std::string msg = std::to_string(ts.tv_sec) + " " + std::to_string(ts.tv_nsec) + "\n";
    if (write(fd, msg.c_str(), msg.size()) == -1) {
        std::string msgerr = strerror(errno);
        std::string msg    = "[MyPlugin] WRITE-ING error" + msgerr;
        Debug::log(LOG, msg);
    } else {
        Debug::log(LOG, "[MyPlugin] WRITE-ING SUCCESSFULL");
    }
    fsync(fd);
    close(fd);
}

static void onMouseButton(void* self, std::any data) {
    // data is guaranteed
    const auto mouseButtonEvent = std::any_cast<IPointer::SButtonEvent>(data);
    if (mouseButtonEvent.state != WL_POINTER_BUTTON_STATE_PRESSED) {
        return;
    }
    write_event();
}

static void onTouchDown(void* self, std::any data) {
    // data is guaranteed
    const auto touchDownEvent = std::any_cast<Aquamarine::ITouch::SDownEvent>(data);
    write_event();
}

static void onKeyPress(void* self, std::any data) {
    // data is guaranteed
    auto dataMap = std::any_cast<std::unordered_map<std::string, std::any>>(data);
    auto event   = std::any_cast<IKeyboard::SKeyEvent>(dataMap["event"]);
    if (event.state != WL_KEYBOARD_KEY_STATE_PRESSED) {
        return;
    }
    write_event();
}

APICALL EXPORT PLUGIN_DESCRIPTION_INFO PLUGIN_INIT(HANDLE handle) {
    PHANDLE = handle;

    const std::string HASH = __hyprland_api_get_hash();

    // ALWAYS add this to your plugins. It will prevent random crashes coming from
    // mismatched header versions.
    if (HASH != GIT_COMMIT_HASH) {
        HyprlandAPI::addNotification(PHANDLE, "[MyPlugin] Mismatched headers! Can't proceed.", CHyprColor{1.0, 0.2, 0.2, 1.0}, 5000);
        throw std::runtime_error("[MyPlugin] Version mismatch");
    }

    struct stat stats;
    if (stat(fifo_file_path, &stats) == -1) {
        if (errno != 2) {
            std::string errnodesc = strerror(errno);
            std::string msg       = "[MyPlugin] stat named pipe fail: " + errnodesc;
            HyprlandAPI::addNotification(PHANDLE, msg, CHyprColor{1.0, 0.2, 0.2, 1.0}, 5000);
            throw std::runtime_error(msg);
        }
        if (mkfifo(fifo_file_path, 0666) == -1) {
            std::string errnodesc = strerror(errno);
            std::string msg       = "[MyPlugin] create named pipe fail: " + errnodesc;
            HyprlandAPI::addNotification(PHANDLE, msg, CHyprColor{1.0, 0.2, 0.2, 1.0}, 5000);
            throw std::runtime_error(msg);
        }
    } else {
        if (stats.st_mode && S_IFIFO == 0) {
            std::string msg = "[MyPlugin] named pipe file already exists and is not a named pipe";
            HyprlandAPI::addNotification(PHANDLE, msg, CHyprColor{1.0, 0.2, 0.2, 1.0}, 5000);
            throw std::runtime_error(msg);
        }
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

    HyprlandAPI::addNotification(PHANDLE, "[MyPlugin] Initialized z successfully!", CHyprColor{0.2, 1.0, 0.2, 1.0}, 5000);

    return {"MyPlugin", "Not that amazing plugin to test the world!", "Me", "0.1"};
}

APICALL EXPORT void PLUGIN_EXIT() {
    mouseButtonCb.reset();
    touchDownCb.reset();
    keyPressCb.reset();
}
