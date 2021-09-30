#include "displays/common.h"
#include "pico/stdlib.h"
#include "hardware/spi.h"

namespace display {
    Device::Device(spi_inst_t *spi, uint cs, uint dc, uint mosi, uint sclk, uint rst, uint bl): SPI{spi}, CS{cs}, DC{dc}, MOSI{mosi}, SLCK{sclk}, RST{rst}, BL{bl} {
        spi_init(SPI, 20000*1000);
        gpio_set_function(MOSI, GPIO_FUNC_SPI);
        gpio_set_function(SLCK, GPIO_FUNC_SPI);
        
        // chip select - default state - high
        gpio_init(CS);
        gpio_set_dir(CS, true);
        gpio_put(CS, true);

        gpio_init(DC);
        gpio_set_dir(DC, true);
        
        if(RST){ 
            gpio_init(RST);
            gpio_set_dir(RST, true);
        }
        if(BL) {
            gpio_init(BL);
            gpio_set_dir(BL, true);
        }
    }

    void Device::backlight(bool enable) {
        if(BL) {
            gpio_put(BL, enable);
        }
    }

    void Device::reset() {
        if(RST) {
            gpio_put(RST, true);
            sleep_ms(5);
            gpio_put(RST, false);
            sleep_ms(20);
            gpio_put(RST, true);
            sleep_ms(150);
        }
    }

    void Device::command(const uint8_t &d) {
        chip_select(true);
        gpio_put(DC, false);
        spi_write_blocking(this->SPI, &d, 1);
        chip_select(false);
    }

    void Device::data(const uint8_t &d) {
        chip_select(true);
        gpio_put(DC, true);
        spi_write_blocking(this->SPI, &d, 1);
        chip_select(false);
    }

    void Device::chip_select(bool select) {
        gpio_put(CS, !select);
    }

    void Device::init(std::vector<cmd_init> &cdl) {
        for(cmd_init &ls : cdl) {
            command(ls.cmd);
            for(uint8_t &dt: ls.data) {
                data(dt);
            }
            if(ls.ms_time) {
                sleep_ms(ls.ms_time);
            }
        }
    }
}