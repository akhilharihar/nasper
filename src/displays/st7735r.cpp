#include "displays/st7735r.h"

#if TFT_ST7735R
#include <string_view>

namespace display {
    St7735r::St7735r(spi_inst_t *spi, uint cs, uint dc, uint mosi, uint sclk, uint rst , uint bl) : Device(spi, cs, dc, mosi, sclk, rst, bl) {
        reset();
    }

    void St7735r::reset() {
        Device::reset();
        init(cd_list);
        rotation(0);
    }
    void St7735r::rotation(short rotation) {
        constexpr short my{1 << 7};
        constexpr short mx{1 << 6};
        constexpr short mv{1 << 5};
        switch (rotation) {
        case 0:
            command(MADCTL);
            data(mx | my | (1 << 3));
            break;
        case 90:
            command(MADCTL);
            data(my | mv | (1 << 3));
            std::swap(horizontal_px, vertical_px);
            break;
        case 180:
            command(MADCTL);
            data(1 << 3);
            break;
        case 270:
            command(MADCTL);
            data(mx | mv | (1 << 3));
            std::swap(horizontal_px, vertical_px);
            break;
        default:
            break;
        }
    }

    void St7735r::get_display_size(unsigned short &h_size, unsigned short &v_size) {
        h_size = horizontal_px;
        v_size = vertical_px;
    }
}

#endif // end TFT_ST7735R