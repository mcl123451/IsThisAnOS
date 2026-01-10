#!/bin/bash
# IsThisAnOS 快速构建脚本

cd "$(dirname "$0")"

echo "Building IsThisAnOS..."

# 检查依赖
command -v gcc >/dev/null 2>&1 || { echo "gcc not found. Please install gcc."; exit 1; }
command -v nasm >/dev/null 2>&1 || { echo "nasm not found. Please install nasm."; exit 1; }
command -v ld >/dev/null 2>&1 || { echo "ld not found. Please install binutils."; exit 1; }
command -v grub-mkrescue >/dev/null 2>&1 || { echo "grub-mkrescue not found. Please install grub."; exit 1; }

# 清理并构建
make clean
make iso

if [ $? -eq 0 ]; then
    echo "Build successful!"
    echo "ISO created: build/IsThisAnOS.iso"
    echo ""
    echo "To run in QEMU: make run"
    echo "To debug: make debug"
else
    echo "Build failed!"
    exit 1
fi