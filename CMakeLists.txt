cmake_minimum_required(VERSION 3.7)

set(SDK "/home/kuba/sw/ai-node-fw-stdln/sdk/kendryte-standalone-sdk")
set(TOOLCHAIN "/home/kuba/sw/kendryte-toolchain/bin")

include("${SDK}/cmake/common.cmake")
project(ai-node-fw)
include("${SDK}/cmake/macros.internal.cmake")

include_directories(${SDK}/lib/arch/include ${SDK}/lib/utils/include ${SDK}/lib/bsp/include ${SDK}/lib/hal/include)
include_directories(
                    src/ src/radio src/mac src/utility
                    src/tasks/ai
                    src/tasks/lora src/tasks/lora/LmHandler src/tasks/lora/LmHandler/packages
                    src/drivers/lcd src/drivers/camera
                   )


add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)

include("${SDK}/cmake/executable.cmake")
