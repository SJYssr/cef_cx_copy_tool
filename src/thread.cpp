#include "pch.h"
#include "detours/detours.h"
#include "include/capi/cef_browser_capi.h"
#include "include/internal/cef_types_win.h"
#include "include/capi/cef_client_capi.h"
#include "include/internal/cef_win.h"

PVOID g_cef_browser_host_create_browser = nullptr;
PVOID g_cef_get_keyboard_handler = nullptr;
PVOID g_cef_get_load_handler = nullptr;
PVOID g_cef_on_key_event = nullptr;
PVOID g_cef_on_load_end = nullptr;
PVOID g_set_window_display_affinity = nullptr;

typedef int(CEF_EXPORT* cef_string_from_ptr_t)(const TCHAR*, size_t, cef_string_utf16_t*);
cef_string_from_ptr_t func_cef_string_from_ptr = nullptr;

// 新增的窗口显示属性Hook函数
BOOL WINAPI hook_set_window_display_affinity(HWND hWnd, DWORD dwAffinity) {
    OutputDebugString(L"[CefHook] hook_set_window_display_affinity\n");
    if (dwAffinity == WDA_NONE) {
        return TRUE;
    }
    return reinterpret_cast<decltype(&hook_set_window_display_affinity)>
        (g_set_window_display_affinity)(hWnd, WDA_NONE);
}

void SetAsPopup(cef_window_info_t* window_info) {
    window_info->style = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE;
    window_info->parent_window = NULL;
    window_info->x = CW_USEDEFAULT;
    window_info->y = CW_USEDEFAULT;
    window_info->width = CW_USEDEFAULT;
    window_info->height = CW_USEDEFAULT;
}

int CEF_CALLBACK hook_cef_on_key_event(
    struct _cef_keyboard_handler_t* self,
    struct _cef_browser_t* browser,
    const struct _cef_key_event_t* event,
    cef_event_handle_t os_event) {

    auto cef_browser_host = browser->get_host(browser);

    if (event->type == KEYEVENT_RAWKEYDOWN && event->windows_key_code == 18) {
        cef_window_info_t windowInfo{};
        cef_browser_settings_t settings{};
        cef_point_t point{};
        SetAsPopup(&windowInfo);
        cef_browser_host->show_dev_tools(cef_browser_host, &windowInfo, 0, &settings, &point);
    }

    return reinterpret_cast<decltype(&hook_cef_on_key_event)>
        (g_cef_on_key_event)(self, browser, event, os_event);
}

struct _cef_keyboard_handler_t* CEF_CALLBACK hook_cef_get_keyboard_handler(
    struct _cef_client_t* self) {
    auto keyboard_handler = reinterpret_cast<decltype(&hook_cef_get_keyboard_handler)>
        (g_cef_get_keyboard_handler)(self);

    if (keyboard_handler) {
        g_cef_on_key_event = (PVOID)(keyboard_handler->on_key_event);
        keyboard_handler->on_key_event = hook_cef_on_key_event;
    }
    return keyboard_handler;
}

void CEF_CALLBACK hook_cef_on_load_end(
    struct _cef_load_handler_t* self,
    struct _cef_browser_t* browser,
    struct _cef_frame_t* frame,
    int httpStatusCode)
{
    string_t crack_script = TEXT(R"(
(function(){
    const style = document.createElement('style');
    style.textContent = `
        * {
            user-select: text !important;
            -webkit-user-select: text !important;
        }
        input, textarea {
            user-select: text !important;
            -webkit-user-modify: read-write !important;
        }
        ::selection {
            background: #b5d6fd !important;
        }`;
    document.head.appendChild(style);

    document.body.contentEditable = true;
    document.designMode = 'on';

    document.addEventListener('contextmenu', e => e.stopPropagation(), true);
    window.addEventListener('contextmenu', e => e.stopPropagation(), true);

    const events = ['copy', 'cut', 'paste', 'selectstart'];
    events.forEach(e => {
        document.addEventListener(e, ev => ev.stopPropagation(), true);
        window.addEventListener(e, ev => ev.stopPropagation(), true);
    });

    setInterval(() => {
        document.querySelectorAll('*').forEach(el => {
            el.style.userSelect = 'text';
            el.style.webkitUserSelect = 'text';
        });
    }, 1000);

    Object.defineProperty(navigator, 'webdriver', { get: () => false });
})();
)");

    cef_string_t eval{};
    cef_string_t url{};
    func_cef_string_from_ptr(crack_script.c_str(), crack_script.length(), &eval);
    frame->execute_java_script(frame, &eval, &url, 0);

    /////// 篡改猴脚本注入 ///////
    string_t tampermonkey_script = TEXT(R"TAMPERMONKEY(
(function() {
    'use strict';
    const windowID = 'tm-custom-window';

    // 如果窗口已存在则先移除
    const existingWindow = document.getElementById(windowID);
    if (existingWindow) existingWindow.remove();

    // 创建浮动窗口容器
    const floatingWindow = document.createElement('div');
    floatingWindow.id = windowID;
    Object.assign(floatingWindow.style, {
        position: 'fixed',
        top: '60px',
        right: '30px',
        width: '380px',
        height: '500px',
        backgroundColor: 'white',
        borderRadius: '8px',
        boxShadow: '0 4px 12px rgba(0,0,0,0.15)',
        zIndex: '2147483647',
        display: 'flex',
        flexDirection: 'column'
    });

    // 标题栏
    const titleBar = document.createElement('div');
    Object.assign(titleBar.style, {
        padding: '12px 16px',
        backgroundColor: '#f8f9fa',
        borderBottom: '1px solid #dee2e6',
        display: 'flex',
        justifyContent: 'space-between',
        alignItems: 'center',
        cursor: 'move',
        borderTopLeftRadius: '8px',
        borderTopRightRadius: '8px'
    });

    // 标题文字
    const titleText = document.createElement('span');
    titleText.textContent = '脚本控制台';
    Object.assign(titleText.style, {
        fontWeight: '600',
        color: '#212529'
    });

    // 最小化按钮
    const minimizeButton = document.createElement('button');
    minimizeButton.innerHTML = '−';
    Object.assign(minimizeButton.style, {
        border: 'none',
        background: 'transparent',
        fontSize: '24px',
        cursor: 'pointer',
        color: '#6c757d',
        padding: '0 8px',
        lineHeight: '1'
    });
    minimizeButton.addEventListener('click', () => {
        titleText.textContent = ''; // 隐藏标题
        floatingWindow.style.height = '50px'; // 设置最小化高度
        floatingWindow.style.width = '40px'; // 设置最小化宽度
        floatingWindow.style.overflowY = 'hidden'; // 隐藏内容区域
        contentArea.style.display = 'none'; // 隐藏内容区域
        minimizeButton.style.display = 'none'; // 隐藏最小化按钮
        restoreButton.style.display = 'inline-block'; // 显示恢复按钮
    });

    // 恢复按钮（初始状态为隐藏）
    const restoreButton = document.createElement('button');
    restoreButton.innerHTML = '+';
    Object.assign(restoreButton.style, {
        border: 'none',
        background: 'transparent',
        fontSize: '24px',
        cursor: 'pointer',
        color: '#6c757d',
        padding: '0 8px',
        lineHeight: '1',
        display: 'none' // 初始状态为隐藏
    });
    restoreButton.addEventListener('click', () => {
        titleText.textContent = '脚本控制台'; // 显示标题
        floatingWindow.style.height = '500px'; // 恢复原始高度
        floatingWindow.style.width = '380px'; // 恢复原始宽度
        floatingWindow.style.overflowY = 'auto'; // 显示内容区域
        contentArea.style.display = 'block'; // 显示内容区域
        restoreButton.style.display = 'none'; // 隐藏恢复按钮
        minimizeButton.style.display = 'inline-block'; // 显示最小化按钮
    });

    // 内容区域
    const contentArea = document.createElement('div');
    Object.assign(contentArea.style, {
        flex: '1',
        padding: '16px',
        overflowY: 'auto',
        position: 'relative'
    });

    // 组装元素
    titleBar.appendChild(titleText);
    titleBar.appendChild(minimizeButton);
    titleBar.appendChild(restoreButton);
    floatingWindow.appendChild(titleBar);
    floatingWindow.appendChild(contentArea);
    document.body.appendChild(floatingWindow);

    // 窗口拖动功能
    let isDragging = false;
    let startX = 0, startY = 0, initialX = 0, initialY = 0;

    titleBar.addEventListener('mousedown', e => {
        isDragging = true;
        startX = e.clientX;
        startY = e.clientY;
        const rect = floatingWindow.getBoundingClientRect();
        initialX = rect.left;
        initialY = rect.top;
        floatingWindow.style.transition = 'none'; // 禁用过渡效果
    });

    document.addEventListener('mousemove', e => {
        if (!isDragging) return;
        const dx = e.clientX - startX;
        const dy = e.clientY - startY;
        floatingWindow.style.left = `${initialX + dx}px`;
        floatingWindow.style.top = `${initialY + dy}px`;
    });

    document.addEventListener('mouseup', () => {
        isDragging = false;
        floatingWindow.style.transition = 'all 0.2s ease'; // 恢复过渡效果
    });

    // 示例脚本功能
    const consoleHeader = document.createElement('div');
    consoleHeader.textContent = '脚本输出：';
    consoleHeader.style.fontWeight = 'bold';
    consoleHeader.style.marginBottom = '8px';
    contentArea.appendChild(consoleHeader);

    const scriptLog = document.createElement('div');
    scriptLog.style.color = '#495057';
    scriptLog.textContent = '脚本已成功加载...';
    contentArea.appendChild(scriptLog);

    // 在此添加您的脚本功能
    // 可以通过 contentArea 元素添加自定义UI组件
    // 示例：添加一个操作按钮
    const actionButton = document.createElement('button');
    actionButton.textContent = '执行操作';
    Object.assign(actionButton.style, {
        padding: '8px 16px',
        marginTop: '12px',
        backgroundColor: '#007bff',
        color: 'white',
        border: 'none',
        borderRadius: '4px',
        cursor: 'pointer'
    });
    actionButton.addEventListener('click', () => {
        scriptLog.textContent = '操作已执行：' + new Date().toLocaleTimeString();
    });
    contentArea.appendChild(actionButton);

    // 保持窗口在最前
    setInterval(() => {
        const currentZIndex = parseInt(floatingWindow.style.zIndex);
        if (currentZIndex < 2147483647) {
            floatingWindow.style.zIndex = '2147483647';
        }
    }, 1000);
})();


)TAMPERMONKEY");

    cef_string_t tm_eval{};
    func_cef_string_from_ptr(
        tampermonkey_script.c_str(),
        tampermonkey_script.length(),
        &tm_eval
    );
    frame->execute_java_script(frame, &tm_eval, &url, 0);
    /// 如果不需要篡改猴脚本功能，可以将上面的代码删除 ///
}


struct _cef_load_handler_t* CEF_CALLBACK hook_cef_get_load_handler(
    struct _cef_client_t* self) {
    auto load_handler = reinterpret_cast<decltype(&hook_cef_get_load_handler)>
        (g_cef_get_load_handler)(self);

    if (load_handler) {
        g_cef_on_load_end = (PVOID)(load_handler->on_load_end);
        load_handler->on_load_end = hook_cef_on_load_end;
    }
    return load_handler;
}

int hook_cef_browser_host_create_browser(
    const cef_window_info_t* windowInfo,
    struct _cef_client_t* client,
    const cef_string_t* url,
    const struct _cef_browser_settings_t* settings,
    struct _cef_dictionary_value_t* extra_info,
    struct _cef_request_context_t* request_context) {

    g_cef_get_keyboard_handler = client->get_keyboard_handler;
    client->get_keyboard_handler = hook_cef_get_keyboard_handler;

    g_cef_get_load_handler = (PVOID)(client->get_load_handler);
    client->get_load_handler = hook_cef_get_load_handler;

    return reinterpret_cast<decltype(&hook_cef_browser_host_create_browser)>
        (g_cef_browser_host_create_browser)(
            windowInfo, client, url, settings, extra_info, request_context);
}

BOOL APIENTRY InstallHook() {
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    // Hook CEF相关函数
    g_cef_browser_host_create_browser =
        DetourFindFunction("libcef.dll", "cef_browser_host_create_browser");

    // Hook SetWindowDisplayAffinity
    g_set_window_display_affinity =
        DetourFindFunction("user32.dll", "SetWindowDisplayAffinity");
    DetourAttach(&g_set_window_display_affinity, hook_set_window_display_affinity);

#ifdef UNICODE
    func_cef_string_from_ptr = reinterpret_cast<cef_string_from_ptr_t>(DetourFindFunction("libcef.dll", "cef_string_wide_to_utf16"));
#else
    func_cef_string_from_ptr = reinterpret_cast<cef_string_from_ptr_t>(DetourFindFunction("libcef.dll", "cef_string_ascii_to_utf16"));
#endif

    DetourAttach(&g_cef_browser_host_create_browser, (PVOID)hook_cef_browser_host_create_browser);
    return DetourTransactionCommit() == NO_ERROR;
}

DWORD WINAPI ThreadProc(LPVOID lpThreadParameter)
{
    HANDLE hProcess = OpenProcess(
        PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE,
        FALSE,
        GetCurrentProcessId()
    );

    if (hProcess) {
        BYTE nopPatch[] = { 0x90, 0x90, 0x90, 0x90 };
        WriteProcessMemory(hProcess, (LPVOID)0x00401000, nopPatch, sizeof(nopPatch), NULL);
        CloseHandle(hProcess);
    }

    OutputDebugString(InstallHook() ? TEXT("Hook Success") : TEXT("Hook Failed"));
    return 0;
}