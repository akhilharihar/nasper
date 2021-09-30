#include "displays/ili9486.h"

#if TFT_ILI9486
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include <string_view>

namespace display {
    Ili9486::Ili9486(spi_inst_t *spi, uint cs, uint dc, uint mosi, uint sclk, uint rst , uint bl) : Device(spi, cs, dc, mosi, sclk, rst, bl) {
        reset();
    }

    void Ili9486::reset() {
        Device::reset();
        init(cd_list);
        rotation(0);
    }

    void Ili9486::command(const uint8_t &d) {
        chip_select(true);
        gpio_put(DC, false);
        spi_write_blocking(this->SPI, 0, 1);
        spi_write_blocking(this->SPI, &d, 1);
        chip_select(false);
    }

    void Ili9486::data(const uint8_t &d) {
        chip_select(true);
        gpio_put(DC, true);
        spi_write_blocking(this->SPI, 0, 1);
        spi_write_blocking(this->SPI, &d, 1);
        chip_select(false);
    }

    void Ili9486::data16(const uint16_t &d) {
        chip_select(true);
        gpio_put(DC, true);
        const uint8_t x{d >> 8};
        const uint8_t y{d & 0xFF};
        spi_write_blocking(this->SPI, &x, 1);
        spi_write_blocking(this->SPI, &y, 1);
        chip_select(false);
    }

    void Ili9486::rotation(short rotation) {
        switch (rotation) {
        case 0:
            command(MADCTL);
            data(0x80 | (1 << 3));
            break;
        case 90:
            command(MADCTL);
            data(0x20 | (1 << 3));
            std::swap(horizontal_px, vertical_px);
            break;
        case 180:
            command(MADCTL);
            data(0x40 | (1 << 3));
            break;
        case 270:
            command(MADCTL);
            data(0xE0 | (1 << 3));
            std::swap(horizontal_px, vertical_px);
            break;
        default:
            break;
        }
    }

    void Ili9486::get_display_size(unsigned short &h_size, unsigned short &v_size) {
        h_size = horizontal_px;
        v_size = vertical_px;
    }
    
}

#endif //TFT_ILI9486