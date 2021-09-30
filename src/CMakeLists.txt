# include_directories(${CMAKE_CURRENT_LIST_DIR}/includes)

add_library(millis STATIC millis.cpp)
target_link_libraries(millis pico_stdlib pico_time)

add_library(displays STATIC displays/common.cpp displays/ili9486.cpp displays/st7735r.cpp)
target_link_libraries(displays pico_stdlib hardware_spi)