#include "tft_conf.h"

#ifndef COMMON_H
#define COMMON_H

#include <vector>
#include "hardware/spi.h"

#ifdef __cplusplus
extern "C" {
#endif

namespace display {
    struct cmd_init {
        unsigned char cmd;
        std::vector<unsigned char> data;
        uint32_t ms_time{};
    };
    constexpr unsigned char NOP     {0x00}; // NOP
    constexpr unsigned char SWRST {0x01}; //soft reset
    constexpr unsigned char RDDID   {0x04}; // Read display identification 
    constexpr unsigned char RDDST   {0x09}; // Read Display Status 
    constexpr unsigned char SLPIN   {0x10}; // Sleep in
    constexpr unsigned char SLPOUT  {0x11}; // sleep out
    constexpr unsigned char PTLON   {0x12}; // Partial Mode ON 
    constexpr unsigned char NORON   {0x13}; // Normal Display Mode ON 
    constexpr unsigned char INVOFF  {0x20}; // Display Inversion OFF 
    constexpr unsigned char INVON   {0x21}; // Display Inversion ON 
    constexpr unsigned char DISPOFF {0x28}; // Display OFF
    constexpr unsigned char DISPON  {0x29}; // Display ON
    constexpr unsigned char CASET   {0x2A}; // Column Address Set
    constexpr unsigned char RASET   {0x2B}; // Row Address Set 
    constexpr unsigned char RAMWR   {0x2C}; // Memory Write
    constexpr unsigned char RAMRD   {0x2E}; // Memory Read
    constexpr unsigned char PTLAR   {0x30}; // Partial Area
    constexpr unsigned char MADCTL  {0x36}; // Memory Access Control 
    constexpr unsigned char ITFPXFT  {0x3A}; // Interface Pixel Format
    constexpr unsigned char RDID1   {0xDA}; // Read ID1
    constexpr unsigned char RDID2   {0xDB}; // Read ID2
    constexpr unsigned char RDID3   {0xDC}; // Read ID3
    constexpr unsigned char ITFCTR1 {0xB0}; // Interface Mode Control 
    constexpr unsigned char FRMCTR1 {0xB1}; // Frame Rate Control（In Normal Mode/Full Colors）
    constexpr unsigned char FRMCTR2 {0xB2}; // Frame Rate Control（In Idle Mode/8 colors）
    constexpr unsigned char FRMCTR3 {0xB3}; // Frame Rate Control（In Partial Mode/Full colors）
    constexpr unsigned char INVCTR  {0xB4}; // Display Inversion Control 
    constexpr unsigned char DISSET5 {0xB6}; // Display Function Control 
    constexpr unsigned char PWCTR1  {0xC0}; // Power Control 1 
    constexpr unsigned char PWCTR2  {0xC1}; // Power Control 2
    constexpr unsigned char PWCTR3  {0xC2}; // Power Control 3
    constexpr unsigned char PWCTR4  {0xC3}; // Power Control 4 
    constexpr unsigned char PWCTR5  {0xC4}; // Power Control 5
    constexpr unsigned char VMCTR1  {0xC5}; // VCOM Control 1 
    constexpr unsigned char RDID4   {0xD3}; // Read ID4 
    constexpr unsigned char PGMCTR {0xE0}; // Positive Gamma Control
    constexpr unsigned char NGMCTR {0xE1}; // Negative Gamma Control
    constexpr unsigned char DGMCTR1 {0xE2}; // Digital Gamma Control 1 
    constexpr unsigned char DGMCTR2 {0xE3}; // Digital Gamma Control 2

    class Device {
        protected:
        spi_inst_t *SPI;
        const uint CS;
        const uint DC;
        const uint MOSI;
        const uint SLCK;
        const uint RST;
        const uint BL;
        
        public:
        Device(spi_inst_t *, uint, uint, uint, uint, uint = 0, uint = 0);
        void backlight(bool);
        void reset();
        void command(const unsigned char &);
        void data(const unsigned char &);
        void chip_select(bool);
        void init(std::vector<cmd_init> &);
        virtual void rotation(short) = 0;
        virtual void get_display_size(unsigned short &, unsigned short &) = 0;
    };
}

#ifdef __cplusplus
}
#endif

#endif