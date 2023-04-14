cmake_minimum_required(VERSION 3.13)
project("lpc")
enable_language(ASM)
set(CMAKE_BUILD_PARALLEL_LEVEL 16)
set(TARGET_NAME main)

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(SYSINC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/common)
set(LD_SCRIPT_DIR ${SYSINC_DIR})
set(WARNING_FLAGS "-Wno-unknown-pragmas -Wno-implicit-function-declaration -Wno-unused-variable -Wno-unused-const-variable")
set(COMMON_FLAGS "-mthumb -mcpu=cortex-m3 -g -Wall -Os -flto -ffunction-sections -fdata-sections -D__STACK_SIZE=0x0a00 -DdebugPrintf -DLOWSTACKWARNING ${WARNING_FLAGS}")

set(COMMON_FLAGS "${COMMON_FLAGS} -DLPC175x_6x")

set(CMAKE_C_FLAGS "${COMMON_FLAGS} -std=gnu99")
set(CMAKE_ASM_FLAGS "${COMMON_FLAGS} -D__STARTUP_CLEAR_BSS -D__START=main")

set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")

# Set the path to the compiler and linker tools
set(CROSS_COMPILE /opt/gcc-arm-none-eabi/bin/arm-none-eabi-)
set(CMAKE_C_COMPILER ${CROSS_COMPILE}gcc)
set(CMAKE_ASM_COMPILER ${CMAKE_C_COMPILER})
set(CMAKE_OBJCOPY ${CROSS_COMPILE}objcopy)
set(CMAKE_OBJDUMP ${CROSS_COMPILE}objdump)
set(CMAKE_SIZE ${CROSS_COMPILE}size)

# Find all source files
file(GLOB SRC
        main.c
        common/LPC175x_6x/*.S
        common/LPC175x_6x/*.c
        common/*.c
        )

# Define the executable target
add_executable(${TARGET_NAME} ${SRC})
# Include directories
target_include_directories(${TARGET_NAME} PRIVATE
        ${PROJECT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/common/LPC175x_6x
        ${PROJECT_SOURCE_DIR}/common

        )


# Generate the binary, hex, disassembly, and size output files
set(BIN_FILE ${TARGET_NAME}.bin)
set(HEX_FILE ${TARGET_NAME}.hex)
set(DIS_FILE ${TARGET_NAME}.dis)
set(MAP_FILE ${TARGET_NAME}.map)

# Linker flags
set(GC "-Wl,--gc-sections")
set(MAP "-Wl,-Map=${MAP_FILE}")
set(LDFLAGS "${GC} ${MAP} -mthumb -mcpu=cortex-m3 -lc -lc --specs=nano.specs -L${LD_SCRIPT_DIR} -T ${PROJECT_SOURCE_DIR}/common/LPC175x_6x/LPC175x_6x.ld")
target_link_libraries(${TARGET_NAME} ${LDFLAGS})

# Define the output files
set_target_properties(${TARGET_NAME} PROPERTIES
        OUTPUT_NAME ${TARGET_NAME}
        SUFFIX .elf
        )

add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} ARGS -j .reset_vector -O binary ${TARGET_NAME}.elf reset_vector.bin
        COMMAND ${CMAKE_OBJCOPY} ARGS -j .text -O binary ${TARGET_NAME}.elf main_code.bin
        #    COMMAND dd ARGS if=/dev/zero of=padding.bin bs=1 count=$((0x1000 - 0xcc))
        #    COMMAND cat ARGS reset_vector.bin padding.bin main_code.bin > final_binary.bin

        COMMAND ${CMAKE_OBJCOPY} ARGS -O binary ${TARGET_NAME}.elf ${BIN_FILE}
        COMMAND ${CMAKE_OBJCOPY} ARGS -O ihex ${TARGET_NAME}.elf ${HEX_FILE}
        COMMAND ${CMAKE_OBJDUMP} ARGS -S ${TARGET_NAME}.elf > ${DIS_FILE}
        COMMAND ${CMAKE_SIZE} ARGS --format=SysV -x ${TARGET_NAME}.elf
        COMMAND ${CMAKE_SIZE} ARGS ${TARGET_NAME}.elf
        )

# Define the clean target
add_custom_target(clean_all
        COMMAND ${CMAKE_COMMAND} -E remove -f ${BIN_FILE} ${HEX_FILE} ${DIS_FILE} ${MAP_FILE} ${TARGET_NAME}.elf
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_BINARY_DIR}/cmake/cmake_clean.cmake
        COMMENT "Clean all"
        )

# Define the flash and upload targets
add_custom_target(flash
        COMMAND echo "Flash not configured"
        DEPENDS ${BIN_FILE}
        COMMENT "Flash ${BIN_FILE} to device"
        )

add_custom_target(upload
        COMMAND echo "Uploading ${BIN_FILE} to device"
        DEPENDS ${BIN_FILE}
        COMMENT "Upload ${BIN_FILE} to device"
        )