//
// Cross-platform CrashHandler for generating dump files.
//

#include "NativeCrashHandler.h"

#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdlib>
#include <ctime>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <thread>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#include <DbgHelp.h>
#elif defined(__APPLE__)
#include <array>
#include <execinfo.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#endif

namespace {

using CrossPlatform_CrashHandler::NativeCrashHandler;

std::atomic<bool> g_dumping{false};
NativeCrashHandler* g_handler = nullptr;
std::terminate_handler g_previousTerminateHandler = nullptr;

#ifdef _WIN32
LPTOP_LEVEL_EXCEPTION_FILTER g_previousUnhandledFilter = nullptr;
#elif defined(__APPLE__)
constexpr std::array<int, 6> kHandledSignals = {
        SIGSEGV, SIGABRT, SIGFPE, SIGILL, SIGBUS, SIGTRAP
};
std::array<struct sigaction, kHandledSignals.size()> g_previousSignalActions{};
#endif

std::string CurrentTimeString() {
    const std::time_t now = std::time(nullptr);
    std::tm tmNow{};
#ifdef _WIN32
    localtime_s(&tmNow, &now);
#else
    localtime_r(&now, &tmNow);
#endif

    std::ostringstream oss;
    oss << std::put_time(&tmNow, "%Y%m%d_%H%M%S");
    return oss.str();
}

unsigned long CurrentProcessIdPortable() {
#ifdef _WIN32
    return static_cast<unsigned long>(::GetCurrentProcessId());
#elif defined(__APPLE__)
    return static_cast<unsigned long>(::getpid());
#else
    return 0UL;
#endif
}

std::filesystem::path EnsureDumpDirectory(const std::string& configuredDir) {
    const std::filesystem::path dumpDir = configuredDir.empty()
                                          ? std::filesystem::path("crash_dumps")
                                          : std::filesystem::path(configuredDir);
    std::error_code ec;
    std::filesystem::create_directories(dumpDir, ec);
    return dumpDir;
}

std::string BuildDumpBaseName(const char* prefix) {
    std::ostringstream oss;
    oss << prefix << "_" << CurrentTimeString() << "_pid_" << CurrentProcessIdPortable();
    return oss.str();
}

#ifdef _WIN32
bool WriteWindowsMiniDump(EXCEPTION_POINTERS* exceptionPointers, const std::string& dumpDirectory) noexcept {
    const std::filesystem::path filePath =
            EnsureDumpDirectory(dumpDirectory) / (BuildDumpBaseName("crash") + ".dmp");

    const std::wstring widePath = filePath.wstring();
    HANDLE fileHandle = ::CreateFileW(widePath.c_str(),
                                      GENERIC_WRITE,
                                      0,
                                      nullptr,
                                      CREATE_ALWAYS,
                                      FILE_ATTRIBUTE_NORMAL,
                                      nullptr);
    if (fileHandle == INVALID_HANDLE_VALUE) {
        return false;
    }

    MINIDUMP_EXCEPTION_INFORMATION exceptionInfo{};
    exceptionInfo.ThreadId = ::GetCurrentThreadId();
    exceptionInfo.ExceptionPointers = exceptionPointers;
    exceptionInfo.ClientPointers = FALSE;

    const auto dumpType = static_cast<MINIDUMP_TYPE>(
            MiniDumpWithThreadInfo |
            MiniDumpWithIndirectlyReferencedMemory |
            MiniDumpScanMemory
    );

    const BOOL writeOk = ::MiniDumpWriteDump(::GetCurrentProcess(),
                                             ::GetCurrentProcessId(),
                                             fileHandle,
                                             dumpType,
                                             exceptionPointers ? &exceptionInfo : nullptr,
                                             nullptr,
                                             nullptr);

    ::CloseHandle(fileHandle);
    return writeOk == TRUE;
}

LONG WINAPI UnhandledExceptionFilterImpl(EXCEPTION_POINTERS* exceptionPointers) {
    bool expected = false;
    if (g_handler && g_dumping.compare_exchange_strong(expected, true)) {
        WriteWindowsMiniDump(exceptionPointers, g_handler->GetDumpDirectory());
    }

    if (g_previousUnhandledFilter != nullptr) {
        return g_previousUnhandledFilter(exceptionPointers);
    }

    return EXCEPTION_EXECUTE_HANDLER;
}
#elif defined(__APPLE__)
const char* SignalName(int signo) {
    switch (signo) {
        case SIGSEGV: return "SIGSEGV";
        case SIGABRT: return "SIGABRT";
        case SIGFPE:  return "SIGFPE";
        case SIGILL:  return "SIGILL";
        case SIGBUS:  return "SIGBUS";
        case SIGTRAP: return "SIGTRAP";
        default:      return "UNKNOWN";
    }
}

void WriteToFd(int fd, const std::string& data) {
    if (fd >= 0 && !data.empty()) {
        (void)::write(fd, data.data(), data.size());
    }
}

void WriteMacCrashReport(int signo, siginfo_t* info, void* context, const std::string& dumpDirectory) noexcept {
    const std::filesystem::path filePath =
            EnsureDumpDirectory(dumpDirectory) / (BuildDumpBaseName("crash") + ".log");

    const int fd = ::open(filePath.string().c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd < 0) {
        return;
    }

    std::ostringstream header;
    header << "Crash Report\n";
    header << "Timestamp: " << CurrentTimeString() << "\n";
    header << "PID: " << ::getpid() << "\n";
    header << "Thread ID: " << std::this_thread::get_id() << "\n";
    header << "Signal: " << SignalName(signo) << " (" << signo << ")\n";
    if (info != nullptr) {
        header << "Fault Address: " << info->si_addr << "\n";
        header << "Signal Code: " << info->si_code << "\n";
    }
    header << "Context: " << context << "\n\n";
    header << "Backtrace:\n";
    WriteToFd(fd, header.str());

    void* frames[128];
    const int frameCount = ::backtrace(frames, 128);
    if (frameCount > 0) {
        ::backtrace_symbols_fd(frames, frameCount, fd);
        WriteToFd(fd, "\n");
    }

    ::close(fd);
}

void SignalHandlerImpl(int signo, siginfo_t* info, void* context) {
    bool expected = false;
    if (g_handler && g_dumping.compare_exchange_strong(expected, true)) {
        WriteMacCrashReport(signo, info, context, g_handler->GetDumpDirectory());
    }

    for (size_t i = 0; i < kHandledSignals.size(); ++i) {
        if (kHandledSignals[i] == signo) {
            ::sigaction(signo, &g_previousSignalActions[i], nullptr);
            break;
        }
    }

    ::raise(signo);
}
#endif

void TerminateHandlerImpl() noexcept {
    bool expected = false;
    if (g_handler && g_dumping.compare_exchange_strong(expected, true)) {
#ifdef _WIN32
        WriteWindowsMiniDump(nullptr, g_handler->GetDumpDirectory());
#elif defined(__APPLE__)
        WriteMacCrashReport(SIGABRT, nullptr, nullptr, g_handler->GetDumpDirectory());
#endif
    }

    if (g_previousTerminateHandler != nullptr) {
        g_previousTerminateHandler();
        return;
    }
    std::abort();
}

} // namespace

namespace CrossPlatform_CrashHandler {

NativeCrashHandler& NativeCrashHandler::GetInstance() {
    static NativeCrashHandler instance;
    return instance;
}

bool NativeCrashHandler::Initialize(const std::string& dumpDirectory) {
    if (initialized_) {
        return true;
    }

    dumpDirectory_ = dumpDirectory.empty() ? "crash_dumps" : dumpDirectory;
    (void)EnsureDumpDirectory(dumpDirectory_);

    g_handler = this;
    g_dumping.store(false);
    g_previousTerminateHandler = std::set_terminate(TerminateHandlerImpl);

#ifdef _WIN32
    g_previousUnhandledFilter = ::SetUnhandledExceptionFilter(UnhandledExceptionFilterImpl);
#elif defined(__APPLE__)
    for (size_t i = 0; i < kHandledSignals.size(); ++i) {
        struct sigaction action{};
        action.sa_flags = SA_SIGINFO | SA_RESETHAND;
        action.sa_sigaction = SignalHandlerImpl;
        sigemptyset(&action.sa_mask);
        ::sigaction(kHandledSignals[i], &action, &g_previousSignalActions[i]);
    }
#endif

    initialized_ = true;
    return true;
}

bool NativeCrashHandler::IsInitialized() const noexcept {
    return initialized_;
}

const std::string& NativeCrashHandler::GetDumpDirectory() const noexcept {
    return dumpDirectory_;
}

} // namespace CrossPlatform_CrashHandler
