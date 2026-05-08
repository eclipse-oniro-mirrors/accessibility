# OHOS A11y Manager 使用说明

## 目录

- [快速开始](#快速开始)
- [状态查询命令](#状态查询命令)
- [能力管理命令](#能力管理命令)
- [屏幕放大命令](#屏幕放大命令)
- [无障碍快捷键命令](#无障碍快捷键命令)
- [高对比度命令](#高对比度命令)
- [反色命令](#反色命令)
- [动画命令](#动画命令)
- [音频命令](#音频命令)
- [色盲校正命令](#色盲校正命令)
- [点击持续时间命令](#点击持续命令)
- [忽略重复点击命令](#忽略重复点击命令)

## 快速开始

### 查看帮助

```bash
ohos-a11yManager --help
```

### 基本使用示例

```bash
# 启用屏幕放大
ohos-a11yManager magnification-set-state --state true

# 查看屏幕放大状态
ohos-a11yManager magnification-get-state
```

## 状态查询命令

### state-is-screen-reader-enabled

检查屏幕阅读器是否启用。

**权限**: `ohos.permission.READ_ACCESSIBILITY_CONFIG`

**参数**: 无

**示例**:
```bash
ohos-a11yManager state-is-screen-reader-enabled
```

**输出**:
```
Screen reader enabled: false
```

## 能力管理命令

### ability-enable-screen-reader

启用屏幕阅读器。

**权限**: `ohos.permission.WRITE_ACCESSIBILITY_CONFIG`

**参数**: 无

**示例**:
```bash
ohos-a11yManager ability-enable-screen-reader
```

**输出**:
```
Screen reader enabled successfully
```

### ability-disable-screen-reader

禁用屏幕阅读器。

**权限**: `ohos.permission.WRITE_ACCESSIBILITY_CONFIG`

**参数**: 无

**示例**:
```bash
ohos-a11yManager ability-disable-screen-reader
```

**输出**:
```
Screen reader disabled successfully
```

## 屏幕放大命令

### magnification-set-state

设置屏幕放大状态。

**权限**: `ohos.permission.WRITE_ACCESSIBILITY_CONFIG`

**参数**:
- `--state` (必需): true/false

**示例**:
```bash
# 启用屏幕放大
ohos-a11yManager magnification-set-state --state true

# 禁用屏幕放大
ohos-a11yManager magnification-set-state --state false
```

**输出**:
```
Screen magnification state set to: enabled
```

### magnification-get-state

获取屏幕放大状态。

**权限**: `ohos.permission.READ_ACCESSIBILITY_CONFIG`

**参数**: 无

**示例**:
```bash
ohos-a11yManager magnification-get-state
```

**输出**:
```
Screen magnification state: enabled
```

## 无障碍快捷键命令

### shortkey-set-state

设置无障碍快捷键状态。

**权限**: `ohos.permission.WRITE_ACCESSIBILITY_CONFIG`

**参数**:
- `--state` (必需): true/false

**示例**:
```bash
ohos-a11yManager shortkey-set-state --state true
```

**输出**:
```
Shortkey state set to: enabled
```

### shortkey-get-state

获取无障碍快捷键状态。

**权限**: `ohos.permission.READ_ACCESSIBILITY_CONFIG`

**参数**: 无

**示例**:
```bash
ohos-a11yManager shortkey-get-state
```

**输出**:
```
Shortkey state: enabled
```

## 高对比度命令

### high-contrast-set-state

设置高对比度文本状态。

**权限**: `ohos.permission.WRITE_ACCESSIBILITY_CONFIG`

**参数**:
- `--state` (必需): true/false

**示例**:
```bash
ohos-a11yManager high-contrast-set-state --state true
```

**输出**:
```
High contrast state set to: enabled
```

### high-contrast-get-state

获取高对比度文本状态。

**权限**: `ohos.permission.READ_ACCESSIBILITY_CONFIG`

**参数**: 无

**示例**:
```bash
ohos-a11yManager contrast-get-state
```

**输出**:
```
High contrast state: enabled
```

## 反色命令

### invert-color-set-state

设置反色状态。

**权限**: `ohos.permission.WRITE_ACCESSIBILITY_CONFIG`

**参数**:
- `--state` (必需): true/false

**示例**:
```bash
ohos-a11yManager invert-color-set-state --state true
```

**输出**:
```
Invert color state set to: enabled
```

### invert-color-get-state

获取反色状态。

**权限**: `ohos.permission.READ_ACCESSIBILITY_CONFIG`

**参数**: 无

**示例**:
```bash
ohos-a11yManager invert-color-get-state
```

**输出**:
```
Invert color state: enabled
```

## 动画命令

### animation-off-set-state

设置动画关闭状态。

**权限**: `ohos.permission.WRITE_ACCESSIBILITY_CONFIG`

**参数**:
- `--state` (必需): true/false

**示例**:
```bash
ohos-a11yManager animation-off-set-state --state true
```

**输出**:
```
Animation off state set to: enabled
```

### animation-off-get-state

获取动画关闭状态。

**权限**: `ohos.permission.READ_ACCESSIBILITY_CONFIG`

**参数**: 无

**示例**:
```bash
ohos-a11yManager animation-off-get-state
```

**输出**:
```
Animation off state: enabled
```

## 音频命令

### audio-set-mono

设置音频单声道状态。

**权限**: `ohos.permission.WRITE_ACCESSIBILITY_CONFIG`

**参数**:
- `--state` (必需): true/false

**示例**:
```bash
ohos-a11yManager audio-set-mono --state true
```

**输出**:
```
Audio mono state set to: enabled
```

### audio-get-mono

获取音频单声道状态。

**权限**: `ohos.permission.READ_ACCESSIBILITY_CONFIG`

**参数**: 无

**示例**:
```bash
ohos-a11yManager audio-get-mono
```

**输出**:
```
Audio mono state: enabled
```

### audio-set-balance

设置音频平衡。

**权限**: `ohos.permission.WRITE_ACCESSIBILITY_CONFIG`

**参数**:
- `--balance` (必需): 平衡值 (float)

**示例**:
```bash
ohos-a11yManager audio-set-balance --balance 0.5
```

**输出**:
```
Audio balance set to: 0.5
```

### audio-get-balance

获取音频平衡。

**权限**: `ohos.permission.READ_ACCESSIBILITY_CONFIG`

**参数**: 无

**示例**:
```bash
ohos-a11yManager audio-get-balance
```

**输出**:
```
Audio balance: 0.5
```

## 色盲校正命令

### daltonization-set-state

设置色盲校正状态。

**权限**: `ohos.permission.WRITE_ACCESSIBILITY_CONFIG`

**参数**:
- `--state` (必需): true/false

**示例**:
```bash
ohos-a11yManager daltonization-set-state --state true
```

**输出**:
```
Daltonization state set to: enabled
```

### daltonization-get-state

获取色盲校正状态。

**权限**: `ohos.permission.READ_ACCESSIBILITY_CONFIG`

**参数**: 无

**示例**:
```bash
ohos-a11yManager daltonization-get-state
```

**输出**:
```
Daltonization state: enabled
```

### daltonization-set-filter

设置色盲校正滤镜类型。

**权限**: `ohos.permission.WRITE_ACCESSIBILITY_CONFIG`

**参数**:
- `--type` (必需): 滤镜类型 (0=Normal, 1=Protanomaly, 2=Deuteranomaly, 3=Tritanomaly)

**示例**:
```bash
ohos-a11yManager daltonization-set-filter --type 1
```

**输出**:
```
Daltonization filter type set to: 1
```

### daltonization-get-filter

获取色盲校正滤镜类型。

**权限**: `ohos.permission.READ_ACCESSIBILITY_CONFIG`

**参数**: 无

**示例**:
```bash
ohos-a11yManager daltonization-get-filter
```

**输出**:
```
Daltonization filter type: 1
```

## 点击持续时间命令

### click-set-response-time

设置点击持续时间。

**权限**: `ohos.permission.WRITE_ACCESSIBILITY_CONFIG`

**参数**:
- `--time` (必需): 响应时间类型 (0=short(default), 1=medium, 2=long)

**示例**:
```bash
# 设置为短响应时间（默认）
ohos-a11yManager click-set-response-time --time 0

# 设置为中等响应时间
ohos-a11yManager click-set-response-time --time 1

# 设置为长响应时间
ohos-a11yManager click-set-response-time --time 2
```

**输出**:
```
Click response time set to: 0
```

### click-get-response-time

获取点击持续时间。

**权限**: `ohos.permission.READ_ACCESSIBILITY_CONFIG`

**参数**: 无

**示例**:
```bash
ohos-a11yManager click-get-response-time
```

**输出**:
```
Click response time: 0
```

## 忽略重复点击命令

### ignore-repeat-click-set-state

设置忽略重复点击状态。

**权限**: `ohos.permission.WRITE_ACCESSIBILITY_CONFIG`

**参数**:
- `--state` (必需): true/false

**示例**:
```bash
ohos-a11yManager ignore-repeat-click-set-state --state true
```

**输出**:
```
Ignore repeat click state set to: enabled
```

### ignore-repeat-click-get-state

获取忽略重复点击状态。

**权限**: `ohos.permission.READ_ACCESSIBILITY_CONFIG`

**参数**: 无

**示例**:
```bash
ohos-a11yManager ignore-repeat-click-get-state
```

**输出**:
```
Ignore repeat click state: enabled
```

### ignore-repeat-click-set-time

设置忽略重复点击时间间隔。

**权限**: `ohos.permission.WRITE_ACCESSIBILITY_CONFIG`

**参数**:
- `--interval` (必需): 时间间隔 (0=0.1s, 1=0.4s, 2=0.7s, 3=1.0s, 4=1.3s)

**示例**:
```bash
# 设置为 0.1s
ohos-a11yManager ignore-repeat-click-set-time --interval 0

# 设置为 0.4s
ohos-a11yManager ignore-repeat-click-set-time --interval 1

# 设置为 0.7s
ohos-a11yManager ignore-repeat-click-set-time --interval 2

# 设置为 1.0s
ohos-a11yManager ignore-repeat-click-set-time --interval 3

# 设置为 1.3s
ohos-a11yManager ignore-repeat-click-set-time --interval 4
```

**输出**:
```
Ignore repeat click time set to: 0
```

### ignore-repeat-click-get-time

获取忽略重复点击时间。

**权限**: `ohos.permission.READ_ACCESSIBILITY_CONFIG`

**参数**: 无

**示例**:
```bash
ohos-a11yManager ignore-repeat-click-get-time
```

**输出**:
```
Ignore repeat click time: 0
```

## 错误处理

当命令执行失败时，工具会返回非零退出码并输出错误信息：

```bash
$ ohos-a11yManager magnification-set-state
Error: --state is required

$ ohos-a11yManager ability-enable-screen-reader
Failed to enable screen reader. Error: 1
```

## 权限说明

| 权限 | 用途 | 适用命令 |
|------|------|----------|
| ohos.permission.READ_ACCESSIBILITY_CONFIG | 读取无障碍配置 | 所有 get/list/is 命令 |
| ohos.permission.WRITE_ACCESSIBILITY_CONFIG | 写入无障碍配置 | 所有 set/enable/disable 命令 |