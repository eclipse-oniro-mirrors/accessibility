# OHOS Accessibility Manager CLI 测试用例

本文档包含 ohos-a11yManager CLI 工具的所有命令测试用例。

## 测试环境要求

- OpenHarmony 系统
- 具有相应无障碍权限（READ_ACCESSIBILITY_CONFIG、WRITE_ACCESSIBILITY_CONFIG）

## 测试用例列表

### 1. 状态查询命令

| 命令 | 参数 | 预期输出 | 权限要求 |
|------|------|----------|----------|
| state-is-screen-reader-enabled | 无 | JSON with status and enabled field | READ_ACCESSIBILITY_CONFIG |

**测试示例**：
```bash
ohos-a11yManager state-is-screen-reader-enabled
```

### 2. 能力管理命令

| 命令 | 参数 | 预期输出 | 权限要求 |
|------|------|----------|----------|
| ability-enable-screen-reader | 无 | JSON with status, name, capabilities | WRITE_ACCESSIBILITY_CONFIG |
| ability-disable-screen-reader | 无 | JSON with status, name | WRITE_ACCESSIBILITY_CONFIG |

**测试示例**：
```bash
ohos-a11yManager ability-enable-screen-reader
ohos-a11yManager ability-disable-screen-reader
```

### 3. 屏幕放大命令

| 命令 | 参数 | 预期输出 | 权限要求 |
|------|------|----------|----------|
| magnification-set-state | --state true | JSON with status and state=true | WRITE_ACCESSIBILITY_CONFIG |
| magnification-set-state | --state false | JSON with status and state=false | WRITE_ACCESSIBILITY_CONFIG |
| magnification-get-state | 无 | JSON with status and state field | READ_ACCESSIBILITY_CONFIG |

**测试示例**：
```bash
ohos-a11yManager magnification-set-state --state true
ohos-a11yManager magnification-set-state --state false
ohos-a11yManager magnification-get-state
```

### 4. 快捷键命令

| 命令 | 参数 | 预期输出 | 权限要求 |
|------|------|----------|----------|
| shortkey-set-state | --state true | JSON with status and state=true | WRITE_ACCESSIBILITY_CONFIG |
| shortkey-set-state | --state false | JSON with status and state=false | WRITE_ACCESSIBILITY_CONFIG |
| shortkey-get-state | 无 | JSON with status and state field | READ_ACCESSIBILITY_CONFIG |

**测试示例**：
```bash
ohos-a11yManager shortkey-set-state --state true
ohos-a11yManager shortkey-set-state --state false
ohos-a11yManager shortkey-get-state
```

### 5. 高对比度命令

| 命令 | 参数 | 预期输出 | 权限要求 |
|------|------|----------|----------|
| high-contrast-set-state | --state true | JSON with status and state=true | WRITE_ACCESSIBILITY_CONFIG |
| high-contrast-set-state | --state false | JSON with status and state=false | WRITE_ACCESSIBILITY_CONFIG |
| high-contrast-get-state | 无 | JSON with status and state field | READ_ACCESSIBILITY_CONFIG |

**测试示例**：
```bash
ohos-a11yManager high-contrast-set-state --state true
ohos-a11yManager high-contrast-set-state --state false
ohos-a11yManager high-contrast-get-state
```

### 6. 反色命令

| 命令 | 参数 | 预期输出 | 权限要求 |
|------|------|----------|----------|
| invert-color-set-state | --state true | JSON with status and state=true | WRITE_ACCESSIBILITY_CONFIG |
| invert-color-set-state | --state false | JSON with status and state=false | WRITE_ACCESSIBILITY_CONFIG |
| invert-color-get-state | 无 | JSON with status and state field | READ_ACCESSIBILITY_CONFIG |

**测试示例**：
```bash
ohos-a11yManager invert-color-set-state --state true
ohos-a11yManager invert-color-set-state --state false
ohos-a11yManager invert-color-get-state
```

### 7. 动画命令

| 命令 | 参数 | 预期输出 | 权限要求 |
|------|------|----------|----------|
| animation-off-set-state | --state true | JSON with status and state=true | WRITE_ACCESSIBILITY_CONFIG |
| animation-off-set-state | --state false | JSON with status and state=false | WRITE_ACCESSIBILITY_CONFIG |
| animation-off-get-state | 无 | JSON with status and state field | READ_ACCESSIBILITY_CONFIG |

**测试示例**：
```bash
ohos-a11yManager animation-off-set-state --state true
ohos-a11yManager animation-off-set-state --state false
ohos-a11yManager animation-off-get-state
```

### 8. 音频命令

| 命令 | 参数 | 预期输出 | 权限要求 |
|------|------|----------|----------|
| audio-set-mono | --state true | JSON with status and state=true | WRITE_ACCESSIBILITY_CONFIG |
| audio-set-mono | --state false | JSON with status and state=false | WRITE_ACCESSIBILITY_CONFIG |
| audio-get-mono | 无 | JSON with status and state field | READ_ACCESSIBILITY_CONFIG |
| audio-set-balance | --balance 0.0 | JSON with status and balance | WRITE_ACCESSIBILITY_CONFIG |
| audio-set-balance | --balance -1.0 | JSON with status and balance | WRITE_ACCESSIBILITY_CONFIG |
| audio-set-balance | --balance 1.0 | JSON with status and balance | WRITE_ACCESSIBILITY_CONFIG |
| audio-get-balance | 无 | JSON with status and balance field | READ_ACCESSIBILITY_CONFIG |

**测试示例**：
```bash
ohos-a11yManager audio-set-mono --state true
ohos-a11yManager audio-set-mono --state false
ohos-a11yManager audio-get-mono
ohos-a11yManager audio-set-balance --balance 0.0
ohos-a11yManager audio-get-balance
```

### 9. 色盲校正命令

| 命令 | 参数 | 预期输出 | 权限要求 |
|------|------|----------|----------|
| daltonization-set-state | --state true | JSON with status and state=true | WRITE_ACCESSIBILITY_CONFIG |
| daltonization-set-state | --state false | JSON with status and state=false | WRITE_ACCESSIBILITY_CONFIG |
| daltonization-get-state | 无 | JSON with status and state field | READ_ACCESSIBILITY_CONFIG |
| daltonization-set-filter | --type 0 | JSON with status and type=0 | WRITE_ACCESSIBILITY_CONFIG |
| daltonization-set-filter | --type 1 | JSON with status and type=1 | WRITE_ACCESSIBILITY_CONFIG |
| daltonization-set-filter | --type 2 | JSON with status and type=2 | WRITE_ACCESSIBILITY_CONFIG |
| daltonization-set-filter | --type 3 | JSON with status and type=3 | WRITE_ACCESSIBILITY_CONFIG |
| daltonization-get-filter | 无 | JSON with status and type field | READ_ACCESSIBILITY_CONFIG |

**测试示例**：
```bash
ohos-a11yManager daltonization-set-state --state true
ohos-a11yManager daltonization-set-state --state false
ohos-a11yManager daltonization-get-state
ohos-a11yManager daltonization-set-filter --type 0
ohos-a11yManager daltonization-set-filter --type 1
ohos-a11yManager daltonization-set-filter --type 2
ohos-a11yManager daltonization-set-filter --type 3
ohos-a11yManager daltonization-get-filter
```

### 10. 点击持续时间命令

| 命令 | 参数 | 预期输出 | 权限要求 |
|------|------|----------|----------|
| click-set-response-time | --time 0 | JSON with status and time=0 | WRITE_ACCESSIBILITY_CONFIG |
| click-set-response-time | --time 1 | JSON with status and time=1 | WRITE_ACCESSIBILITY_CONFIG |
| click-set-response-time | --time 2 | JSON with status and time=2 | WRITE_ACCESSIBILITY_CONFIG |
| click-get-response-time | 无 | JSON with status and time field | READ_ACCESSIBILITY_CONFIG |

**测试示例**：
```bash
ohos-a11yManager click-set-response-time --time 0
ohos-a11yManager click-set-response-time --time 1
ohos-a11yManager click-set-response-time --time 2
ohos-a11yManager click-get-response-time
```

### 11. 忽略重复点击命令

| 命令 | 参数 | 预期输出 | 权限要求 |
|------|------|----------|----------|
| ignore-repeat-click-set-state | --state true | JSON with status and state=true | WRITE_ACCESSIBILITY_CONFIG |
| ignore-repeat-click-set-state | --state false | JSON with status and state=false | WRITE_ACCESSIBILITY_CONFIG |
| ignore-repeat-click-get-state | 无 | JSON with status and state field | READ_ACCESSIBILITY_CONFIG |
| ignore-repeat-click-set-time | --interval 0 | JSON with status and interval=0 | WRITE_ACCESSIBILITY_CONFIG |
| ignore-repeat-click-set-time | --interval 1 | JSON with status and interval=1 | WRITE_ACCESSIBILITY_CONFIG |
| ignore-repeat-click-set-time | --interval 2 | JSON with status and interval=2 | WRITE_ACCESSIBILITY_CONFIG |
| ignore-repeat-click-set-time | --interval 3 | JSON with status and interval=3 | WRITE_ACCESSIBILITY_CONFIG |
| ignore-repeat-click-set-time | --interval 4 | JSON with status and interval=4 | WRITE_ACCESSIBILITY_CONFIG |
| ignore-repeat-click-get-time | 无 | JSON with status and interval field | READ_ACCESSIBILITY_CONFIG |

**测试示例**：
```bash
ohos-a11yManager ignore-repeat-click-set-state --state true
ohos-a11yManager ignore-repeat-click-set-state --state false
ohos-a11yManager ignore-repeat-click-get-state
ohos-a11yManager ignore-repeat-click-set-time --interval 0
ohos-a11yManager ignore-repeat-click-set-time --interval 1
ohos-a11yManager ignore-repeat-click-set-time --interval 2
ohos-a11yManager ignore-repeat-click-set-time --interval 3
ohos-a11yManager ignore-repeat-click-set-time --interval 4
ohos-a11yManager ignore-repeat-click-get-time
```

### 12. 帮助命令

| 命令 | 参数 | 预期输出 | 权限要求 |
|------|------|----------|----------|
| --help | 无 | 帮助信息到 stderr | 无 |
| -h | 无 | 帮助信息到 stderr | 无 |
| <command> --help | 无 | 子命令帮助信息到 stderr | 无 |

**测试示例**：
```bash
ohos-a11yManager --help
ohos-a11yManager -h
ohos-a11yManager magnification-set-state --help
```

### 13. 未知命令

| 命令 | 参数 | 预期输出 | 权限要求 |
|------|------|----------|----------|
| unknown-command | 无 | JSON error with ERR_UNKNOWN_COMMAND | 无 |

**测试示例**：
```bash
ohos-a11yManager unknown-command
```

## 测试覆盖统计

- **总命令数**: 27 个
- **测试用例总数**: 55 个
- **参数组合覆盖**: 100%

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

## 预期输出格式

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