cmake_minimum_required(VERSION 3.7)

set(SDK "sdk/kendryte-standalone-sdk")
#set(TOOLCHAIN "/home/kuba/sw/kendryte-toolchain/bin")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include("${SDK}/cmake/common.cmake")
project(ai-node-fw)
include("${SDK}/cmake/macros.internal.cmake")

if(APP STREQUAL "parking")
	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DAPP_PARKING")
else()
	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DAPP_PERSON")
endif()

include_directories(${SDK}/lib/arch/include ${SDK}/lib/utils/include ${SDK}/lib/bsp/include ${SDK}/lib/hal/include)
include_directories(
                    src/
                    src/radio src/radio/board
                    src/mac
                    src/utility
                    src/tasks/ai
                    src/tasks/lora src/tasks/lora/LmHandler src/tasks/lora/LmHandler/packages
                    src/drivers/lcd src/drivers/camera src/drivers/sd src/drivers/flash src/drivers/ioexp src/drivers/adc
                   )


add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)

include("${SDK}/cmake/executable.cmake")
