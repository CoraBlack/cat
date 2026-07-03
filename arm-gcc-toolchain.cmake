set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# ⚠️ 关键：使用 CACHE FILEPATH 强制 CMake 将其识别为完整路径
# 这能完美解决路径中包含 "Program Files" 空格导致的解析问题
set(CMAKE_C_COMPILER "C:/Program Files/Arm/GNU Toolchain mingw-w64-x86_64-arm-none-eabi/bin/arm-none-eabi-gcc.exe" CACHE FILEPATH "C compiler")
set(CMAKE_CXX_COMPILER "C:/Program Files/Arm/GNU Toolchain mingw-w64-x86_64-arm-none-eabi/bin/arm-none-eabi-g++.exe" CACHE FILEPATH "C++ compiler")
set(CMAKE_ASM_COMPILER "C:/Program Files/Arm/GNU Toolchain mingw-w64-x86_64-arm-none-eabi/bin/arm-none-eabi-gcc.exe" CACHE FILEPATH "ASM compiler")

# 针对 STM32 裸机开发，防止 CMake 在配置阶段尝试链接主机库而报错
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# 可选：指定查找程序的路径前缀，帮助 CMake 找到 binutils (如 ar, objcopy)
set(CMAKE_FIND_ROOT_PATH "C:/Program Files/Arm/GNU Toolchain mingw-w64-x86_64-arm-none-eabi")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)