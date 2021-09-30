#ifndef UI_H
#define UI_H

#include "lvgl/lvgl.h"

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
    lv_obj_t * eth0up_speed{nullptr};
    lv_obj_t * eth0down_speed{nullptr};

    // containers
    lv_obj_t * eth0_container{nullptr}; // not available or down, up speed. Tracked in eth0_unavailable
    lv_obj_t * eth0_unavailcontainer{nullptr}; // not available or down, up speed. Tracked in eth0_unavailable
    lv_obj_t * icon_container{nullptr}; // default icon or www offline icon
    lv_obj_t * overheat_container{nullptr}; // empty or overheat logo

    // images
    lv_obj_t * status_icon{nullptr};
    lv_obj_t * overheat_icon{nullptr};
    
    // default styles
    lv_style_t screen_style;
    lv_style_t primary_style;
    lv_style_t secondary_style; // cpu, mem, disk label containers

    // show screen based on usb status
    bool is_splash_screen_active{true};
    lv_obj_t * splash_screen{nullptr};
    lv_obj_t * main_screen{nullptr};

    void create_splash_screen();
    void create_main_screen();
    void main_screen_icon_c(bool);
    void main_screen_net_c(const char *, const char *, bool = false);
    void main_screen_overheat_c(bool);
    
    public:
    Ui(lv_disp_t *);
    void show_splash_screen(bool);
    void update_info(const char *, const char *, const char *, const char *, const char *, bool, bool, bool);
};

#ifdef __cplusplus
}
#endif

#endif