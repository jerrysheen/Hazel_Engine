#!/bin/bash

# 进入项目根目录
cd "$(dirname "$0")/../../.."

# 生成Makefiles
echo "Generating Makefiles..."
./vendor/premake/premake5 gmake2

echo "Makefiles generated successfully!"
echo "Project files are located in: Project/Linux/Makefiles/"
echo "To build the project, navigate to the generated directory and run 'make'"
read -p "Press any key to continue..." 