#pragma once

#include <string>
#include <monoSDK.h>
#include <atomic>
extern std::atomic<bool> g_cursorReady;

namespace GameBackend
{
    // 启动后台线程（幂等）
    void Start();

    // 请求停止并等待后台线程结束（幂等）
    void Stop();

    // 将数据提交到后台线程进行处理（线程安全）
    void Enqueue(const std::string& data);

    // 后台线程是否在运行
    bool IsRunning();
}

inline MonoMethod* set_lockState = nullptr;
inline MonoMethod* set_visible = nullptr;
inline std::atomic<bool> g_cursorReady{ false };  // 如果还没定义

// 辅助函数
inline void UnlockCursor() {
    if (!set_lockState) return;
    void* args[1];
    int state = 0;
    args[0] = &state;
    mono_runtime_invoke(set_lockState, nullptr, args, nullptr);
}
inline void LockCursor() {
    if (!set_lockState) return;
    void* args[1];
    int state = 1;
    args[0] = &state;
    mono_runtime_invoke(set_lockState, nullptr, args, nullptr);
}
inline void SetCursorVisible(bool visible) {
    if (!set_visible) return;
    void* args[1];
    int val = visible ? 1 : 0;
    args[0] = &val;
    mono_runtime_invoke(set_visible, nullptr, args, nullptr);
}

inline void TriggerHook(bool& lock, bool ED, void (*Install)(), void (*Uninstall)())
{
    
    if (ED && !lock)
    {
        Install();
        lock = true;
    }
    else if (!ED && lock)
    {
        Uninstall();
        lock = false;
    }
}
