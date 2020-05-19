PonyOCR

# 简介

PonyOCR 是一个简单的双平台 OCR 工具。它并不具备本地识别能力，它只是一个综合了各大 OCR 平台接口的小工具。因此，**它不是开箱即用的，在使用前需要在各平台注册接口权限**。

PonyOCR 的跨平台特性来自 Qt 自身的跨平台特性，但
* 实现截图的代码依赖于平台
  * 对于 Linux 下的 GNOME 桌面，使用 [GNOME 提供的 D-Bus 接口](https://github.com/GNOME/gnome-shell/blob/master/data/dbus-interfaces/org.gnome.Shell.Screenshot.xml)
    * 为何不调用 Qt 自身截图函数? [TODO]
  * 对于 Windows，调用 Qt 自身的函数
  * 多显示器支持 [TODO]
    * 请不要做出奇奇怪怪的事，比如在截图的过程中插拔显示器，或截图截图等套娃动作
* Markdown 预览的代码由于采用了 Qt WebEngine 模块，也依赖于平台（见下文）

因此 PonyOCR 目前仅支持 Linux GNOME 桌面与 Windows。

支持的 OCR 平台与模式：
* 百度：普通文本、手写、公式、网络图片、表格、数字、二维码
* 腾讯：普通文本、手写
* OCR.Space：普通文本（默认中英文）
* Mathpix：公式

对于除了公式识以外的模式，推荐使用百度平台。

程序主要分为三部分：
* 调用API：（接口鉴权、）发送请求（、查询进度）、获取结果
* 截图
* 窗口
  * 主窗口（提供Markdown预览）
  * 配置窗口

源码下有 5 个子文件夹
```
PonyOCR
├── API
├── Capture
├── Configuation
├── Markdown
└── img
```

# Configuation 目录
```
.
├── configuation.pri # Qt 项目 include 文件
├── confighandler.cpp # 所有配置的调用&保存
├── confighandler.h
├── apiconfigwidget.cpp # 各平台API配置
├── apiconfigwidget.h
├── generalconfigwidget.cpp # 一般配置
├── generalconfigwidget.h
├── proxyconfigwidget.cpp # 代理配置
├── proxyconfigwidget.h
├── configdialog.cpp # 设置窗口（集成所有设置）
└── configdialog.h
```

## “把手”类——`ConfigHandler`

这个 handler 类是 `QSettings` 的 wrapper，它保存了所有API的鉴权信息（Key, App ID 等）、软件的基本配置、代理配置。因为 `QSettings` 的构造和析构都很快，几乎下面的所有类都会单独拥有一个 `m_handler` 成员。

## 窗口类

### `APIConfigWidget` 类

各平台API配置，包括接口鉴权信息，可用模式，是否经过代理。

![APIConfigWidget](resources/962cc30b7df441a092c03aa53a995cab.png)

### `GeneralConfigWidget` 类

![GeneralConfigWidget](resources/c2e4e2ad5af1428b824ccb870cc4c0f6.png)

可用来调整每种模式的默认平台。

### `ProxyConfigWidget` 类

![ProxyConfigWidget](resources/1fe3d65c5f9e468180f1955d4fcae374.png)

因为一些原因，某些海外的 API 无法访问（虽然还没支持），那么就需要使用代理。当然，就像 PonyOCR 实质上并不提供 OCR 服务，它也不提供任何代理服务。

为了实现代理，PonyOCR 使用了两个 QNAM(`QNetworkAccessManager`)，其中 `normalManager` 负责无代理访问，`proxiedManager` 负责 `HTTP` 或 `Socks5` 代理访问。

在每次配置保存后 `ProxyConfigWidget` 都会修改 `proxiedManager` 的代理设置。

# API 目录
```
.
├── api.pri # Qt 项目 include 文件
├── networkbase.cpp # 网络基类
├── networkbase.h
├── apibase.cpp # API 基类
├── apibase.h
├── apicommon.cpp # 平台相关信息
├── apicommon.h

├── baiduapi.cpp # 百度
├── baiduapi.h
├── baiduaccesstoken.cpp # 百度 AccessToken 获取
├── baiduaccesstoken.h
├── baidutableprocessor.cpp # 异步表格处理
├── baidutableprocessor.h

├── mathpixapi.cpp # Mathpix
├── mathpixapi.h

├── ocrspaceapi.cpp # OCR.Space
├── ocrspaceapi.h

├── tencentapi.cpp # 腾讯
├── tencentapi.h

├── generalapi.cpp # API集成
└── generalapi.h
```

## `networkbase` 类

提供基本的网络服务，包含 `normalManager` 和 `proxiedManager`，可以选择经过 `HTTP` 或 `Socks5` 代理。

PonyOCR 全程一共使用两个 QNAM，但它们并不是全局可见的。

## `apibase` 类

继承 `networkbase`，添加了对外的 `process` 接口，是所有平台特定API类的基类。

### 函数

* `virtual void process(const OCRRequest &request);`
  处理OCR请求，实质上调用了下面的 `_process` 函数。
* `void _process(const OCRRequest &request);`
  初始化，将图片转换为 base64 编码，再调用对应模式的处理函数（`processNormalText`，`processFormula` 等）

## `apicommon` 文件

包含了所有平台的 API 接口地址、错误信息（`ErrorCodeMap`）。由于错误信息包含中文，在 Windows 下使用 msvc 编译时可能出现乱码或报错。对于乱码，参考 [Qt使用MSVC编译器输出中文乱码的问题](https://blog.csdn.net/WU9797/article/details/85019490)。

## 各平台 API 类

每个 API 类都在自己平台的 namespace 内。百度 API 比较复杂，分成三个类处理，其他平台各自有一个类。

总体来说，当一个 API 类的 `process(OCRRequest)` 方法被调用时，会有以下动作：
* 初始化（获取 API 鉴权信息等）
* 将源图片转化为 base64 编码
* 根据不同模式调用处理函数
* 针对不同平台、模式，给 POST 添加不同参数
* POST 请求，获取结果
* 如果是异步接口则不断查询状态
* 处理
  * 出错则发送 `OCRFailure` 信号
  * 成功则发送 `OCRSuccessful` 信号

先从比较简单的平台开始。

### OCR.Space

文档地址：http://ocr.space/OCRAPI

OCR.Space 提供两个 OCR 引擎，但由于第二引擎不支持中文，这里只会使用第一个。

注：该平台有表格模式，但这不是真正的表格OCR（虽然其他的表格OCR也比较鸡肋）。正如文档所说，这只是尽量将结构化文本更好地输出而已。
> If set to true, the OCR logic makes sure that the parsed text result is always returned line by line. This switch is recommended for table OCR, receipt OCR, invoice processing and all other type of input documents that have a table like structure.

### Mathpix

文档地址：https://docs.mathpix.com/

Mathpix 最大的优势是公式识别，但纯文本无法很好地显示公式，因此增加了 Markdown 预览。

注：[Mathpix 在 LaTeX 的基础上增加了 `\longdiv`](https://docs.mathpix.com/#long-division)，因此这无法很好地显示。

### 腾讯

接口鉴权：https://ai.qq.com/doc/auth.shtml
OCR文档地址：https://ai.qq.com/doc/ocrgeneralocr.shtml

腾讯的接口鉴权比较复杂，每次需要基于 APPID，APPKey，时间戳还有随机字符验证，其余并没什么。

### 百度

接口鉴权：https://ai.baidu.com/ai-doc/REFERENCE/Ck3dwjhhu
OCR文档地址：https://ai.baidu.com/ai-doc/OCR/zk3h7xz52

百度的接口鉴权需要向服务器请求一个 AT (AccessToken)，因此增加了基于 `NetworkBase` 专门用来获取 AT 的 `BaiduAccessToken` 类。因为 AT 有寿命限制，但短期大量查询只需用到同一个 AT，因此 `BaiduAccessToken` 会保存 AT 一段时间。 

由于百度的表格处理默认异步，所以又增加了一个基于 `NetworkBase` 的 `BaiduTableProcessor`。它内部有两个阶段，用 `m_stage` 记录。
* `tableRequest()` 发送处理请求，把 `m_stage` 设为 1
* 接收返回结果进入 `parse()`，获取 request id
* `tableReply()` 将 `m_stage` 设为 2，查询结果
* 接收返回结果进入 `parse()`，如果完成则处理，否则等待一段时间后继续查询结果

# Markdown 目录
```
.
├── markdown.pri # Qt 项目 include 文件
├── document.cpp
├── document.h
├── previewpage.cpp
├── previewpage.h
├── marked.min.js # Markdown parser
├── markdown.css  # Markdown 样式表
├── index-katex.html   # 用于显示预览的 html 页面
└── index-mathjax.html # 使用 KaTeX 或 MathJax
```

Markdown 主要用于对公式和表格的预览，代码来源于 [Qt WebEngine Markdown Editor Example](https://doc.qt.io/qt-5/qtwebengine-webenginewidgets-markdowneditor-example.html)。
由于使用了 Qt 的浏览器模块，对于 Windows，需要满足下列要求：
* 64 位
* [OpenSSL Toolkit](https://www.openssl.org/)
* Visual Studio 2017 version 15.8 or later
* Windows 10 SDK version 10.0.18362 or later
详情参考 Qt 文档：
* [Qt for Windows - Requirements](https://doc.qt.io/qt-5/windows-requirements.html)
* [Qt WebEngine Platform Notes](https://doc.qt.io/qt-5/qtwebengine-platform-notes.html#windows)

# Capture 目录
```
.
├── capture.pri # Qt 项目 include 文件
├── screengrabber.cpp
├── screengrabber.h
├── capturebuttonhandler.cpp
├── capturebuttonhandler.h
├── capturewidget.cpp
├── capturewidget.h
├── selectionwidget.cpp
├── selectionwidget.h
├── twobutton.cpp
└── twobutton.h
```

包含一个截图窗口，将需要识别的部分和用户指定的模式、平台封装入一个 `OCRRequest` 中，通过 `captureTaken` 信号发送至 `PonyOCR` 处理。

类似于 `AutoCAD`，截图时有两个模式
* 快速模式：截图时光标划过主对角线（左上、右下），则会自动选择普通文本模式以及对应的默认平台。
* 一般模式：划过副对角线（右上、左下），则会允许选择模式和平台。

由于截图部分的代码依赖于平台，目前仅支持 Linux GNOME 桌面和 Windows 10。

部分代码来自 [Flameshot - 强大而又简单的截图工具](https://flameshot.js.org/#/)

## 按按钮——`twobutton` 文件

![twobutton](resources/ae68490072b14877ba9f074ba73ae2fa.png)
图中右侧是模式按钮，左侧则是平台按钮。不同模式对应不同的平台按钮。

一般模式下，截图完成后让用户能够快捷地选择模式&平台。
因为大多数情况下都是从右上角滑到左下角，按钮也就默认在左下角出现（当然如果你非要反着来我也没办法）。如果左下角放不下所有按钮，那么就会换一个能看见地方显示（上、下、右或截图框内部），同时也会避免在多显示器的情况下被几个显示器分割。要是你的显示器实在小得连几个按钮都装不下的话，那我也很无奈了。

### `PlatformButton` 类

其实是添加了 `buttonClicked(OCRPlatform)` 信号并且会改变光标的 `QPushButton`。在光标移动到按钮上时，会把光标改为箭头型。如果光标之前是在灰色背景上的十字，移动到按钮上就会变成箭头了。

### `ModeButton` 类

类似，但有两个信号
* `buttonClicked(OCRMode)`
* `enterMode(OCRMode)` 用于提示 `CaptureButtonHandler`： 当前光标移动到这个模式上，需要显示相应平台按钮。  

## 另一个把手类——`CaptureButtonHandler`

正如名称所示，它管理着上面两个按钮。
它维护了两个数组：`m_pbArray` 和 `m_mbArray`，并且接收数组内按钮的信号。
