

add_library(displays STATIC displays/common.cpp displays/ili9486.cpp displays/st7735r.cpp)
target_link_libraries(displays pico_stdlib hardware_spi)