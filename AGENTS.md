# Accessibility子系统 AGENTS.md
 	 
## 基本信息

无障碍子系统提供在应用程序和辅助应用之间交换信息的标准机制，支持开发辅助应用增强无障碍功能体验。主要功能包括：
- 为残障人士提供使用应用的能力（如读屏软件）
- 为开发者提供与应用交互的能力（如UI自动化测试框架、基于UI响应的辅助应用）
- 支持多种无障碍能力类型：视觉障碍、听觉障碍、肢体障碍等

## 架构及依赖

### 核心架构

采用分层架构设计，包含以下层次：

1. **应用层**：
- 无障碍Extension Ability（AA）：使用ExtensionAbility开发带有辅助能力的应用
- 普通应用（ACE）：通过ACE内部集成成为无障碍目标应用
- 系统设置应用：通过ACfwk配置无障碍功能开关

2. **应用框架层**：
- **AAFwk**：无障碍辅助能力开发套件，支持Native和TS语言开发
- **ASACfwk**：无障碍能力客户端开发套件，为应用程序提供使用无障碍辅助服务的能力
- **ACfwk**：无障碍功能设定开发套件，为系统应用提供设置无障碍功能开关的能力

3. **服务层**：
- **AccessibilityService（AAMS）**：无障碍系统服务，管理无障碍辅助能力，作为ASACfwk连接的服务端，同时连接其他系统级服务

4. **接口层**：
- **InnerKits**：内部C/C++接口，提供给Native应用使用
- **Kits**：对外TS接口，提供给JS应用使用

5. **公共层**：
- **Interface**：IPC接口定义和序列化
- **Log**：日志适配
- **ETC**：配置文件

### 设计实现

**核心服务类**：
- `AccessibleAbilityManagerService`：系统服务入口，继承SystemAbility和AccessibleAbilityManagerServiceStub
- `AccessibleAbilityConnection`：管理与AA的连接
- `AccessibleAbilityChannel`：AA与AAMS的通信桥梁
- `AccessibilityAccountData`：多账户数据管理
- `AccessibilityWindowManager`：窗口管理
- `AccessibilityWindowConnection`：窗口连接管理

**Framework核心类**：
- `AccessibleAbilityClientImpl`：AA客户端实现
- `AccessibilitySystemAbilityClientImpl`：ACE客户端实现
- `AccessibilityConfig::Impl`：配置管理实现

### 通信机制

使用OpenHarmony Binder IPC机制，采用Proxy-Stub模式：

**关键IPC接口**：
- `IAccessibleAbilityManagerService`：AAMS服务接口
- `IAccessibleAbilityClient`：AA客户端接口
- `IAccessibleAbilityChannel`：AA与AAMS通信接口
- `IAccessibilityElementOperator`：元素操作符接口
- `IAccessibilityElementOperatorCallback`：元素操作回调接口

**数据流**：
1. **事件流**：ACE → AAMS → AA
2. **元素查询流**：AA → AAMS → ACE → AAMS → AA
3. **动作执行流**：AA → AAMS → ACE → AAMS → AA
4. **配置流**：Settings App → AAMS → 观察者


## 目录结构

```
/foundation/barrierfree/accessibility
├── common
│   ├── interface           # 无障碍子系统服务接口层
│   │   ├── include         # 接口头文件
│   │   └── test            # 接口测试
│   ├── log                 # 无障碍子系统hilog适配代码
│   └── etc                 # 配置文件
├── figures                 # README图片
├── frameworks
│   ├── aafwk               # 无障碍辅助能力Kit实现代码
│   │   ├── include         # 头文件
│   │   ├── src             # 源代码
│   │   └── test            # 测试代码
│   ├── acfwk               # 无障碍功能设定Kit实现代码
│   │   ├── include         # 头文件
│   │   ├── src             # 源代码
│   │   └── test            # 测试代码
│   ├── asacfwk             # 无障碍能力客户端Kit实现代码
│   │   ├── include         # 头文件
│   │   ├── src             # 源代码
│   │   └── test            # 测试代码
│   └── common              # 无障碍服务共同数据类型与常量
│       ├── src             # 源代码
│       └── test            # 测试代码
├── interfaces
│   ├── innerkits           # 无障碍子系统内部C/C++接口
│   │   ├── aafwk           # AAFwk接口
│   │   ├── acfwk           # ACfwk接口
│   │   ├── asacfwk         # ASACfwk接口
│   │   ├── common          # 公共接口
│   │   └── test            # 测试代码
│   └── kits                # 无障碍子系统对外TS接口
│       ├── js              # JS接口
│       ├── napi            # NAPI接口实现
│       │   ├── accessibility_config
│       │   ├── accessibility_extension
│       │   ├── accessibility_extension_context
│       │   ├── accessibility_extension_module_loader
│       │   ├── accessibility_gesture_path
│       │   ├── accessibility_gesture_point
│       │   ├── include
│       │   └── src
│       ├── cj              # Cangjie接口
│       │   ├── include
│       │   └── src
│       └── ani             # ANI接口
│           ├── accessibility_config
│           ├── accessibility_extension
│           ├── accessibility_extension_module
│           ├── ets
│           ├── include
│           └── src
├── resources               # 无障碍子系统资源文件
│   └── config              # 配置文件
├── sa_profile              # 无障碍子系统SA配置文件
├── services
│   ├── aams                # 无障碍子系统服务代码
│   │   ├── include         # 头文件
│   │   ├── src             # 源代码
│   │   └── test            # 测试代码
│   ├── aams_ext            # 扩展服务代码
│   │   ├── include         # 头文件
│   │   ├── src             # 源代码
│   │   └── test            # 测试代码
│   ├── etc                 # 无障碍子系统配置文件
│   └── test                # 模块测试
│       ├── mock            # Mock测试
│       └── moduletest      # 模块测试
├── BUILD.gn                # 构建配置
├── bundle.json             # 组件配置
├── MODULE.md               # 模块模板
└── README_zh.md            # 中文文档
```

## 构建

### 构建命令

```bash
./build.sh --product-name {product_name} --build-target accessibility
```