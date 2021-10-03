#include "gui.h"
#include <vector>

LV_IMG_DECLARE(default_icon_img)
LV_IMG_DECLARE(no_internet_icon_img)
LV_IMG_DECLARE(overheat_icon_img)
LV_IMG_DECLARE(splash_screen_img)

// screen container format for generator
struct scrc_abstract {
    std::string name;
	int x_pos;
	int y_pos;
	int width;
	int height;
    bool primary_style{true};
    bool contains_label{false};
    int lbl_alignment{9};
};

Ui::Ui(lv_disp_t * disp) {
    // Set Theme
    lv_theme_t * th = lv_theme_basic_init(disp);
    lv_disp_set_theme(disp, th);
    // Create styles
    lv_style_init(&main_screen_style);
    lv_style_set_text_font(&main_screen_style, &lv_font_montserrat_40);
    lv_style_set_text_color(&main_screen_style, lv_color_white());
    lv_style_set_bg_color(&main_screen_style, primary_color);

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
            cpu_value = nullptr;
            mem_value = nullptr;
            disk_value = nullptr;
            network_value = nullptr;
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
    lv_obj_add_style(main_screen, &main_screen_style, 0);

    /* Containers
    CPU : label - "CPU"
    CPU value : label - CPU % Usage
    MEM : label - "MEM"
    MEM value : label - MEM % Usage
    Disk : label - "DISK"
    Disk value : label - Disk % Usage
    Main Icon : Image - all_ok or no_internet or overheat
    Network : label - ("UP_SPEED", upload speed value, "DOWN_SPEED", download speed value) or INET_ERRROR(when not connected to any network)
    */

   std::vector<scrc_abstract> mnscreen_containers {
       {"CPU", 0, 170, 160, 75, false, true},
       {"MEM", 160, 170, 160, 75, false, true},
       {"DISK", 320, 170, 160, 75, false, true},
       {"CPU_V", 0, 245, 160, 75, true, true},
       {"MEM_V", 160, 245, 160, 75, true, true},
       {"DISK_V", 320, 245, 160, 75, true, true},
       {"NET", 0, 0, 320, 170, true, true},
       {"MAIN_ICON", 320, 0, 160, 170, true, false}
    };


    for(scrc_abstract &elem : mnscreen_containers) {

        lv_obj_t * cont = lv_obj_create(main_screen);
        lv_obj_set_pos(cont, elem.x_pos, elem.y_pos);
        lv_obj_set_size(cont, elem.width, elem.height);

        if(elem.primary_style) {
            lv_obj_add_style(cont, &primary_style, 0);
        } else {
            lv_obj_add_style(cont, &secondary_style, 0);
        }

        if(elem.name == "MAIN_ICON") {
            status_icon = lv_img_create(cont);
            lv_obj_align(status_icon, LV_ALIGN_CENTER, 0 , 0);
            lv_img_set_src(status_icon, &default_icon_img);
            continue;
        }

        if(!elem.contains_label) {
            continue;
        }

        lv_obj_t * lbl = lv_label_create(cont);
        lv_label_set_text(lbl, elem.name.c_str());
        lv_obj_align(lbl, elem.lbl_alignment, 0, 0);

        if(elem.name == "CPU_V") cpu_value = lbl;
        if(elem.name == "MEM_V") mem_value = lbl;
        if(elem.name == "DISK_V") disk_value = lbl;
        if(elem.name == "NET") network_value = lbl;
    }
}

void Ui::set_main_screen_status_icon(unsigned short icon_type) {
    switch(icon_type) {
        case 1:
            lv_img_set_src(status_icon, &overheat_icon_img);
            break;
        case 2:
            lv_img_set_src(status_icon, &no_internet_icon_img);
            break;
        default:
            lv_img_set_src(status_icon, &default_icon_img);
    }
}

// cpu, mem, disk, ethup, ethdown, eth_unit, eth_unavail, no internet, overheat
void Ui::update_info(std::string &cpu, std::string &mem, std::string &disk, std::string &ethdown, std::string &ethup, std::string &eth_unit, std::string &eth_unavail, std::string &no_internet, std::string &overheating) {
    if(is_splash_screen_active) return;
    lv_label_set_text(cpu_value, cpu.c_str());
    lv_label_set_text(mem_value, mem.c_str());
    lv_label_set_text(disk_value, disk.c_str());

    if(eth_unavail == "1") {
        lv_label_set_text(network_value, "Network\n N\\A");
    } else {
        std::string net_string{};
        net_string = "\xef\x80\x99" + std::string(" : ") +  ethdown + std::string(" Mb/s") +"\n\xef\x82\x93" + std::string(" : ") + ethup + std::string(" Mb/s");
        lv_label_set_text(network_value, net_string.c_str());
    }
    if(overheating== "1") {
        set_main_screen_status_icon(1);
    } else if (eth_unavail == "1" || no_internet == "1") {
        set_main_screen_status_icon(2);
    } else {
        set_main_screen_status_icon(0);
    }
}