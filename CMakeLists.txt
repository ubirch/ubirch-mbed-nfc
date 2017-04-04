cmake_minimum_required(VERSION 3.5)
set(CMAKE_BUILD_TYPE Debug)

project(MBED_NFC C CXX)

# == MBED OS 5 settings ==
set(MCU MK82FN256VLL15)
set(TARGET K82F)
set(BOARD UBRIDGE)
#set(MCU MKL82Z128VLK7)
#set(TARGET KL82Z)
#set(BOARD USENSE)

set(FEATURES netsocket)

add_definitions(
        -DTOOLCHAIN_GCC
        -DCPU_${MCU}
        -DTARGET_${TARGET}
        -DTARGET_${BOARD}
        -DMBED_CONF_RTOS_PRESENT
        -DMBED_CONF_NSAPI_PRESENT
        -DMBED_CONF_EVENTS_PRESENT
        -D__MBED__
        -DDEVICE_RTC
        -DDEVICE_I2C
        -DDEVICE_SPI
        -DDEVICE_SERIAL
        -DDEVICE_TRNG
        -DDEVICE_INTERRUPTIN
        -DMBED_CONF_EVENTS_PRESENT=1
        -DMBED_CONF_RTOS_PRESENT=1
        -DMBED_CONF_PLATFORM_STDIO_BAUD_RATE=115200
        -DMBED_CONF_NSAPI_PRESENT=1
        -DMBED_CONF_PLATFORM_DEFAULT_SERIAL_BAUD_RATE=115200
        -DMBED_CONF_PLATFORM_STDIO_FLUSH_AT_EXIT=0
        -DMBED_CONF_PLATFORM_STDIO_CONVERT_NEWLINES=0
)

set(MBED_OS
        mbed-os/targets/TARGET_Freescale/TARGET_MCUXpresso_MCUS/TARGET_${TARGET}/TARGET_${BOARD}
        mbed-os/targets/TARGET_Freescale/TARGET_MCUXpresso_MCUS/TARGET_${TARGET}/device/TOOLCHAIN_GCC_ARM
        mbed-os/targets/TARGET_Freescale/TARGET_MCUXpresso_MCUS/TARGET_${TARGET}/drivers
        mbed-os/targets/TARGET_Freescale/TARGET_MCUXpresso_MCUS/TARGET_${TARGET}/device
        mbed-os/targets/TARGET_Freescale/TARGET_MCUXpresso_MCUS/TARGET_${TARGET}
        mbed-os/targets/TARGET_Freescale/TARGET_MCUXpresso_MCUS/api
        mbed-os/targets/TARGET_Freescale
        mbed-os/rtos/rtx/TARGET_CORTEX_M/TARGET_RTOS_M4_M7
        mbed-os/rtos/rtx/TARGET_CORTEX_M
        mbed-os/rtos
        mbed-os/features
        mbed-os/platform
        mbed-os/hal/storage_abstraction
        mbed-os/hal
        mbed-os/events/enqueue
        mbed-os/events
        mbed-os/drivers
        mbed-os/cmsis
        mbed-os
        )

set(MBED_OS_SRCS "")
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

#set(NTAG
#        NTAG_I2C_API/src/HAL_BSP11U6x
#        NTAG_I2C_API/src/HAL_BSP_LPC8xx
#        NTAG_I2C_API/src/HAL_BSP_WINDOWS
#        NTAG_I2C_API/src/HAL_BSPMSP430
#        NTAG_I2C_API/src/HAL_I2C
#        NTAG_I2C_API/src/HAL_ISR
#        NTAG_I2C_API/src/HAL_NTAG
#        NTAG_I2C_API/src/HAL_TMR
#        NTAG_I2C_API/src/HAL_UCODE
#        NTAG_I2C_API/src/inc
#        NTAG_I2C_API/src/UNITY
#        )
#
#set(NTAG_I2C_SRC
#        HAL_BSP11U6x
#        HAL_BSP_LPC8xx
#        HAL_BSP_WINDOWS
#        HAL_BSPMSP430
#        HAL_I2C
#        HAL_ISR
#        HAL_NTAG
#        HAL_TMR
#        HAL_UCODE
#        inc
#        UNITY
#        )
#
#set(NTAG_SRCS "")
#foreach (DIR in ${NTAG})
#            file(GLOB_RECURSE includes
#                    ${DIR}/*.cpp ${DIR}/*.c ${DIR}/*.s ${DIR}/*.S)
#            list(APPEND NTAG_SRCS ${includes})
#endforeach ()
#
##add_library(ntag_i2c ${NTAG_SRCS})
##include_directories(${NTAG})


add_executable(nfc example/rw_eeprom.cpp
            example/rw_sram.cpp
            source/ntag_driver.c
            source/ntag_bridge.c
            source/ntag_interface.cpp)
target_link_libraries(nfc mbed-os)

add_custom_target(nfc-compile ALL
        COMMAND mbed compile --profile mbed-os/tools/profiles/debug.json
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})