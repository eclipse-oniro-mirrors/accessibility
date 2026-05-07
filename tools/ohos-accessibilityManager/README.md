# OHOS A11y Manager

OpenHarmony 无障碍管理 CLI 工具，提供命令行接口用于管理和配置无障碍功能。

## 功能特性

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

## 编译

```bash
# 在源码根目录执行
./build.sh --product-name <product> --build-target ohos_a11y_manager
```

## 权限要求

- `ohos.permission.READ_ACCESSIBILITY_CONFIG` - 读取无障碍配置
- `ohos.permission.WRITE_ACCESSIBILITY_CONFIG` - 写入无障碍配置

## 快速开始

```bash
# 查看帮助
ohos-a11yManager --help

# 启用屏幕放大
ohos-a11yManager magnification-set-state --state true

# 查看屏幕放大状态
ohos-a11yManager magnification-get-state
```

## 详细文档

- [使用说明](docs/USAGE.md) - 完整的命令参考和使用示例

## 许可证

Apache License 2.0