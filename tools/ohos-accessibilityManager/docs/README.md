# OHOS Accessibility Manager

## 概述

OHOS Accessibility Manager 是 OpenHarmony 无障碍服务的命令行管理工具，提供完整的功能接口用于查询和配置系统的无障碍特性。该工具适用于开发者测试和系统管理员配置无障碍设置。

## 功能列表

- 状态查询：查询屏幕阅读器状态
- 能力管理：启用/禁用无障碍能力
- 屏幕放大：配置屏幕放大功能
- 无障碍快捷键：管理无障碍快捷键功能
- 高对比度：配置高对比度文本
- 反色：管理颜色反转功能
- 动画：控制动画开关
- 音频：设置单声道和音频平衡
- 色盲校正：配置色盲校正滤镜
- 点击持续时间：配置点击持续时间
- 忽略重复点击：管理重复点击过滤

## 依赖

### 系统能力
- AccessibilityConfig - 无障碍配置管理接口
- AccessibilitySystemAbilityClient - 无障碍系统能力客户端接口

### 权限
- `ohos.permission.READ_ACCESSIBILITY_CONFIG` - 读取无障碍配置
- `ohos.permission.WRITE_ACCESSIBILITY_CONFIG` - 写入无障碍配置

## 基本用法

```bash
ohos-a11yManager <command> [options]
```

## 命令列表

| 命令 | 说明 | 参数 | 权限 | 前置依赖 |
|------|------|------|------|----------|
| state-is-screen-reader-enabled | 检查屏幕阅读器是否已启用 | 无 | READ_ACCESSIBILITY_CONFIG | 无 |
| ability-enable-screen-reader | 启用屏幕阅读器功能 | 无 | WRITE_ACCESSIBILITY_CONFIG | 无 |
| ability-disable-screen-reader | 禁用屏幕阅读器功能 | 无 | WRITE_ACCESSIBILITY_CONFIG | 无 |
| magnification-set-state | 设置屏幕放大功能的状态 | --state | WRITE_ACCESSIBILITY_CONFIG | 无 |
| magnification-get-state | 获取屏幕放大功能的状态 | 无 | READ_ACCESSIBILITY_CONFIG | 无 |
| shortkey-set-state | 设置快捷键功能的状态 | --state | WRITE_ACCESSIBILITY_CONFIG | 无 |
| shortkey-get-state | 获取快捷键功能的状态 | 无 | READ_ACCESSIBILITY_CONFIG | 无 |
| high-contrast-set-state | 设置高对比度文本功能的状态 | --state | WRITE_ACCESSIBILITY_CONFIG | 无 |
| high-contrast-get-state | 获取高对比度文本功能的状态 | 无 | READ_ACCESSIBILITY_CONFIG | 无 |
| invert-color-set-state | 设置颜色反转功能的状态 | --state | WRITE_ACCESSIBILITY_CONFIG | 无 |
| invert-color-get-state | 获取颜色反转功能的状态 | 无 | READ_ACCESSIBILITY_CONFIG | 无 |
| animation-off-set-state | 设置关闭动画功能的状态 | --state | WRITE_ACCESSIBILITY_CONFIG | 无 |
| animation-off-get-state | 获取关闭动画功能的状态 | 无 | READ_ACCESSIBILITY_CONFIG | 无 |
| audio-set-mono | 设置音频单声道功能的状态 | --state | WRITE_ACCESSIBILITY_CONFIG | 无 |
| audio-get-mono | 获取音频单声道功能的状态 | 无 | READ_ACCESSIBILITY_CONFIG | 无 |
| audio-set-balance | 设置音频平衡值 | --balance | WRITE_ACCESSIBILITY_CONFIG | 无 |
| audio-get-balance | 获取音频平衡值 | 无 | READ_ACCESSIBILITY_CONFIG | 无 |
| daltonization-set-state | 设置色盲校正功能的状态 | --state | WRITE_ACCESSIBILITY_CONFIG | 无 |
| daltonization-get-state | 获取色盲校正功能的状态 | 无 | READ_ACCESSIBILITY_CONFIG | 无 |
| daltonization-set-filter | 设置色盲校正滤镜类型 | --type | WRITE_ACCESSIBILITY_CONFIG | 无 |
| daltonization-get-filter | 获取色盲校正滤镜类型 | 无 | READ_ACCESSIBILITY_CONFIG | 无 |
| click-set-response-time | 设置点击响应时间 | --time | WRITE_ACCESSIBILITY_CONFIG | 无 |
| click-get-response-time | 获取点击响应时间 | 无 | READ_ACCESSIBILITY_CONFIG | 无 |
| ignore-repeat-click-set-state | 设置忽略重复点击功能的状态 | --state | WRITE_ACCESSIBILITY_CONFIG | 无 |
| ignore-repeat-click-get-state | 获取忽略重复点击功能的状态 | 无 | READ_ACCESSIBILITY_CONFIG | 无 |
| ignore-repeat-click-set-time | 设置忽略重复点击的时间间隔 | --interval | WRITE_ACCESSIBILITY_CONFIG | 无 |
| ignore-repeat-click-get-time | 获取忽略重复点击的时间间隔 | 无 | READ_ACCESSIBILITY_CONFIG | 无 |

**前置依赖说明**：
- **无**：该命令可直接执行，无需前置条件

## 参数说明

### --state
- **类型**: 布尔值
- **可选值**: true, false
- **说明**: 用于启用或禁用某个功能

### --time
- **类型**: 整数
- **可选值**: 0 (short), 1 (medium), 2 (long)
- **说明**: 点击响应时间设置

### --interval
- **类型**: 整数
- **可选值**: 0 (0.1s), 1 (0.4s), 2 (0.7s), 3 (1.0s), 4 (1.3s)
- **说明**: 忽略重复点击的时间间隔

### --balance
- **类型**: 浮点数
- **可选值**: -1.0 到 1.0
- **说明**: 音频平衡值

### --type
- **类型**: 整数
- **可选值**: 0 (Normal), 1 (Protanomaly), 2 (Deuteranomaly), 3 (Tritanomaly)
- **说明**: 色盲校正滤镜类型

## 示例

### 状态查询命令
```bash
# 检查屏幕阅读器是否已启用
ohos-a11yManager state-is-screen-reader-enabled
```

### 能力管理命令
```bash
# 启用屏幕阅读器
ohos-a11yManager ability-enable-screen-reader

# 禁用屏幕阅读器
ohos-a11yManager ability-disable-screen-reader
```

### 屏幕放大命令
```bash
# 启用屏幕放大
ohos-a11yManager magnification-set-state --state true

# 禁用屏幕放大
ohos-a11yManager magnification-set-state --state false

# 查询屏幕放大状态
ohos-a11yManager magnification-get-state
```

### 快捷键命令
```bash
# 启用快捷键
ohos-a11yManager shortkey-set-state --state true

# 禁用快捷键
ohos-a11yManager shortkey-set-state --state false

# 查询快捷键状态
ohos-a11yManager shortkey-get-state
```

### 高对比度命令
```bash
# 启用高对比度文本
ohos-a11yManager high-contrast-set-state --state true

# 禁用高对比度文本
ohos-a11yManager high-contrast-set-state --state false

# 查询高对比度状态
ohos-a11yManager high-contrast-get-state
```

### 反色命令
```bash
# 启用颜色反转
ohos-a11yManager invert-color-set-state --state true

# 禁用颜色反转
ohos-a11yManager invert-color-set-state --state false

# 查询颜色反转状态
ohos-a11yManager invert-color-get-state
```

### 动画命令
```bash
# 关闭动画
ohos-a11yManager animation-off-set-state --state true

# 开启动画
ohos-a11yManager animation-off-set-state --state false

# 查询动画状态
ohos-a11yManager animation-off-get-state
```

### 音频命令
```bash
# 启用音频单声道
ohos-a11yManager audio-set-mono --state true

# 禁用音频单声道
ohos-a11yManager audio-set-mono --state false

# 查询音频单声道状态
ohos-a11yManager audio-get-mono

# 设置音频平衡
ohos-a11yManager audio-set-balance --balance 0.0

# 获取音频平衡值
ohos-a11yManager audio-get-balance
```

### 色盲校正命令
```bash
# 启用色盲校正
ohos-a11yManager daltonization-set-state --state true

# 禁用色盲校正
ohos-a11yManager daltonization-set-state --state false

# 查询色盲校正状态
ohos-a11yManager daltonization-get-state

# 设置色盲校正滤镜类型
ohos-a11yManager daltonization-set-filter --type 0  # Normal
ohos-a11yManager daltonization-set-filter --type 1  # Protanomaly
ohos-a11yManager daltonization-set-filter --type 2  # Deuteranomaly
ohos-a11yManager daltonization-set-filter --type 3  # Tritanomaly

# 获取色盲校正滤镜类型
ohos-a11yManager daltonization-get-filter
```

### 点击持续时间命令
```bash
# 设置点击响应时间为短
ohos-a11yManager click-set-response-time --time 0

# 设置点击响应时间为中
ohos-a11yManager click-set-response-time --time 1

# 设置点击响应时间为长
ohos-a11yManager click-set-response-time --time 2

# 获取点击响应时间
ohos-a11yManager click-get-response-time
```

### 忽略重复点击命令
```bash
# 启用忽略重复点击
ohos-a11yManager ignore-repeat-click-set-state --state true

# 禁用忽略重复点击
ohos-a11yManager ignore-repeat-click-set-state --state false

# 查询忽略重复点击状态
ohos-a11yManager ignore-repeat-click-get-state

# 设置忽略重复点击的时间间隔
ohos-a11yManager ignore-repeat-click-set-time --interval 0  # 0.1s
ohos-a11yManager ignore-repeat-click-set-time --interval 1  # 0.4s
ohos-a11yManager ignore-repeat-click-set-time --interval 2  # 0.7s
ohos-a11yManager ignore-repeat-click-set-time --interval 3  # 1.0s
ohos-a11yManager ignore-repeat-click-set-time --interval 4  # 1.3s

# 获取忽略重复点击的时间间隔
ohos-a11yManager ignore-repeat-click-get-time
```

### 帮助命令
```bash
# 查看全局帮助
ohos-a11yManager --help

# 查看子命令帮助
ohos-a11yManager magnification-set-state --help
```

## 输出格式

### 成功响应
```json
{
  "type": "result",
  "status": "success",
  "data": { ... }
}
```

### 失败响应
```json
{
  "type": "result",
  "status": "failed",
  "data": "",
  "errCode": "ERR_XXX",
  "errMsg": "Error description",
  "suggestion": "Suggested next operation"
}
```

## 编译

```bash
# 在源码根目录执行
./build.sh --product-name <product> --build-target ohos-a11yManager
```

## 许可证

Apache License 2.0