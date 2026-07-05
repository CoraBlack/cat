set(CMAKE_VERBOSE_MAKEFILE ON)
cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME Cat)

# Enable compile command to ease indexing with e.g. clangd

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME}
   Core/hardware/ESP_SPI.h
   Core/hardware/OLED_Font.h)

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)



set(HARDWARE_DIR ${PROJECT_SOURCE_DIR}/Core/hardware)
set(STM_DIR ${PROJECT_SOURCE_DIR}/Core/Src)



file(GLOB HARDWARE_SRCS CONFIGURE_DEPENDS "${HARDWARE_DIR}/*.c")
file(GLOB STM_SRCS CONFIGURE_DEPENDS "${STM_DIR}/*.c")

list(FILTER STM_SRCS EXCLUDE REGEX ".*system_stm32f4xx\\.c$")



# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
   
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE


    # Add user sources here

    ${HARDWARE_SRCS}
    ${STM_SRCS}
    Core/hardware/MAX98357.c
    Core/Src/oled.c
    # Core/Src/animation.c
    Core/Src/oled_frames.c
    Core/Src/interaction.c
    Core/Src/emotion.c

    Core/hardware/ESP_SPI.c
    Core/hardware/Distance_Sensor.c
    Core/hardware/Motor.c
    Core/hardware/IR.c
    Core/hardware/INMP441.c
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE

    # Add user defined include paths

    ${HARDWARE_DIR}
    ${STM_DIR}
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
)


# 将 your_target_name 替换为你实际的 elf 目标名称（即 add_executable 中的名字）
set(ELF_FILE ${CMAKE_PROJECT_NAME}.elf) 

# 生成 .hex 文件（推荐，带地址校验，拖拽烧录最稳）
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${CMAKE_PROJECT_NAME}> ${CMAKE_PROJECT_NAME}.hex
    COMMENT "Generating HEX file for drag-and-drop flashing"
)