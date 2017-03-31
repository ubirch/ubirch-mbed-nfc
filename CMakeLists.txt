cmake_minimum_required(VERSION 3.5)
set(CMAKE_BUILD_TYPE Debug)

project(NTAG C CXX)

# == MBED OS 5 settings ==
set(FEATURES netsocket)

add_definitions(
        -DTOOLCHAIN_GCC
        -DCPU_MK82FN256VLL15
        -DTARGET_K82F
        -DTARGET_UBIRCH1
        -DMBED_CONF_RTOS_PRESENT
        -DMBED_CONF_NSAPI_PRESENT
        -DMBED_CONF_EVENTS_PRESENT
        -D__MBED__
        -DDEVICE_I2C
        -DDEVICE_SERIAL
        -DDEVICE_TRNG
        -DDEVICE_INTERRUPTIN
        -DMBED_CONF_EVENTS_PRESENT=1
        -DMBED_CONF_RTOS_PRESENT=1
        -DMBED_CONF_PLATFORM_STDIO_BAUD_RATE=9600
        -DMBED_CONF_NSAPI_PRESENT=1
        -DMBED_CONF_PLATFORM_DEFAULT_SERIAL_BAUD_RATE=9600
        -DMBED_CONF_PLATFORM_STDIO_FLUSH_AT_EXIT=1
        -DMBED_CONF_PLATFORM_STDIO_CONVERT_NEWLINES=0
        -DDEVICE_RTC
)

set(MBED_OS
        mbed-os
        mbed-os/cmsis
        mbed-os/drivers
        mbed-os/events
        mbed-os/events/enqueue
        mbed-os/hal
        mbed-os/hal/storage_abstraction
        mbed-os/platform
        mbed-os/rtos
        mbed-os/features
        mbed-os/rtos/rtx/TARGET_CORTEX_M
        mbed-os/rtos/rtx/TARGET_CORTEX_M/TARGET_RTOS_M4_M7
        mbed-os/targets/TARGET_Freescale
        mbed-os/targets/TARGET_Freescale/TARGET_KSDK2_MCUS/api
        mbed-os/targets/TARGET_Freescale/TARGET_KSDK2_MCUS/TARGET_K82F
        mbed-os/targets/TARGET_Freescale/TARGET_KSDK2_MCUS/TARGET_K82F/device
        mbed-os/targets/TARGET_Freescale/TARGET_KSDK2_MCUS/TARGET_K82F/drivers
        mbed-os/targets/TARGET_Freescale/TARGET_KSDK2_MCUS/TARGET_K82F/device/TOOLCHAIN_GCC_ARM
        mbed-os/targets/TARGET_Freescale/TARGET_KSDK2_MCUS/TARGET_K82F/TARGET_UBIRCH1
        )

set(MBED_OS_SRCS "" source/main.cpp)
foreach (DIR in ${MBED_OS})
    if (DIR MATCHES "mbed-os/features")
        foreach(FEATURE in ${FEATURES})
            file(GLOB_RECURSE includes
                    ${DIR}/${FEATURE}/*.cpp ${DIR}/${FEATURE}/*.c ${DIR}/${FEATURE}/*.s ${DIR}/${FEATURE}/*.S)
            list(APPEND MBED_OS_SRCS ${includes})
        endforeach()
    else ()
        file(GLOB includes ${DIR}/*.cpp ${DIR}/*.c ${DIR}/*.s ${DIR}/*.S)
        list(APPEND MBED_OS_SRCS ${includes})
    endif ()
endforeach ()

add_library(mbed-os ${MBED_OS_SRCS})
include_directories(${MBED_OS})
# == END MBED OS 5 ==

#add_library(BME280 BME280/BME280.cpp)
#target_include_directories(BME280 PUBLIC BME280)

#add_library(mbed-os-quectel-m66-driver
#        mbed-os-quectelM66-driver/M66Interface.cpp
#        mbed-os-quectelM66-driver/M66ATParser/M66ATParser.cpp
#        mbed-os-quectelM66-driver/M66ATParser/BufferedSerial/BufferedPrint.c
#        mbed-os-quectelM66-driver/M66ATParser/BufferedSerial/BufferedSerial.cpp
#        mbed-os-quectelM66-driver/M66ATParser/BufferedSerial/Buffer/MyBuffer.cpp
#        )
#target_include_directories(mbed-os-quectel-m66-driver PUBLIC
#        mbed-os-quectelM66-driver
#        mbed-os-quectelM66-driver/M66ATParser
#        mbed-os-quectelM66-driver/M66ATParser/BufferedSerial
#        mbed-os-quectelM66-driver/M66ATParser/BufferedSerial/Buffer
#        )

set(NTAG
        NTAG_I2C_API/src/HAL_BSP11U6x
        NTAG_I2C_API/src/HAL_BSP_LPC8xx
        NTAG_I2C_API/src/HAL_BSP_WINDOWS
        NTAG_I2C_API/src/HAL_BSPMSP430
        NTAG_I2C_API/src/HAL_I2C
        NTAG_I2C_API/src/HAL_ISR
        NTAG_I2C_API/src/HAL_NTAG
        NTAG_I2C_API/src/HAL_TMR
        NTAG_I2C_API/src/HAL_UCODE
        NTAG_I2C_API/src/inc
        NTAG_I2C_API/src/UNITY
        )

set(NTAG_I2C_SRC
        HAL_BSP11U6x
        HAL_BSP_LPC8xx
        HAL_BSP_WINDOWS
        HAL_BSPMSP430
        HAL_I2C
        HAL_ISR
        HAL_NTAG
        HAL_TMR
        HAL_UCODE
        inc
        UNITY
        )

set(NTAG_SRCS "")
foreach (DIR in ${NTAG})
            file(GLOB_RECURSE includes
                    ${DIR}/*.cpp ${DIR}/*.c ${DIR}/*.s ${DIR}/*.S)
            list(APPEND NTAG_SRCS ${includes})
endforeach ()

#add_library(ntag_i2c ${NTAG_SRCS})
#include_directories(${NTAG})


add_executable(ntag source/main.cpp
            source/ntag_driver.c
        )
target_link_libraries(ntag mbed-os)

add_custom_target(relay-compile ALL
        COMMAND mbed compile --profile mbed-os/tools/profiles/debug.json
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})