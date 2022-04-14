# 无障碍软件服务子系统

-   [简介](#intro)
-   [目录](#files)
-   [相关仓](#relevant)
# 简介<a name="intro"></a>

**无障碍框架**提供在应用程序和辅助应用之间交换信息的标准机制，支持开发辅助应用增强无障碍功能体验。典型的应用场景包含两方面：

1. 为残障人士提供使用应用的能力：例如针对视觉障碍人士提供屏幕朗读等功能。
1. 为开发者提供与应用交互的能力：例如支持 UI 自动化测试框架、支持开发基于 UI 响应的辅助应用。

![](./figures/AccessibilityFramework.png)

# 目录<a name="files"></a>

```
/base/accessibility
├── frameworks
│   ├── aafwk               # AccessibleAbility Kit实现代码
│   └── asacfwk             # AccessibleAbilityClient Kit实现代码
├── interfaces
│   ├── innerkits           # AccessibleAbility 和 AccessibleAbilityClient 的子系统接口
│   └── kits                # AccessibleAbility 和 AccessibleAbilityClient 的开发者接口
├── sa_profile              # 系统服务配置文件
└── services
    └── aams                # AccessibleAbility 管理服务框架代码
    └── interface           # AccessibleAbility 管理服务接口层
    └── test                # 测试目录
```

# 相关仓<a name="relevant"></a>

**accessibility**

[distributedschedule_samgr](https://gitee.com/openharmony/distributedschedule_samgr)

[appexecfwk_standard](https://gitee.com/openharmony/appexecfwk_standard)

[notification_ces_standard](https://gitee.com/openharmony/notification_ces_standard)

[multimodalinput_input](https://gitee.com/openharmony/multimodalinput_input)