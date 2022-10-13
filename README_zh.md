# 无障碍子系统

-   [简介](#intro)
-   [架构图](#architecture)
-   [目录](#files)
-   [使用说明](#usage)
-   [相关仓](#relevant)
## 简介<a name="intro"></a>

**无障碍子系统**提供在应用程序和辅助应用之间交换信息的标准机制，支持开发辅助应用增强无障碍功能体验。典型的应用场景包含两方面：

1. 为残障人士提供使用应用的能力：例如针对视觉障碍人士提供屏幕朗读等功能。
2. 为开发者提供与应用交互的能力：例如支持 UI 自动化测试框架、支持开发基于 UI 响应的辅助应用。

## 架构图<a name="architecture"></a>

**图1** 无障碍子系统架构图

![架构图](./figures/AccessibilityFramework.png)

**应用层**：

- 使用无障碍ExtensionAbility开发带有辅助能力的应用。
- 一般应用由ACE内部集成成为无障碍目标应用，同时可以使用无障碍辅助能力。
- 通过系统设定应用可以设定无障碍功能的开关。

**应用框架层**：

- AAkit：无障碍辅助能力开发套件，支持Native和TS语言开发，用来构建无障碍辅助能力运行环境。

- ASACkit：无障碍能力客户端开发套件，支持Native和TS语言开发，为应用程序提供使用无障碍辅助服务的能力。

- ACkit：无障碍功能设定开发套件，支持Native和TS语言开发，为系统应用提供设置无障碍功能开关的能力。

**服务层**：

- AccessibilityService：无障碍系统服务，管理无障碍辅助能力，无障碍能力客户端(ASACkit)连接的服务端，同时连接其他系统级服务以提供无障碍输入能力。

## 目录<a name="files"></a>

```
/foundation/barrierfree/accessibility
├── common
│   ├── interface           # 无障碍子系统服务接口层
│   └── log                 # 无障碍子系统hilog适配代码
├── figures                 # README图片
├── frameworks
│   ├── aafwk               # 无障碍辅助能力Kit实现代码
│   ├── acfwk               # 无障碍功能设定Kit实现代码
│   ├── asacfwk             # 无障碍能力客户端Kit实现代码
│   └── common              # 无障碍服务共同数据类型与常量
├── interfaces
│   ├── innerkits           # 无障碍子系统内部C/C++接口
│   └── kits                # 无障碍子系统对外TS接口
│       ├── js              # TS接口定义
│       └── napi            # TS接口实现
├── resources               # 无障碍子系统资源文件
├── sa_profile              # 无障碍子系统SA配置文件
└── services
    ├── aams                # 无障碍子系统服务代码
    ├── etc                 # 无障碍子系统配置文件
    └── test                # 无障碍子系统服务测试代码
```

## 使用说明<a name="usage"></a>

[1. 系统辅助功能配置](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/apis/js-apis-accessibility-config.md)

[2. 辅助扩展能力](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/apis/js-apis-accessibility-extension-context.md)

[3. 辅助功能](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/apis/js-apis-accessibility.md)

## 相关仓<a name="relevant"></a>

**无障碍子系统**

[accessibility](https://gitee.com/openharmony/accessibility)