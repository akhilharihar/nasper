#ifndef UI_H
#define UI_H

#include "lvgl/lvgl.h"
#include <string>

#ifdef __cplusplus
extern "C" {
#endif

class Ui {    
    protected:
    lv_color_t primary_color{lv_color_hex(0x161342)};
    lv_color_t secondary_color{lv_color_hex(0x8787de)};

    // labels
    lv_obj_t * cpu_value{nullptr};
    lv_obj_t * mem_value{nullptr};
    lv_obj_t * disk_value{nullptr};
    lv_obj_t * network_value{nullptr};

    // images
    lv_obj_t * status_icon{nullptr};
    
    // default styles
    lv_style_t main_screen_style;
    lv_style_t primary_style;
    lv_style_t secondary_style; // cpu, mem, disk label containers

    // show screen based on usb status
    bool is_splash_screen_active{true};
    lv_obj_t * splash_screen{nullptr};
    lv_obj_t * main_screen{nullptr};

    void create_splash_screen();
    void create_main_screen();
    void set_main_screen_status_icon(unsigned short);
    
    public:
    Ui(lv_disp_t *);
    void show_splash_screen(bool);
    void update_info(std::string &, std::string &, std::string &, std::string &, std::string &, std::string &);
};

#ifdef __cplusplus
}
#endif

#endif