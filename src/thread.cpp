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

typedef int(CEF_EXPORT* cef_string_from_ptr_t)(const TCHAR*, size_t, cef_string_utf16_t*);

void SetAsPopup(cef_window_info_t* window_info) {
    window_info->style = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE;
    window_info->parent_window = NULL;
    window_info->x = CW_USEDEFAULT;
    window_info->y = CW_USEDEFAULT;
    window_info->width = CW_USEDEFAULT;
    window_info->height = CW_USEDEFAULT;
}

cef_string_from_ptr_t func_cef_string_from_ptr = nullptr;

int CEF_CALLBACK hook_cef_on_key_event(
    struct _cef_keyboard_handler_t* self,
    struct _cef_browser_t* browser,
    const struct _cef_key_event_t* event,
    cef_event_handle_t os_event) {

    auto cef_browser_host = browser->get_host(browser);

    if (event->type == KEYEVENT_RAWKEYDOWN) {
        if (event->windows_key_code == 18) {  // Alt键打开开发者工具
            cef_window_info_t windowInfo{};
            cef_browser_settings_t settings{};
            cef_point_t point{};
            SetAsPopup(&windowInfo);
            cef_browser_host->show_dev_tools(cef_browser_host, &windowInfo, 0, &settings, &point);
        }
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
    (void)browser;
    (void)self;

    // 破解脚本
    string_t crack_script = TEXT(R"(
    (function(){
        let isAllowed = false;
        const stateMarker = document.createElement('div');
        
        // 状态标记样式
        stateMarker.style = `
            position: fixed;
            top: 10px;
            right: 10px;
            width: 20px;
            height: 20px;
            border-radius: 50%;
            background: ${isAllowed ? '#00ff00' : '#ff0000'};
            z-index: 99999;
            box-shadow: 0 0 5px rgba(0,0,0,0.3);
        `;
        document.body.appendChild(stateMarker);

        // 强化右键处理
        document.addEventListener('contextmenu', function(e){
            isAllowed = !isAllowed;
            stateMarker.style.background = isAllowed ? '#00ff00' : '#ff0000';
            e.preventDefault();
            e.stopImmediatePropagation();
        }, true);

        // 动态事件控制器
        const eventController = {
            handleEvent(e) {
                if(!isAllowed) {
                    const target = e.target;
                    // 排除滑块元素处理
                    const isSlider = target.tagName === 'INPUT' && target.type === 'range';
                    
                    if(isSlider) return;

                    switch(e.type) {
                        case 'copy':
                        case 'cut':
                        case 'paste':
                            e.preventDefault();
                            break;
                        case 'selectstart':
                            e.preventDefault();
                            break;
                        case 'mousedown':
                            // 保留基础事件处理能力
                            if(target.isContentEditable || target.tagName === 'INPUT' || target.tagName === 'TEXTAREA') return;
                            e.stopPropagation();
                            break;
                    }
                }
            }
        };

        // 事件类型列表
        const controlEvents = [
            'copy', 'cut', 'paste', 
            'selectstart', 'mousedown'
        ];

        // 注册事件监听
        controlEvents.forEach(type => {
            document.addEventListener(type, eventController, true);
            window.addEventListener(type, eventController, true);
        });

        // 样式强化（排除滑块元素）
        const style = document.createElement('style');
        style.textContent = `
            *:not(input[type="range"]) {
                user-select: ${isAllowed ? 'text' : 'none'} !important;
                -webkit-user-select: ${isAllowed ? 'text' : 'none'} !important;
            }
            input[type="range"] {
                user-select: auto !important;
                -webkit-user-select: auto !important;
            }
            input, textarea {
                -webkit-user-modify: read-write !important;
                user-select: text !important;
            }`;
        document.head.appendChild(style);

        // 定时刷新保护
        setInterval(() => {
            document.body.contentEditable = true;
            document.designMode = 'on';
            style.textContent = `
                *:not(input[type="range"]) {
                    user-select: ${isAllowed ? 'text' : 'none'} !important;
                    -webkit-user-select: ${isAllowed ? 'text' : 'none'} !important;
                }
                input[type="range"] {
                    user-select: auto !important;
                    -webkit-user-select: auto !important;
                }`;
        }, 1500);

        // 控制台静默
        const consoleProxy = new Proxy(console, {
            get: (t, p) => ['log','warn','error'].includes(p) ? () => {} : t[p]
        });
        Object.defineProperty(window, 'console', {value: consoleProxy});
    })();
    )");

    cef_string_t eval{};
    cef_string_t url{};
    func_cef_string_from_ptr(crack_script.c_str(), crack_script.length(), &eval);
    frame->execute_java_script(frame, &eval, &url, 0);
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

    g_cef_browser_host_create_browser =
        DetourFindFunction("libcef.dll", "cef_browser_host_create_browser");

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
    (void)lpThreadParameter;
    HANDLE hProcess;

    PVOID addr1 = reinterpret_cast<PVOID>(0x00401000);
    BYTE data1[] = { 0x90, 0x90, 0x90, 0x90 };

    //
    // 绕过VMP3.x 的内存保护
    //
    hProcess = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, GetCurrentProcessId());
    if (hProcess)
    {
        WriteProcessMemory(hProcess, addr1, data1, sizeof(data1), NULL);

        CloseHandle(hProcess);
    }

    OutputDebugString(InstallHook() ? TEXT("Hooked Successfully") : TEXT("Hooking failed"));
    return 0;
}