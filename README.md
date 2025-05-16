# 超星考试客户端题目复制工具

<div align="center">

<!-- 替换不可显示的Logo图片为文字标志 -->
<h1>📚 超星题目复制助手</h1>

[![版本](https://img.shields.io/badge/版本-V2.1.0-brightgreen)](https://github.com/SJYssr/cef_cx_copy_tool/releases)
[![许可证](https://img.shields.io/badge/许可证-GPL--3.0-blue)](https://github.com/SJYssr/cef_cx_copy_tool/blob/main/LICENSE)
[![状态](https://img.shields.io/badge/状态-可用-success)](https://github.com/SJYssr/cef_cx_copy_tool)
[![贡献](https://img.shields.io/badge/欢迎-贡献-orange)](https://github.com/SJYssr/cef_cx_copy_tool/issues)

*本项目基于CEF钩子技术，用于超星考试客户端的题目复制和截图*

*仅用于个人学习交流，严禁用于商业用途！*

</div>

<p align="center">
  <a href="#项目起源">项目起源</a> •
  <a href="#功能特性">功能特性</a> •
  <a href="#实现原理">实现原理</a> •
  <a href="#使用方法">使用方法</a> •
  <a href="#赞赏支持">赞赏支持</a> •
  <a href="#免责声明">免责声明</a>
</p>

---

## 📖 项目起源

<table>
<tr>
<td width="60%">

本项目修改自 [kazutoiris](https://github.com/kazutoiris) 的 [cef-hook](https://github.com/kazutoiris/cef-hook) 项目，针对超星考试客户端进行了特定优化。

**特别说明**：
- 版本：V2.1.0 (2025-5-16更新)
- 适用于：超星考试客户端最新版(4.3.0.2)
- 后续可能此版本也可以用
- 开发者：[SJYssr](https://github.com/SJYssr)

</td>
<td width="40%">

<!-- 替换不可显示的CEF示意图为描述性文字 -->
<div align="center">
<h3>🔄 CEF渲染引擎</h3>
<p>基于Chromium的内嵌浏览器框架</p>
<p>↓</p>
<h3>🛠️ DLL注入与钩子</h3>
<p>修改核心功能行为</p>
</div>

</td>
</tr>
</table>

## 🚀 功能特性

<div align="center">

| 功能 | 描述 | 状态 |
|:----:|:-----|:----:|
| 🛡️ **反反截屏** | 绕过考试客户端对截图和录屏的限制 | ✅ |
| 📋 **破除复制限制** | 解除考试客户端内容选择和复制限制 | ✅ |
| 🖱️ **破除选中限制** | 可以自由选择考试页面上的文本 | ✅ |
| 📝 **复制粘贴题目** | 支持题目内容的复制粘贴功能 | ✅ |
| 🛠️ **开发者调试** | 按下 ALT 键可以进入开发者调试模式 | ✅ |

</div>

## 💻 实现原理

<details open>
<summary><b>技术实现详解</b></summary>

<!-- 替换不可显示的Mermaid图表为文本描述 -->
<div align="center">
<h3>🔄 工作流程</h3>
</div>

```
超星客户端启动
    ↓
DLL注入到客户端
    ↓
Hook关键函数:
    ├── 1. SetWindowDisplayAffinity → 绕过防截图
    ├── 2. CEF浏览器函数 → 注入JavaScript
    └── 3. 键盘事件处理 → 添加ALT快捷键
            ↓
JavaScript执行:
    ├── 移除CSS限制
    ├── 启用选中文本
    └── 启用复制功能
```

本项目主要通过以下技术实现功能：

- 使用 **Detours** 库进行函数钩子，拦截并修改关键函数行为
- 拦截 `SetWindowDisplayAffinity` 函数，绕过窗口的防截图保护
- 通过钩住 CEF 浏览器相关函数注入自定义 JavaScript 代码
- 实现了自定义键盘事件处理，添加开发者工具快捷键

</details>

## 📋 使用方法

### 🔧 安装依赖

<div style="padding: 15px; border-left: 4px solid #5bc0de; background-color:rgb(7, 78, 114); margin: 10px 0;">
<b>⚠️ 注意</b>：请确保按照正确顺序安装依赖，否则可能导致功能异常
</div>

1. 请移步至[超星钩子教程](https://www.52pojie.cn/thread-1995491-1-1.html)按照步骤安装所需依赖
2. 确保系统已安装 Visual Studio 2019 或更高版本（用于编译）

### 🔨 编译部署

```bash
# 克隆项目
git clone https://github.com/SJYssr/cef_cx_copy_tool.git

# 进入项目目录
cd cef_cx_copy_tool

# 使用VS编译（或直接用VS打开解决方案）
```

1. 使用 Visual Studio 打开项目并编译，生成 DLL 文件
2. 将生成的 DLL 文件复制到超星考试客户端的安装目录下
3. 启动超星考试客户端，功能将自动生效

### 📝 使用提示

<div style="padding: 15px; border-left: 4px solid #f0ad4e; background-color:rgb(58, 49, 3); margin: 10px 0;">
<b>重要提示</b>：使用前请仔细阅读以下内容
</div>

- 登录考试前会显示注入成功的提示窗口
- ⚠️ **警告**: ALT 键可进入开发者调试模式，但误触可能导致无法正常考试
- 注入后可以正常使用 <kbd>Ctrl</kbd>+<kbd>C</kbd>、<kbd>Ctrl</kbd>+<kbd>V</kbd> 复制粘贴文本
- 可自由对考试页面进行截图和录屏
## 可能遇到的问题
 1. 不同的考试客户端版本可能存在兼容性问题，建议直接将考试客户端升至最新版本
 2. 不同的计算机可能因版本不同存在兼容性问题（目前win7,win11,23H2都可使用）
 3. 无法正常加载可能是您自身的问题

## 🎁 赞赏支持

<div align="center">
<p>如果您觉得此项目对您有所帮助，可以扫描以下二维码进行赞赏支持：</p>

<img src="https://github.com/SJYssr/img/raw/main/1/zanshang.jpg" alt="赞赏码" width="200">

<p><i>您的支持是我持续更新的动力</i></p>
</div>

## ⚖️ 免责声明

<div style="padding: 15px; border: 1px solid #d9534f; background-color:rgb(238, 156, 156); border-radius: 5px; margin: 10px 0;">
<h4>⚠️ 免责声明</h4>
<ul>
<li>本代码遵循 <a href="https://github.com/SJYssr/cef_cx_copy_tool/blob/main/LICENSE">GPL-3.0 License</a> 协议，允许<strong>开源/免费使用和引用/修改/衍生代码的开源/免费使用</strong>，不允许<strong>修改和衍生的代码作为闭源的商业软件发布和销售</strong>，禁止<strong>使用本代码盈利</strong>，以此代码为基础的程序<strong>必须</strong>同样遵守 <a href="https://github.com/SJYssr/cef_cx_copy_tool/blob/main/LICENSE">GPL-3.0 License</a>协议</li>
<li>本代码仅用于<strong>学习讨论</strong>，禁止<strong>用于盈利</strong>和<strong>非法用途</strong></li>
<li>他人或组织使用本代码进行的任何<strong>违法行为</strong>与本人无关</li>
<li>使用本项目造成的任何后果由使用者自行承担</li>
</ul>
</div>

---

<div align="center">
<p>Made with ❤️ by <a href="https://github.com/SJYssr">SJYssr</a></p>
<p>
  <a href="#">回到顶部</a> •
  <a href="https://github.com/SJYssr/cef_cx_copy_tool/issues">报告问题</a> •
  <a href="https://github.com/SJYssr/cef_cx_copy_tool/blob/main/LICENSE">许可协议</a>
</p>
</div>