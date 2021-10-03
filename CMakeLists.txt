cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

include(lib/pico-sdk/pico_sdk_init.cmake)

project(nasper 
VERSION 0.1
)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

include_directories(${CMAKE_CURRENT_LIST_DIR}/src/includes)
include_directories(${CMAKE_PROJECT_NAME} lib/)

add_subdirectory(lib/lvgl)

add_subdirectory(src)
target_compile_definitions(lvgl PUBLIC LV_CONF_INCLUDE_SIMPLE)
target_link_libraries(lvgl millis)

file(GLOB_RECURSE IMAGES src/images/*.c)

add_executable(${CMAKE_PROJECT_NAME} src/main.cpp src/gui.cpp src/usb_descriptors.c ${IMAGES})

target_link_libraries(${CMAKE_PROJECT_NAME}
        pico_stdlib
        lvgl
        displays
        millis
        pico_multicore
        tinyusb_device
)

pico_enable_stdio_uart(${CMAKE_PROJECT_NAME} 1)
pico_enable_stdio_usb(${CMAKE_PROJECT_NAME} 0)

pico_add_extra_outputs(${CMAKE_PROJECT_NAME})

file(WRITE ${CMAKE_BINARY_DIR}/tmp/flash_elf 
"#!/usr/bin/env bash
rsync -az --info=progress2 ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}.elf pi@raspberrypi.local:/home/pi/
if [[ $? -gt 0 ]]
then
exit 1
fi
ssh pi@raspberrypi.local 'openocd -f interface/raspberrypi-swd.cfg -f target/rp2040.cfg -c \"program /home/pi/${CMAKE_PROJECT_NAME}.elf verify reset exit\"'
")
file(COPY ${CMAKE_BINARY_DIR}/tmp/flash_elf
DESTINATION ${CMAKE_BINARY_DIR}
FILE_PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
file(REMOVE_RECURSE ${CMAKE_BINARY_DIR}/tmp)
