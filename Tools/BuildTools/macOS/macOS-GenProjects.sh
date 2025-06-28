#!/bin/bash

# 进入项目根目录
cd "$(dirname "$0")/../../.."

# 生成Xcode项目
echo "Generating Xcode projects..."
./vendor/premake/premake5 xcode4

echo "Xcode projects generated successfully!"
echo "Project files are located in: Project/macOS/Xcode/"
read -p "Press any key to continue..." 