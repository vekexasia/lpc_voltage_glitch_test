cmake_minimum_required(VERSION 3.13)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
include($ENV{PICO_SDK_PATH}/tools/CMakeLists.txt)

project(glitcher C CXX ASM)
#set(CMAKE_C_COMPILER "/usr/bin/arm-none-eabi-gcc")
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()
add_executable(${PROJECT_NAME}
        glitch.c)
pico_generate_pio_header(${PROJECT_NAME}
${CMAKE_CURRENT_LIST_DIR}/pio_serializer.pio
)

#dca
target_link_libraries(${PROJECT_NAME}
        pico_stdlib
        hardware_pio
        hardware_dma
        hardware_sync
        pico_multicore
        )

pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)

pico_add_extra_outputs(${PROJECT_NAME})
add_custom_command(TARGET glitcher POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy
        "$<TARGET_FILE_DIR:glitcher>/$<TARGET_FILE_BASE_NAME:glitcher>.uf2"
        "${PROJECT_SOURCE_DIR}/../pi2040_firmware.uf2"
        )
