#include "pch.h"
#include "detours/detours.h"  // 导入Detours库，用于函数钩子
#include "include/capi/cef_browser_capi.h"  // CEF浏览器相关API
#include "include/internal/cef_types_win.h"  // CEF Windows相关类型定义
#include "include/capi/cef_client_capi.h"  // CEF客户端相关API
#include "include/internal/cef_win.h"  // CEF Windows特定实现

// 存储原始函数指针，用于钩子函数调用原始函数
PVOID g_cef_browser_host_create_browser = nullptr;  // CEF创建浏览器函数指针
PVOID g_cef_get_keyboard_handler = nullptr;  // 获取键盘处理器函数指针
PVOID g_cef_get_load_handler = nullptr;  // 获取加载处理器函数指针
PVOID g_cef_on_key_event = nullptr;  // 键盘事件处理函数指针
PVOID g_cef_on_load_end = nullptr;  // 页面加载完成事件处理函数指针
PVOID g_set_window_display_affinity = nullptr;  // 设置窗口显示属性函数指针

// 用于跟踪是否是首次注入
bool g_first_injection = true;

// 定义CEF字符串函数类型
typedef int(CEF_EXPORT* cef_string_from_ptr_t)(const TCHAR*, size_t, cef_string_utf16_t*);
cef_string_from_ptr_t func_cef_string_from_ptr = nullptr;  // 字符串转换函数指针

// 钩子函数：hook_set_window_display_affinity
// 目的：绕过窗口的防截图保护
// 参数：hWnd - 窗口句柄，dwAffinity - 显示属性
// 当应用尝试设置防截图属性时，此函数会将其设置为WDA_NONE（无保护）
BOOL WINAPI hook_set_window_display_affinity(HWND hWnd, DWORD dwAffinity) {
    OutputDebugString(L"[CefHook] hook_set_window_display_affinity\n");  // 输出调试信息
    if (dwAffinity == WDA_NONE) {  // 如果本来就是无保护状态，直接返回成功
        return TRUE;
    }
    // 调用原始函数，但强制设置为WDA_NONE（无保护）
    return reinterpret_cast<decltype(&hook_set_window_display_affinity)>
        (g_set_window_display_affinity)(hWnd, WDA_NONE);
}

// 设置窗口为弹出窗口样式
// 用于开发者工具窗口的创建
void SetAsPopup(cef_window_info_t* window_info) {
    // 设置窗口样式为覆盖窗口，包含子窗口裁剪和可见属性
    window_info->style = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE;
    window_info->parent_window = NULL;  // 无父窗口
    // 使用默认位置和大小
    window_info->x = CW_USEDEFAULT;
    window_info->y = CW_USEDEFAULT;
    window_info->width = CW_USEDEFAULT;
    window_info->height = CW_USEDEFAULT;
}

// 钩子函数：hook_cef_on_key_event
// 目的：截获键盘事件，添加快捷键功能
// 添加Alt键(18)按下时打开开发者工具的功能
int CEF_CALLBACK hook_cef_on_key_event(
    struct _cef_keyboard_handler_t* self,
    struct _cef_browser_t* browser,
    const struct _cef_key_event_t* event,
    cef_event_handle_t os_event) {

    auto cef_browser_host = browser->get_host(browser);  // 获取浏览器主机对象

    // 当Alt键被按下时，打开开发者工具
    if (event->type == KEYEVENT_RAWKEYDOWN && event->windows_key_code == 18) {
        cef_window_info_t windowInfo{};  // 初始化窗口信息
        cef_browser_settings_t settings{};  // 初始化浏览器设置
        cef_point_t point{};  // 初始化坐标点
        SetAsPopup(&windowInfo);  // 将窗口设置为弹出样式
        // 显示开发者工具
        cef_browser_host->show_dev_tools(cef_browser_host, &windowInfo, 0, &settings, &point);
    }

    // 调用原始键盘事件处理函数
    return reinterpret_cast<decltype(&hook_cef_on_key_event)>
        (g_cef_on_key_event)(self, browser, event, os_event);
}

// 钩子函数：hook_cef_get_keyboard_handler
// 目的：拦截获取键盘处理器的请求，替换为自定义键盘处理器
struct _cef_keyboard_handler_t* CEF_CALLBACK hook_cef_get_keyboard_handler(
    struct _cef_client_t* self) {
    // 调用原始函数获取键盘处理器
    auto keyboard_handler = reinterpret_cast<decltype(&hook_cef_get_keyboard_handler)>
        (g_cef_get_keyboard_handler)(self);

    if (keyboard_handler) {  // 如果键盘处理器存在
        // 保存原始的键盘事件处理函数
        g_cef_on_key_event = (PVOID)(keyboard_handler->on_key_event);
        // 替换为我们的钩子函数
        keyboard_handler->on_key_event = hook_cef_on_key_event;
    }
    return keyboard_handler;
}

// 钩子函数：hook_cef_on_load_end
// 目的：在页面加载完成时注入自定义JavaScript
// 实现解除复制限制和添加篡改猴功能
void CEF_CALLBACK hook_cef_on_load_end(
    struct _cef_load_handler_t* self,
    struct _cef_browser_t* browser,
    struct _cef_frame_t* frame,
    int httpStatusCode)
{
    // JS脚本：用于破解网页复制限制
    string_t crack_script = TEXT(R"(
(function(){
    // 创建样式元素，设置全局可选中文本
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

    // 开启内容可编辑和设计模式
    document.body.contentEditable = false;
    document.designMode = 'off';

    // 阻止右键菜单事件传播
    document.addEventListener('contextmenu', e => e.stopPropagation(), true);
    window.addEventListener('contextmenu', e => e.stopPropagation(), true);

    // 阻止复制、剪切、粘贴、选择事件的传播
    const events = ['copy', 'cut', 'paste', 'selectstart'];
    events.forEach(e => {
        document.addEventListener(e, ev => ev.stopPropagation(), true);
        window.addEventListener(e, ev => ev.stopPropagation(), true);
    });

    // 定期强制所有元素可选
    setInterval(() => {
        document.querySelectorAll('*').forEach(el => {
            el.style.userSelect = 'text';
            el.style.webkitUserSelect = 'text';
        });
    }, 1000);

    // 禁用webdriver检测
    Object.defineProperty(navigator, 'webdriver', { get: () => false });
})();
)");

    // 将脚本转换为CEF字符串格式
    cef_string_t eval{};
    cef_string_t url{};
    func_cef_string_from_ptr(crack_script.c_str(), crack_script.length(), &eval);
    // 执行JavaScript脚本
    frame->execute_java_script(frame, &eval, &url, 0);

    // 显示注入成功弹窗（仅首次注入时显示）
    if (g_first_injection) {
        string_t success_notification = TEXT(R"(
(function(){
    const notificationDiv = document.createElement('div');
    Object.assign(notificationDiv.style, {
        position: 'fixed',
        top: '20px',
        left: '50%',
        transform: 'translateX(-50%)',
        backgroundColor: 'rgba(46, 204, 113, 0.9)',
        color: 'white',
        padding: '12px 24px',
        borderRadius: '8px',
        boxShadow: '0 4px 8px rgba(0,0,0,0.2)',
        zIndex: '9999999',
        fontFamily: 'Arial, sans-serif',
        fontSize: '14px',
        textAlign: 'left',
        maxWidth: '450px',
        lineHeight: '1.5'
    });
    
    // 创建标题
    const titleElement = document.createElement('div');
    titleElement.textContent = '超星破除复制粘贴限制已成功注入！';
    titleElement.style.fontSize = '16px';
    titleElement.style.fontWeight = 'bold';
    titleElement.style.marginBottom = '10px';
    titleElement.style.textAlign = 'center';
    notificationDiv.appendChild(titleElement);
    
    // 创建作者信息
    const authorElement = document.createElement('div');
    authorElement.innerHTML = '<b>作者:</b> SJYssr<br><b>地址:</b> <a href="https://github.com/SJYssr" target="_blank" style="color: white; text-decoration: underline;">https://github.com/SJYssr</a>';
    authorElement.style.marginBottom = '10px';
    notificationDiv.appendChild(authorElement);
    
    // 创建使用说明
    const instructionTitle = document.createElement('div');
    instructionTitle.textContent = '使用说明:';
    instructionTitle.style.fontWeight = 'bold';
    instructionTitle.style.marginBottom = '5px';
    notificationDiv.appendChild(instructionTitle);
    
    // ALT键警告
    const altWarning = document.createElement('div');
    altWarning.innerHTML = '<b>ALT键:</b> 进入开发者调试，<span style="color: #ffff00; font-weight: bold;">误触会使无法考试</span>';
    altWarning.style.marginBottom = '10px';
    notificationDiv.appendChild(altWarning);
    
    // 主要功能列表
    const featuresTitle = document.createElement('div');
    featuresTitle.textContent = '主要功能:';
    featuresTitle.style.fontWeight = 'bold';
    featuresTitle.style.marginBottom = '5px';
    notificationDiv.appendChild(featuresTitle);
    
    const featuresList = document.createElement('ol');
    featuresList.style.margin = '0';
    featuresList.style.paddingLeft = '20px';
    
    const feature1 = document.createElement('li');
    feature1.textContent = '破除考试客户端的复制粘贴显示';
    featuresList.appendChild(feature1);
    
    const feature2 = document.createElement('li');
    feature2.textContent = '破除考试客户端无法录屏截屏';
    featuresList.appendChild(feature2);
    
    notificationDiv.appendChild(featuresList);
    
    // 添加关闭按钮
    const closeButton = document.createElement('span');
    closeButton.textContent = '×';
    Object.assign(closeButton.style, {
        position: 'absolute',
        top: '10px',
        right: '12px',
        cursor: 'pointer',
        fontWeight: 'bold',
        fontSize: '18px'
    });
    closeButton.addEventListener('click', () => {
        document.body.removeChild(notificationDiv);
    });
    
    notificationDiv.appendChild(closeButton);
    document.body.appendChild(notificationDiv);
})();
)");

        // 将弹窗脚本转换为CEF字符串格式并执行
        cef_string_t notification_eval{};
        func_cef_string_from_ptr(success_notification.c_str(), success_notification.length(), &notification_eval);
        frame->execute_java_script(frame, &notification_eval, &url, 0);
        
        // 标记为非首次注入
        g_first_injection = false;
    }

    /////// 篡改猴脚本注入 ///////
    // 创建一个可自定义的浮动控制台窗口
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

    // 将篡改猴脚本转换为CEF字符串格式
    cef_string_t tm_eval{};
    func_cef_string_from_ptr(
        tampermonkey_script.c_str(),
        tampermonkey_script.length(),
        &tm_eval
    );
    // 执行篡改猴脚本
    frame->execute_java_script(frame, &tm_eval, &url, 0);
    /// 如果不需要篡改猴脚本功能，可以将上面的代码删除 ///
}

// 钩子函数：hook_cef_get_load_handler
// 目的：拦截获取加载处理器的请求，替换为自定义加载处理器
struct _cef_load_handler_t* CEF_CALLBACK hook_cef_get_load_handler(
    struct _cef_client_t* self) {
    // 调用原始函数获取加载处理器
    auto load_handler = reinterpret_cast<decltype(&hook_cef_get_load_handler)>
        (g_cef_get_load_handler)(self);

    if (load_handler) {  // 如果加载处理器存在
        // 保存原始的页面加载完成事件处理函数
        g_cef_on_load_end = (PVOID)(load_handler->on_load_end);
        // 替换为我们的钩子函数
        load_handler->on_load_end = hook_cef_on_load_end;
    }
    return load_handler;
}

// 钩子函数：hook_cef_browser_host_create_browser
// 目的：拦截浏览器创建过程，替换处理器
int hook_cef_browser_host_create_browser(
    const cef_window_info_t* windowInfo,
    struct _cef_client_t* client,
    const cef_string_t* url,
    const struct _cef_browser_settings_t* settings,
    struct _cef_dictionary_value_t* extra_info,
    struct _cef_request_context_t* request_context) {

    // 保存并替换键盘处理器获取函数
    g_cef_get_keyboard_handler = client->get_keyboard_handler;
    client->get_keyboard_handler = hook_cef_get_keyboard_handler;

    // 保存并替换加载处理器获取函数
    g_cef_get_load_handler = (PVOID)(client->get_load_handler);
    client->get_load_handler = hook_cef_get_load_handler;

    // 调用原始浏览器创建函数
    return reinterpret_cast<decltype(&hook_cef_browser_host_create_browser)>
        (g_cef_browser_host_create_browser)(
            windowInfo, client, url, settings, extra_info, request_context);
}

// 安装所有钩子函数
BOOL APIENTRY InstallHook() {
    // 开始Detours事务
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    // 查找CEF浏览器创建函数
    g_cef_browser_host_create_browser =
        DetourFindFunction("libcef.dll", "cef_browser_host_create_browser");

    // 查找并钩住SetWindowDisplayAffinity函数
    g_set_window_display_affinity =
        DetourFindFunction("user32.dll", "SetWindowDisplayAffinity");
    DetourAttach(&g_set_window_display_affinity, hook_set_window_display_affinity);

    // 根据编译环境选择适当的字符串转换函数
#ifdef UNICODE
    // 在Unicode环境下使用宽字符转UTF16函数
    func_cef_string_from_ptr = reinterpret_cast<cef_string_from_ptr_t>(DetourFindFunction("libcef.dll", "cef_string_wide_to_utf16"));
#else
    // 在非Unicode环境下使用ASCII转UTF16函数
    func_cef_string_from_ptr = reinterpret_cast<cef_string_from_ptr_t>(DetourFindFunction("libcef.dll", "cef_string_ascii_to_utf16"));
#endif

    // 钩住CEF浏览器创建函数
    DetourAttach(&g_cef_browser_host_create_browser, (PVOID)hook_cef_browser_host_create_browser);
    // 提交所有Detours事务，返回是否成功
    return DetourTransactionCommit() == NO_ERROR;
}

// 线程处理函数：用于初始化钩子
DWORD WINAPI ThreadProc(LPVOID lpThreadParameter)
{
    // 打开当前进程，获取操作内存的权限
    HANDLE hProcess = OpenProcess(
        PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE,
        FALSE,
        GetCurrentProcessId()
    );

    if (hProcess) {
        // 在特定内存地址写入NOP指令（用于绕过某些保护）
        BYTE nopPatch[] = { 0x90, 0x90, 0x90, 0x90 };  // NOP指令，用于跳过代码
        WriteProcessMemory(hProcess, (LPVOID)0x00401000, nopPatch, sizeof(nopPatch), NULL);
        CloseHandle(hProcess);
    }

    // 安装钩子并输出结果到调试窗口
    OutputDebugString(InstallHook() ? TEXT("Hook Success") : TEXT("Hook Failed"));
    return 0;
}