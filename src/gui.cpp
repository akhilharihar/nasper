#include "gui.h"
#include <string>
#include <vector>

LV_IMG_DECLARE(default_icon_img)
LV_IMG_DECLARE(no_internet_icon_img)
LV_IMG_DECLARE(overheat_icon_img)
LV_IMG_DECLARE(splash_screen_img)

// screen container format for generator
struct scrc_abstract {
    std::string name;
	int x;
	int y;
	int width;
	int height;
    bool primary_style{false};
    bool contains_label{true};
};

Ui::Ui(lv_disp_t * disp) {
    // Set Theme
    lv_theme_t * th = lv_theme_basic_init(disp);
    lv_disp_set_theme(disp, th);
    // Create styles
    lv_style_init(&screen_style);
    lv_style_set_text_font(&screen_style, &lv_font_montserrat_48);
    lv_style_set_text_color(&screen_style, lv_color_white());
    lv_style_set_bg_color(&screen_style, primary_color);

    lv_style_init(&primary_style);
    lv_style_set_bg_color(&primary_style, primary_color);
    lv_style_set_border_width(&primary_style, 0);

    lv_style_init(&secondary_style);
    lv_style_set_bg_color(&secondary_style, secondary_color);
    lv_style_set_border_width(&secondary_style, 0);
}

void Ui::show_splash_screen(bool show_splash_screen) {
    if(show_splash_screen) {
        is_splash_screen_active = true;
        if(splash_screen != nullptr) return;
        create_splash_screen();
        lv_scr_load(splash_screen);
        if(main_screen != nullptr) { 
            lv_obj_del(main_screen);
            main_screen = nullptr;
            overheat_icon = nullptr;
            status_icon = nullptr;
        }
    } else {
        is_splash_screen_active = false;
        if(main_screen != nullptr) return;
        create_main_screen();
        lv_scr_load(main_screen);
        if(splash_screen != nullptr) { 
            lv_obj_del(splash_screen);
            splash_screen = nullptr;
        }
    }
}

void Ui::create_splash_screen() {
    splash_screen = lv_obj_create(NULL);
    lv_obj_t * img1 = lv_img_create(splash_screen);
    lv_img_set_src(img1, &splash_screen_img);
    lv_obj_align(img1, LV_ALIGN_CENTER, 0 , 0);
}

void Ui::create_main_screen() {
    main_screen = lv_obj_create(NULL);
    lv_obj_add_style(main_screen, &screen_style, 0);

    std::vector<scrc_abstract> main_screen_containers {
        {"CPU", 10, 10, 140, 75},
        {"MEM", 10, 85, 140, 75},
        {"DISK", 10, 160, 140, 75},
        {"CPUV", 160, 10, 170, 75, true},
        {"MEMV", 160, 85, 170, 75, true},
        {"DISKV", 160, 160, 170, 75, true},
        {"ETH0C", 10, 235, 460, 75, true, false},
        {"ETH0NC", 10, 235, 460, 75, true, false},
        {"STATLC", 330, 10, 140, 165, true, false},
        {"OHTC", 330, 175, 140, 60, true, false}
    };

    for(scrc_abstract &elem : main_screen_containers) {
        lv_obj_t * cont = lv_obj_create(main_screen);
        lv_obj_set_pos(cont, elem.x, elem.y);
        lv_obj_set_size(cont, elem.width, elem.height);
        
        if(elem.primary_style) {
            lv_obj_add_style(cont, &primary_style, 0);
        } else {
            lv_obj_add_style(cont, &secondary_style, 0);
        }

        if(elem.name == "ETH0C") eth0_container = cont;
        if(elem.name == "ETH0NC") eth0_unavailcontainer = cont;
        if(elem.name == "STATLC") icon_container = cont;
        if(elem.name == "OHTC") overheat_container = cont;

        if(!elem.contains_label) {
            continue;
        }

        lv_obj_t * lbl = lv_label_create(cont);
        lv_label_set_text(lbl, elem.name.c_str());
        if(elem.primary_style) {
            lv_obj_align(lbl, LV_ALIGN_LEFT_MID, 0, 0);
        } else {
            lv_obj_align(lbl, LV_ALIGN_RIGHT_MID, 0, 0);
        }

        if(elem.name == "CPUV") cpu_value = lbl;
        if(elem.name == "MEMV") mem_value = lbl;
        if(elem.name == "DISKV") disk_value = lbl;
    }

    lv_obj_set_flex_flow(eth0_container, LV_FLEX_FLOW_ROW);
    
    lv_obj_t *lbl = lv_label_create(eth0_unavailcontainer);
    lv_label_set_text(lbl, "INET Error");
    lv_obj_align(lbl, LV_ALIGN_CENTER, 0, 0);

    eth0up_speed = lv_label_create(eth0_container);
    eth0down_speed = lv_label_create(eth0_container);

    lv_obj_add_flag(eth0_unavailcontainer, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(eth0_container, LV_OBJ_FLAG_HIDDEN);

    main_screen_overheat_c(false);
    main_screen_icon_c(false);
    main_screen_net_c("0", "0", false);
}

void Ui::main_screen_overheat_c(bool overheating) {
    if(overheat_icon == nullptr) {
        overheat_icon = lv_img_create(overheat_container);
        lv_img_set_src(overheat_icon, &overheat_icon_img);
        lv_obj_align(overheat_icon, LV_ALIGN_CENTER, 0 , 0);
        lv_obj_add_flag(overheat_icon, LV_OBJ_FLAG_HIDDEN);
    }
    if(overheating) {
        lv_obj_clear_flag(overheat_icon, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_flag(overheat_icon, LV_OBJ_FLAG_HIDDEN);
    }
}

void Ui::main_screen_icon_c(bool no_internet) {
    if(status_icon == nullptr) {
        status_icon = lv_img_create(icon_container);
        lv_obj_align(status_icon, LV_ALIGN_CENTER, 0 , 0);
    }
    if(no_internet) {
        lv_img_set_src(status_icon, &no_internet_icon_img);
    } else {
        lv_img_set_src(status_icon, &default_icon_img);
    }
}

void Ui::main_screen_net_c(const char * up_speed, const char * down_speed, bool eth0_unavail){
    if(eth0_unavail) {
        lv_obj_add_flag(eth0_container, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(eth0_unavailcontainer, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_flag(eth0_unavailcontainer, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(eth0_container, LV_OBJ_FLAG_HIDDEN);
        lv_label_set_text(eth0up_speed, up_speed);
        lv_label_set_text(eth0down_speed, down_speed);
    }
}

// cpu, mem, disk, ethup, ethdown, eth_unavail, no internet, overheat
void Ui::update_info(const char *cpu, const char *mem, const char *disk, const char *ethup, const char *ethdown, bool eth_unavail, bool no_internet, bool overheating) {
    if(is_splash_screen_active) return;
    lv_label_set_text(cpu_value, cpu);
    lv_label_set_text(mem_value, mem);
    lv_label_set_text(disk_value, disk);

    if(eth_unavail) { // eth0_unavail = flase
        main_screen_net_c("0", "0", true);
        main_screen_icon_c(true);
    } else {
        main_screen_net_c(ethup, ethdown, false);
        main_screen_icon_c(no_internet);
    }
    main_screen_overheat_c(overheating);
}