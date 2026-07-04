//
// Cross-platform CrashHandler for generating dump files.
//

#ifndef CROSSPLATFORM_CRASHHANDLERAPP_NATIVECRASHHANDLER_H
#define CROSSPLATFORM_CRASHHANDLERAPP_NATIVECRASHHANDLER_H

#include <string>

namespace CrossPlatform_CrashHandler {

class NativeCrashHandler {
public:
    static NativeCrashHandler& GetInstance();

    // Install crash handlers. Safe to call repeatedly.
    bool Initialize(const std::string& dumpDirectory = "crash_dumps");

    [[nodiscard]] bool IsInitialized() const noexcept;
    [[nodiscard]] const std::string& GetDumpDirectory() const noexcept;

private:
    NativeCrashHandler() = default;
    ~NativeCrashHandler() = default;

    NativeCrashHandler(const NativeCrashHandler&) = delete;
    NativeCrashHandler& operator=(const NativeCrashHandler&) = delete;
    NativeCrashHandler(NativeCrashHandler&&) = delete;
    NativeCrashHandler& operator=(NativeCrashHandler&&) = delete;

    bool initialized_ = false;
    std::string dumpDirectory_;
};

} // namespace CrossPlatform_CrashHandler

#endif // CROSSPLATFORM_CRASHHANDLERAPP_NATIVECRASHHANDLER_H
