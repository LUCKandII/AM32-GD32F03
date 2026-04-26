#!/bin/bash
#
# setup_f103_symlinks.sh
#
# 为 GD32F103 移植创建 GD32F10x 固件库符号链接
#
# 使用方法:
#   ./setup_f103_symlinks.sh
#
# 执行前提:
#   - GD32_Basic 目录位于 AM32 同级目录
#   - GD32_Basic/GD32F10x_Firmware_Library_V2.7.0 存在
#
# 示例目录结构:
#   GD32_Basic/
#   ├── AM32/              <- 当前仓库
#   │   └── setup_f103_symlinks.sh
#   └── GD32F10x_Firmware_Library_V2.7.0/
#

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
DRIVERS_DIR="$SCRIPT_DIR/Mcu/f103/Drivers"
FIRMWARE_LIB_DIR="$SCRIPT_DIR/../../../GD32_Basic/GD32F10x_Firmware_Library_V2.7.0/Firmware"

echo "=== GD32F103 Symlink Setup ==="
echo ""

# 检查固件库是否存在
if [ ! -d "$FIRMWARE_LIB_DIR" ]; then
    echo "错误: 固件库未找到"
    echo "预期路径: $FIRMWARE_LIB_DIR"
    echo ""
    echo "请确保 GD32_Basic/GD32F10x_Firmware_Library_V2.7.0 目录存在"
    exit 1
fi

echo "固件库路径: $FIRMWARE_LIB_DIR"
echo ""

# 进入 Drivers 目录
cd "$DRIVERS_DIR"

# 删除已存在的链接或空目录
echo "清理旧链接..."
rm -rf CMSIS GD32F10x_standard_peripheral

# 创建符号链接 (使用相对路径)
echo "创建 CMSIS 符号链接..."
ln -s ../../../../GD32_Basic/GD32F10x_Firmware_Library_V2.7.0/Firmware/CMSIS CMSIS

echo "创建 GD32F10x_standard_peripheral 符号链接..."
ln -s ../../../../GD32_Basic/GD32F10x_Firmware_Library_V2.7.0/Firmware/GD32F10x_standard_peripheral GD32F10x_standard_peripheral

echo ""
echo "=== 验证 ==="
ls -la "$DRIVERS_DIR"

echo ""
echo "完成！现在可以执行 'make f103' 进行编译"
